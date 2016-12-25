#include "app_p.h"
#include <QRegularExpression>
#include <QThread>
#include <QDir>
#include <QProcess>
#include <iostream>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <QDir>
#include <QStandardPaths>
#else
#include <unistd.h>
#include <sys/types.h>
#endif
using namespace QtBackgroundProcess;

//logging category
Q_LOGGING_CATEGORY(QtBackgroundProcess::loggingCategory, "QtBackgroundProcess")

bool AppPrivate::p_valid = false;

const QString AppPrivate::masterArgument(QStringLiteral("__qbckgrndprcss$start#master~"));
const QString AppPrivate::purgeArgument(QStringLiteral("purge_master"));
const QString AppPrivate::startArgument(QStringLiteral("start"));
const QString AppPrivate::terminalMessageFormat(QStringLiteral("%{if-debug}[Debug]    %{endif}"
															   "%{if-info}[Info]     %{endif}"
															   "%{if-warning}[Warning]  %{endif}"
															   "%{if-critical}[Critical] %{endif}"
															   "%{if-fatal}[Fatal]    %{endif}"
															   "%{if-category}%{category}: %{endif}"
															   "%{message}\n"));
const QString AppPrivate::masterMessageFormat(QStringLiteral("[%{time} "
															 "%{if-debug}Debug]    %{endif}"
															 "%{if-info}Info]     %{endif}"
															 "%{if-warning}Warning]  %{endif}"
															 "%{if-critical}Critical] %{endif}"
															 "%{if-fatal}Fatal]    %{endif}"
															 "%{if-category}%{category}: %{endif}"
															 "%{message}\n"));

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

AppPrivate *AppPrivate::p_ptr()
{
	return qApp->d_ptr;
}

void AppPrivate::qbackProcMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	auto message = qFormatLogMessage(type, context, msg).toUtf8();
	auto any = false;

	if(p_valid) {
		auto self = p_ptr();

		if(self->debugTerm) {
			self->debugTerm->write(message);
			self->debugTerm->flush();
			any = true;
		}

		if(self->logFile && self->logFile->isWritable()) {
			self->logFile->write(message);
			self->logFile->flush();
			any = true;
		}
	}

	if(!any) {
		std::cerr << qFormatLogMessage(type, context, msg).toStdString();
		std::cerr.flush();
	}

	if(type == QtMsgType::QtFatalMsg)
		qt_assert_x(context.function, qUtf8Printable(msg), context.file, context.line);
}

AppPrivate::AppPrivate(App *q_ptr) :
	QObject(q_ptr),
	running(false),
	masterLogging(false),
	autoStart(false),
	ignoreExtraStart(false),
	autoDelete(false),
	autoKill(false),
	instanceId(),
	masterLock(nullptr),
	masterServer(nullptr),
	parserFunc(),
	startupFunc(),
	shutdownFunc(),
	master(nullptr),
	debugTerm(nullptr),
	logFile(nullptr),
	q_ptr(q_ptr)
{}

void AppPrivate::setInstanceId(const QString &id)
{
	this->instanceId = id;
	auto lockPath = QDir::temp().absoluteFilePath(id + QStringLiteral(".lock"));
	this->masterLock.reset(new QLockFile(lockPath));
	this->masterLock->setStaleLockTime(0);
}

