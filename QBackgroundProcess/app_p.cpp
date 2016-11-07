#include "app_p.h"
#include <QRegularExpression>
#include <QThread>
#include <QDir>
#include <QProcess>
#include <iostream>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif
using namespace QBackgroundProcess;

//logging category
Q_LOGGING_CATEGORY(QBackgroundProcess::loggingCategory, "QBackgroundProcess")

const QString AppPrivate::masterArgument(QStringLiteral("__qbckgrndprcss$start#master~"));
const QString AppPrivate::purgeArgument(QStringLiteral("purge_master"));
const QString AppPrivate::startArgument(QStringLiteral("start"));
const QString AppPrivate::messageFormat(QStringLiteral("%{if-debug}[Debug]    %{endif}"
													   "%{if-info}[Info]     %{endif}"
													   "%{if-warning}[Warning]  %{endif}"
													   "%{if-critical}[Critical] %{endif}"
													   "%{if-fatal}[Fatal]    %{endif}"
													   "%{if-category}%{category}: %{endif}"
													   "%{message}\n"));
QPointer<GlobalTerminal> AppPrivate::debugTerm(nullptr);

QString AppPrivate::generateSingleId(const QString &seed)
{
	auto fullId = QCoreApplication::applicationName().toLower();
	fullId.remove(QRegularExpression(QStringLiteral("[^a-zA-Z0-9_]")));
	fullId.truncate(8);
	fullId.prepend(QStringLiteral("qbackproc-"));
	QByteArray hashBase = (QCoreApplication::organizationName() +
						   QCoreApplication::organizationDomain() +
						   seed).toUtf8();
	fullId += QLatin1Char('-') +
			  QString::number(qChecksum(hashBase.data(), hashBase.size()), 16) +
			  QLatin1Char('-');

#ifdef Q_OS_WIN
	DWORD sessID;
	if(::ProcessIdToSessionId(::GetCurrentProcessId(), &sessID))
		fullId += QString::number(sessID, 16);
#else
	fullId += QString::number(::getuid(), 16);
#endif

	return fullId;
}

AppPrivate *AppPrivate::p_ptr(App *app)
{
	return app->d_ptr;
}

void AppPrivate::termDebugMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	std::cerr << qFormatLogMessage(type, context, msg).toStdString();
	std::cerr.flush();

	if(type == QtMsgType::QtFatalMsg)
		qt_assert_x(context.function, qUtf8Printable(msg), context.file, context.line);
}

void AppPrivate::masterDebugMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	if(debugTerm) {
		debugTerm->write(qFormatLogMessage(type, context, msg).toUtf8());
		debugTerm->flush();

		if(type == QtMsgType::QtFatalMsg)
			qt_assert_x(context.function, qUtf8Printable(msg), context.file, context.line);
	} else
		termDebugMessage(type, context, msg);
}

AppPrivate::AppPrivate(App *q_ptr) :
	QObject(q_ptr),
	running(false),
	autoStart(false),
	ignoreExtraStart(false),
	autoDelete(false),
	autoKill(false),
	instanceId(),
	masterLock(nullptr),
	masterServer(nullptr),
	startupFunc(),
	shutdownFunc(),
	master(nullptr),
	q_ptr(q_ptr)
{}

void AppPrivate::setInstanceId(const QString &id)
{
	this->instanceId = id;
	auto lockPath = QDir::temp().absoluteFilePath(id + QStringLiteral(".lock"));
	this->masterLock.reset(new QLockFile(lockPath));
	this->masterLock->setStaleLockTime(0);
}

int AppPrivate::initControlFlow()
{
	//start/make master
	auto args = QCoreApplication::arguments();
	args.removeFirst();
	if(args.size() > 0) {
		if(args[0] == masterArgument) {
			args.removeFirst();
			return this->makeMaster(args);
		} else if(args[0] == purgeArgument) {
			args.removeFirst();
			return this->purgeMaster(args);
		} else if(args[0] == startArgument)
			return this->startMaster(args);
	}

	//neither start nor make master --> "normal" client or autostart
	if(this->autoStart)
		return this->startMaster(args, true);
	else
		return this->testMasterRunning(args);
}

int AppPrivate::makeMaster(const QStringList &arguments)
{
	//create local server --> do first to make shure clients only see a "valid" master lock if the master started successfully
	this->masterServer = new QLocalServer(this);
	connect(this->masterServer, &QLocalServer::newConnection,
			this, &AppPrivate::newTerminalConnected,
			Qt::QueuedConnection);
	if(!this->masterServer->listen(this->instanceId)) {
		qCCritical(loggingCategory) << "Failed to create local server with error:"
					<< qUtf8Printable(this->masterServer->errorString());
		return EXIT_FAILURE;
	}

	//get the lock
	if(!this->masterLock->tryLock(5000)) {//wait at most 5 sec
		this->masterServer->close();
		qCCritical(loggingCategory) << "Unable to start master process. Failed with lock error:"
					<< qUtf8Printable(this->masterLock->error());
		return EXIT_FAILURE;
	} else {
		auto res = this->q_ptr->startupApp(arguments);
		if(res != EXIT_SUCCESS) {
			//cleanup
			this->masterServer->close();
			this->masterLock->unlock();
		}
		return res;
	}
}