void AppPrivate::setupDefaultParser(QCommandLineParser &parser, bool useShortOptions)
{
	parser.addHelpOption();
	if(!QCoreApplication::applicationVersion().isEmpty())
		parser.addVersionOption();

	QStringList DParams("detached");
	QStringList lParams({"log", "loglevel"});
	QStringList LParams("logpath");
	if(useShortOptions) {
		DParams.prepend("D");
		lParams.prepend("l");
		LParams.prepend("L");
	}

	parser.addPositionalArgument("<command>",
								 "A control command to control the background application. "
								 "Possible options are:\n"
								 " - start: starts the application\n"
								 " - stop: stops the application\n"
								 " - purge_master: purges local servers and lockfiles, in case the master process crashed. "
								 "Pass \"--accept\" as second parameter, if you want to skip the prompt.",
								 "[start|stop|purge_master]");

	parser.addOption({
						 DParams,
						 "It set, the terminal will only pass it's arguments to the master, and automatically finish after."
					 });
	parser.addOption({
						 lParams,
						 "Set the desired log <level>. Possible values are:\n"
						 " - 0: log nothing\n"
						 " - 1: critical errors only\n"
						 " - 2: like 1 plus warnings\n"
					 #ifdef QT_NO_DEBUG
						 " - 3: like 2 plus information messages (default)\n"
						 " - 4: verbose - log everything",
						 "level",
						 "3"
					 #else
						 " - 3: like 2 plus information messages\n"
						 " - 4: verbose - log everything (default)",
						 "level",
						 "4"
					 #endif
					 });

	QString defaultPath;
#ifdef Q_OS_UNIX
	if(QFileInfo("/var/log").isWritable())
		defaultPath = QStringLiteral("/var/log/%1.log").arg(QCoreApplication::applicationName());
	else
#endif
	{
		auto basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
		QDir(basePath).mkpath(".");
		defaultPath = QStringLiteral("%1/%2.log")
			.arg(basePath)
			.arg(QCoreApplication::applicationName());
	}

	parser.addOption({
						 LParams,
						 QStringLiteral("Overwrites the default log <path>. The default path is platform and application specific. "
						 "For this instance, it defaults to \"%1\". NOTE: The application can override the value internally. "
						 "Pass an empty string (--logpath \"\") to disable logging to a file.").arg(defaultPath),
						 "path",
						 defaultPath
					 });
	parser.addOption({
						 "terminallog",
						 "Sets the log <level> for terminal only messages. This does not include messages forwarded from the master. "
						 "Log levels are the same as for the <loglevel> option.",
						 "level",
					 #ifdef QT_NO_DEBUG
						 "3"
					 #else
						 "4"
					 #endif
					 });

	parser.addOption({
						 "accept",
						 "purge_master only: skips the prompt and purges automatically."
					 });
}

void AppPrivate::updateLoggingMode(int level)
{
	QString logStr;
	switch (level) {
	case 0:
		logStr.prepend("\n*.critical=false");
	case 1:
		logStr.prepend("\n*.warning=false");
	case 2:
		logStr.prepend("\n*.info=false");
	case 3:
		logStr.prepend("*.debug=false");
	case 4:
		break;
	default:
		return;
	}
	QLoggingCategory::setFilterRules(logStr);
}

void AppPrivate::updateLoggingPath(const QString &path)
{
	if(this->logFile) {
		this->logFile->close();
		this->logFile->deleteLater();
		this->logFile.clear();
	}
	if(!path.isEmpty()) {
		this->logFile = new QFile(path, this);
		if(!this->logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
			this->logFile->deleteLater();
			this->logFile.clear();
		}
	}
}

int AppPrivate::initControlFlow(const QCommandLineParser &parser)
{
	//start/make master
	auto args = parser.positionalArguments();
	if(args.size() > 0) {
		if(args[0] == masterArgument)
			return this->makeMaster(parser);
		else if(args[0] == purgeArgument)
			return this->purgeMaster(parser);
		else if(args[0] == startArgument)
			return this->startMaster();
	}

	//neither start nor make master --> "normal" client or autostart
	if(this->autoStart)
		return this->startMaster(true);
	else
		return this->testMasterRunning();
}

int AppPrivate::makeMaster(const QCommandLineParser &parser)
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
		//setup master logging stuff
		qSetMessagePattern(AppPrivate::masterMessageFormat);
		if(this->masterLogging)
			this->debugTerm = new GlobalTerminal(this, true);
		this->updateLoggingMode(parser.value("loglevel").toInt());
		this->updateLoggingPath(parser.value("logpath"));

		auto res = this->q_ptr->startupApp(parser);
		if(res != EXIT_SUCCESS) {
			//cleanup
			this->masterServer->close();
			this->masterLock->unlock();
		}

		return res;
	}
}