int AppPrivate::startMaster(const QStringList &arguments, bool hideWarning)
{
	//check if master already lives
	if(this->masterLock->tryLock()) {//no master alive
		auto ok = false;

		auto args = arguments;
		args.replace(0, masterArgument);
		if(QProcess::startDetached(QCoreApplication::applicationFilePath(), args)) {//start MASTER with additional start params
			//wait for the master to start
			this->masterLock->unlock();
			for(auto i = 0; i < 50; i++) {//wait at most 5 sec
				qint64 pid;
				QString hostname;
				QString appname;
				if(this->masterLock->getLockInfo(&pid, &hostname, &appname)) {
					ok = true;
					break;
				} else
					QThread::msleep(100);
			}
		}

		if(ok) {//master started --> start to connect (after safety delay)
			QThread::msleep(250);
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, arguments),
									  Q_ARG(bool, true));
			return EXIT_SUCCESS;
		} else {
			qCCritical(loggingCategory) << "Failed to start master process! No master lock was detected.";
			return EXIT_FAILURE;
		}
	} else {//master is running --> ok
		if(this->ignoreExtraStart) {
			qCWarning(loggingCategory) << "Start commands ignored because master is already running!\n"
						  "The terminal will connect with an empty argument list!";
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, QStringList()),
									  Q_ARG(bool, false));
			return EXIT_SUCCESS;
		} else {
			if(!hideWarning)
				qCWarning(loggingCategory) << "Master is already running. Start arguments will be passed to it as is";
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, arguments),
									  Q_ARG(bool, false));
			return EXIT_SUCCESS;
		}
	}
}

int AppPrivate::testMasterRunning(const QStringList &arguments)
{
	if(this->masterLock->tryLock()) {
		this->masterLock->unlock();
		qCCritical(loggingCategory) << "Master process is not running! Please launch it by using:"
					<< QCoreApplication::applicationFilePath() + QStringLiteral(" start");
		return EXIT_FAILURE;
	} else {
		QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
								  Q_ARG(QStringList, arguments),
								  Q_ARG(bool, false));
		return EXIT_SUCCESS;
	}
}

int AppPrivate::purgeMaster(const QStringList &arguments)
{
	if(arguments.isEmpty() || arguments[0] != "accept") {
		std::cout << "Are you shure you want to purge the master lock and server?\n"
				  << "Only do this if the master process is not running anymore, but the lock/server "
					 "are not available (for example after a crash)\n"
				  << "Purging while the master process is still running will crash it.\n"
				  << "Press (y) to purge, or (n) to cancel:";
		std::cout.flush();
		char res = (char)std::cin.get();
		if(res != 'y' && res != 'Y')
			return EXIT_FAILURE;
	}

	auto res = 0;

	if(!this->masterLock->isLocked() || this->masterLock->removeStaleLockFile())
		std::cout << "Master lockfile successfully removed" << std::endl;
	else {
		std::cout << "Failed to remove master lockfile" << std::endl;
		res |= 0x02;
	}

	if(QLocalServer::removeServer(this->instanceId))
		std::cout << "Master server successfully removed" << std::endl;
	else {
		std::cout << "Failed to remove master server" << std::endl;
		res |= 0x04;
	}

	return res == 0 ? -1 : res;
}

void AppPrivate::newTerminalConnected()
{
	while(this->masterServer->hasPendingConnections()) {
		auto termp = new TerminalPrivate(this->masterServer->nextPendingConnection(), this);
		connect(termp, &TerminalPrivate::statusLoadComplete,
				this, &AppPrivate::terminalLoaded);
	}
}

void AppPrivate::terminalLoaded(TerminalPrivate *terminal, bool success)
{
	if(success) {		
		auto rTerm = new Terminal(terminal, this);
		rTerm->setAutoDelete(this->autoDelete);
		connect(rTerm, &Terminal::destroyed, this, [=](){
			this->activeTerminals.removeOne(rTerm);
			emit this->q_ptr->connectedTerminalsChanged(this->activeTerminals, App::QPrivateSignal());
		});
		this->activeTerminals.append(rTerm);
		emit this->q_ptr->connectedTerminalsChanged(this->activeTerminals, App::QPrivateSignal());
		emit this->q_ptr->commandReceived(rTerm->arguments(), App::QPrivateSignal());

		//test stop command
		if(!rTerm->arguments().isEmpty() && rTerm->arguments().first() == QStringLiteral("stop"))
			this->stopMaster(rTerm);

		if(this->autoKill) {
			rTerm->setAutoDelete(true);
			rTerm->disconnectTerminal();
		} else
			emit this->q_ptr->newTerminalConnected(rTerm, App::QPrivateSignal());
	} else
		terminal->deleteLater();
}

void AppPrivate::stopMaster(Terminal *term)
{
	int eCode = EXIT_SUCCESS;
	if(this->q_ptr->shutdownApp(term, eCode)) {
		foreach(auto termin, this->activeTerminals)
			termin->flush();
		QMetaObject::invokeMethod(this, "doExit", Qt::QueuedConnection,
								  Q_ARG(int, eCode));
	}
}

void AppPrivate::doExit(int code)
{
	QCoreApplication::exit(code);
}

void AppPrivate::beginMasterConnect(const QStringList &arguments, bool isStarter)
{
	this->master = new MasterConnecter(this->instanceId, arguments, isStarter, this);
}