int AppPrivate::startMaster(bool isAutoStart)
{
	auto arguments = QCoreApplication::arguments();
	arguments.removeFirst();//remove app name
	//check if master already lives
	if(this->masterLock->tryLock()) {//no master alive
		auto ok = false;

		auto args = arguments;
		if(!isAutoStart)
			args.removeOne(startArgument);
		args.prepend(masterArgument);
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
									  Q_ARG(QStringList, arguments),//send original arguments
									  Q_ARG(bool, true));
			return EXIT_SUCCESS;
		} else {
			qCCritical(loggingCategory) << "Failed to start master process! No master lock was detected.";
			return EXIT_FAILURE;
		}
	} else {//master is running --> ok
		if(!isAutoStart && this->ignoreExtraStart) {// ignore only on normal starts, not on auto start
			qCWarning(loggingCategory) << "Start commands ignored because master is already running!"
									   << "The terminal will connect with an empty argument list!";
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, QStringList()),
									  Q_ARG(bool, false));
			return EXIT_SUCCESS;
		} else {
			if(!isAutoStart)
				qCWarning(loggingCategory) << "Master is already running. Start arguments will be passed to it as is";
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, arguments),//send original arguments
									  Q_ARG(bool, false));
			return EXIT_SUCCESS;
		}
	}
}

int AppPrivate::testMasterRunning()
{
	auto arguments = QCoreApplication::arguments();
	arguments.removeFirst();//remove app name
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

int AppPrivate::purgeMaster(const QCommandLineParser &parser)
{
	if(!parser.isSet("accept")) {
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

	qint64 pid;
	QString hostname;
	QString appname;
	if(this->masterLock->getLockInfo(&pid, &hostname, &appname)) {
		if(this->masterLock->removeStaleLockFile())
			std::cout << "Master lockfile successfully removed. It was locked by:";
		else {
			std::cout << "Failed to remove master lockfile. Lock data is:";
			res |= 0x02;
		}
		std::cout << "\n - PID: "
				  << pid
				  << "\n - Hostname: "
				  << hostname.toStdString()
				  << "\n - Appname: "
				  << appname.toStdString()
				  << std::endl;
	} else
		std::cout << "No lock file detected" << std::endl;

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
		//create terminal parser and validate it
		terminal->parser.reset(new QCommandLineParser());
		qApp->setupParser(*terminal->parser.data());
		if(!terminal->loadParser()) {
			qCWarning(loggingCategory) << "Terminal with invalid commands discarded. Error:"
									   << terminal->parser->errorText();
			terminal->deleteLater();
			return;
		}

		//handle own arguments (logging)
		if(terminal->parser->isSet("loglevel"))
			this->updateLoggingMode(terminal->parser->value("loglevel").toInt());
		if(terminal->parser->isSet("logpath"))
			this->updateLoggingPath(terminal->parser->value("logpath"));

		//add terminal to terminal list
		auto rTerm = new Terminal(terminal, this);
		rTerm->setAutoDelete(this->autoDelete);
		connect(rTerm, &Terminal::destroyed, this, [=](){
			this->activeTerminals.removeOne(rTerm);
			emit this->q_ptr->connectedTerminalsChanged(this->activeTerminals, App::QPrivateSignal());
		});
		this->activeTerminals.append(rTerm);
		emit this->q_ptr->connectedTerminalsChanged(this->activeTerminals, App::QPrivateSignal());
		//emit the command
		emit this->q_ptr->commandReceived(rTerm->parser(), rTerm->isStarter(), App::QPrivateSignal());

		//test if stop command
		auto args = rTerm->parser()->positionalArguments();
		if(!args.isEmpty() && args.first() == QStringLiteral("stop"))
			this->stopMaster(rTerm);

		if(this->autoKill || rTerm->parser()->isSet("detached")) {
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
	if(this->q_ptr->requestAppShutdown(term, eCode)) {
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
