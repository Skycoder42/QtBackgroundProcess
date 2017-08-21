#include "app_p.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>

#include <QCtrlSignals>
#include <iostream>
#ifdef Q_OS_WIN
#include <qt_windows.h>
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
const QString AppPrivate::restartArgument(QStringLiteral("restart"));
#ifdef Q_OS_WIN
const QString AppPrivate::terminalMessageFormat(QStringLiteral("%{if-debug}[Debug]    %{endif}"
															   "%{if-info}[Info]     %{endif}"
															   "%{if-warning}[Warning]  %{endif}"
															   "%{if-critical}[Critical] %{endif}"
															   "%{if-fatal}[Fatal]    %{endif}"
															   "%{if-category}%{category}: %{endif}"
															   "%{message}\n"));
#else
const QString AppPrivate::terminalMessageFormat(QStringLiteral("%{if-debug}[\033[32mDebug\033[0m]    %{endif}"
															   "%{if-info}[\033[36mInfo\033[0m]     %{endif}"
															   "%{if-warning}[\033[33mWarning\033[0m]  %{endif}"
															   "%{if-critical}[\033[31mCritical\033[0m] %{endif}"
															   "%{if-fatal}[\033[35mFatal\033[0m]    %{endif}"
															   "%{if-category}%{category}: %{endif}"
															   "%{message}\n"));
#endif
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
	fullId.prepend(QStringLiteral("qtbackproc-"));
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
	return qApp->d;
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
		std::abort();
}

AppPrivate::AppPrivate(App *q_ptr) :
	QObject(q_ptr),
	running(false),
	masterLogging(false),
	autoStart(false),
	ignoreExtraStart(false),
	autoDelete(true),
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
	q(q_ptr)
{}

void AppPrivate::setInstanceId(const QString &id)
{
	if(running)
		throw NotAllowedInRunningStateException();

	instanceId = id;
	auto lockPath = QDir::temp().absoluteFilePath(id + QStringLiteral(".lock"));
	masterLock.reset(new QLockFile(lockPath));
	masterLock->setStaleLockTime(0);
}

void AppPrivate::setupDefaultParser(QCommandLineParser &parser, bool useShortOptions)
{
	parser.addHelpOption();
	if(!QCoreApplication::applicationVersion().isEmpty())
		parser.addVersionOption();

	QStringList DParams(QStringLiteral("detached"));
	QStringList lParams({QStringLiteral("log"), QStringLiteral("loglevel")});
	QStringList LParams(QStringLiteral("logpath"));
	if(useShortOptions) {
		DParams.prepend(QStringLiteral("D"));
		lParams.prepend(QStringLiteral("l"));
		LParams.prepend(QStringLiteral("L"));
	}

	parser.addPositionalArgument(QStringLiteral("<command>"),
								 tr("A control command to control the background application. "
									"Possible options are:\n"
									" - start: starts the application\n"
									" - stop: stops the application\n"
									" - restart: stops the application and then starts it again with the given arguments\n"
									" - purge_master: purges local servers and lockfiles, in case the master process crashed. "
									"Pass \"--accept\" as second parameter, if you want to skip the prompt."),
								 QStringLiteral("[start|stop|purge_master]"));

	parser.addOption({
						 DParams,
						 tr("It set, the terminal will only pass it's arguments to the master, and automatically finish after.")
					 });
	parser.addOption({
						 lParams,
						 tr("Set the desired log <level>. Possible values are:\n"
							" - 0: log nothing\n"
							" - 1: critical errors only\n"
							" - 2: like 1 plus warnings\n") +
					 #ifdef QT_NO_DEBUG
						 tr(" - 3: like 2 plus information messages (default)\n"
							" - 4: verbose - log everything"),
						 tr("level"),
						 QStringLiteral("3")
					 #else
						 tr(" - 3: like 2 plus information messages\n"
							" - 4: verbose - log everything (default)"),
						 tr("level"),
						 QStringLiteral("4")
					 #endif
					 });

	QString defaultPath;
#ifdef Q_OS_UNIX
	if(QFileInfo(QStringLiteral("/var/log")).isWritable())
		defaultPath = QStringLiteral("/var/log/%1.log").arg(QCoreApplication::applicationName());
	else
#endif
	{
		auto basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
		QDir(basePath).mkpath(QStringLiteral("."));
		defaultPath = QStringLiteral("%1/%2.log")
			.arg(basePath)
			.arg(QCoreApplication::applicationName());
	}

	parser.addOption({
						 LParams,
						 tr("Overwrites the default log <path>. The default path is platform and application specific. "
							"For this instance, it defaults to \"%1\". NOTE: The application can override the value internally. "
							"Pass an empty string (--logpath \"\") to disable logging to a file.")
						 .arg(defaultPath),
						 tr("path"),
						 defaultPath
					 });
	parser.addOption({
						 QStringLiteral("terminallog"),
						 tr("Sets the log <level> for terminal only messages. This does not include messages forwarded from the master. "
							"Log levels are the same as for the <loglevel> option."),
						 tr("level"),
					 #ifdef QT_NO_DEBUG
						 QStringLiteral("3")
					 #else
						 QStringLiteral("4")
					 #endif
					 });
	parser.addOption({
						 {QStringLiteral("no-daemon"), QStringLiteral("keep-console")},
						 tr("Will prevent the master process from \"closing\" the console and other stuff that is done to daemonize the process. "
							"Can be useful for debugging purpose.")
					 });

	parser.addOption({
						 QStringLiteral("accept"),
						 tr("Skips the prompt and accepts where possible.")
					 });
}

void AppPrivate::updateLoggingMode(int level)
{
	QString logStr;
	QT_WARNING_PUSH
	QT_WARNING_DISABLE_GCC("-Wimplicit-fallthrough=")
	switch (level) {
	case 0:
		logStr.prepend(QStringLiteral("\n*.critical=false"));
	case 1:
		logStr.prepend(QStringLiteral("\n*.warning=false"));
	case 2:
		logStr.prepend(QStringLiteral("\n*.info=false"));
	case 3:
		logStr.prepend(QStringLiteral("*.debug=false"));
	case 4:
		break;
	default:
		return;
	}
	QT_WARNING_POP
	QLoggingCategory::setFilterRules(logStr);
}

void AppPrivate::updateLoggingPath(const QString &path)
{
	if(logFile) {
		logFile->close();
		logFile->deleteLater();
		logFile.clear();
	}
	if(!path.isEmpty()) {
		logFile = new QFile(path, this);
		if(!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
			auto error = logFile->errorString();
			logFile->deleteLater();
			logFile.clear();
			qWarning() << "Failed to open logfile with error:" << error;
		}
	}
}

int AppPrivate::initControlFlow(const QCommandLineParser &parser)
{
	//start/make master
	auto args = parser.positionalArguments();
	if(args.size() > 0) {
		if(args[0] == masterArgument)
			return makeMaster(parser);
		else if(args[0] == purgeArgument)
			return purgeMaster(parser);
		else if(args[0] == startArgument)
			return startMaster();
		else if(args[0] == restartArgument)
			return restartMaster(parser);
	}

	//neither start nor make master --> "normal" client or autostart
	if(autoStart)
		return startMaster(true);
	else
		return commandMaster();
}

int AppPrivate::makeMaster(const QCommandLineParser &parser)
{
	//create local server --> do first to make shure clients only see a "valid" master lock if the master started successfully
	masterServer = new QLocalServer(this);
	connect(masterServer, &QLocalServer::newConnection,
			this, &AppPrivate::newTerminalConnected,
			Qt::QueuedConnection);
	if(!masterServer->listen(instanceId)) {
		qCritical() << tr("Failed to create local server with error:")
					<< masterServer->errorString();
		return EXIT_FAILURE;
	}

	//get the lock
	if(!masterLock->tryLock(5000)) {//wait at most 5 sec
		masterServer->close();
		qCritical() << tr("Unable to start master process. Failed with lock error:")
					<< masterLock->error();
		return EXIT_FAILURE;
	} else {
		//setup master logging stuff
		qSetMessagePattern(AppPrivate::masterMessageFormat);
		if(masterLogging)
			debugTerm = new GlobalTerminal(this, true);
		updateLoggingMode(parser.value(QStringLiteral("loglevel")).toInt());
		updateLoggingPath(parser.value(QStringLiteral("logpath")));

		//detache from any console, if wished
		if(!parser.isSet(QStringLiteral("no-daemon"))) {
#ifdef Q_OS_WIN //detach the console window
			if(!FreeConsole()) {
				auto console = GetConsoleWindow();
				if(console)
					ShowWindow(GetConsoleWindow(), SW_HIDE);
			}

			//TODO temporary disable for testing
			//auto sigHandler = QCtrlSignalHandler::instance();
			//sigHandler->setAutoQuitActive(true);

			//set current directory
			QDir::setCurrent(QDir::rootPath());
#else
			daemon(false, false);

			auto sigHandler = QCtrlSignalHandler::instance();
			sigHandler->setAutoQuitActive(true);
			sigHandler->registerForSignal(SIGINT);
			sigHandler->registerForSignal(SIGHUP);
			sigHandler->registerForSignal(SIGWINCH);
#endif
		} else
			QDir::setCurrent(QDir::rootPath());

		auto res = q->startupApp(parser);
		if(res != EXIT_SUCCESS) {
			//cleanup
			masterServer->close();
			masterLock->unlock();
		}

		return res;
	}
}

int AppPrivate::startMaster(bool isAutoStart, bool isRestart)
{
	auto arguments = QCoreApplication::arguments();
	arguments.removeFirst();//remove app name
	//check if master already lives
	if(masterLock->tryLock()) {//no master alive
		auto ok = false;

		auto args = arguments;
		if(isRestart)
			args.removeOne(restartArgument);
		else if(!isAutoStart)
			args.removeOne(startArgument);
		args.prepend(masterArgument);
		if(QProcess::startDetached(QCoreApplication::applicationFilePath(), args)) {//start MASTER with additional start params
			//wait for the master to start
			masterLock->unlock();
			for(auto i = 0; i < 50; i++) {//wait at most 5 sec
				qint64 pid;
				QString hostname;
				QString appname;
				if(masterLock->getLockInfo(&pid, &hostname, &appname)) {
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
			qCritical() << tr("Failed to start master process! No master lock was detected.");
			return EXIT_FAILURE;
		}
	} else {//master is running --> ok
		if(!isAutoStart && ignoreExtraStart) {// ignore only on normal starts, not on auto start
			qWarning() << tr("Start commands ignored because master is already running! "
											 "The terminal will connect with an empty argument list!");
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, QStringList()),
									  Q_ARG(bool, false));
			return EXIT_SUCCESS;
		} else {
			if(!isAutoStart)
				qWarning() << tr("Master is already running. Start arguments will be passed to it as is");
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, arguments),//send original arguments
									  Q_ARG(bool, false));
			return EXIT_SUCCESS;
		}
	}
}

int AppPrivate::restartMaster(const QCommandLineParser &parser)
{
	//step 1 -> stop master, by running another terminal
	auto res = QProcess::execute(QCoreApplication::applicationFilePath(), {QStringLiteral("stop")});
	if(res != EXIT_SUCCESS && !parser.isSet(QStringLiteral("accept"))) {
		std::cout << tr("\nFailed to stop the running master process.\n"
						"Do you want to restart it anyway? (y/N)").toStdString();
		std::cout.flush();
		char res = (char)std::cin.get();
		if(res != tr("y") && res != tr("Y"))
			return EXIT_FAILURE;
	} else
		qDebug() << tr("Master process successfully stopped");

	//step 2 -> start master
	return startMaster(false, true);
}

int AppPrivate::commandMaster()
{
	auto arguments = QCoreApplication::arguments();
	arguments.removeFirst();//remove app name
	if(masterLock->tryLock()) {
		masterLock->unlock();
		qCritical() << tr("Master process is not running! Please launch it by using:")
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
	if(!parser.isSet(QStringLiteral("accept"))) {
		std::cout << tr("Are you shure you want to purge the master lock and server?\n"
						"Only do this if the master process is not running anymore, but the lock/server "
						"are not available (for example after a crash)\n"
						"Purging while the master process is still running will crash it.\n"
						"Press (Y) to purge, or (n) to cancel:").toStdString();
		std::cout.flush();
		char res = (char)std::cin.get();
		if(res == tr("n") || res == tr("N"))
			return EXIT_FAILURE;
	}

	auto res = 0;

	qint64 pid;
	QString hostname;
	QString appname;
	if(masterLock->getLockInfo(&pid, &hostname, &appname)) {
		if(masterLock->removeStaleLockFile())
			std::cout << tr("Master lockfile successfully removed. It was locked by:").toStdString();
		else {
			std::cout << tr("Failed to remove master lockfile. Lock data is:").toStdString();
			res |= 0x02;
		}
		std::cout << tr("\n - PID: ").toStdString()
				  << pid
				  << tr("\n - Hostname: ").toStdString()
				  << hostname.toStdString()
				  << tr("\n - Appname: ").toStdString()
				  << appname.toStdString()
				  << std::endl;
	} else
		std::cout << tr("No lock file detected").toStdString() << std::endl;

	if(QLocalServer::removeServer(instanceId))
		std::cout << tr("Master server successfully removed").toStdString() << std::endl;
	else {
		std::cout << tr("Failed to remove master server").toStdString() << std::endl;
		res |= 0x04;
	}

	return res == 0 ? -1 : res;
}

void AppPrivate::newTerminalConnected()
{
	while(masterServer->hasPendingConnections()) {
		auto termp = new TerminalPrivate(masterServer->nextPendingConnection(), this);
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
			qWarning() << tr("Terminal with invalid commands discarded. Error:")
					   << terminal->parser->errorText();
			terminal->deleteLater();
			return;
		}

		//handle own arguments (logging)
		if(terminal->parser->isSet(QStringLiteral("loglevel")))
			updateLoggingMode(terminal->parser->value(QStringLiteral("loglevel")).toInt());
		if(terminal->parser->isSet(QStringLiteral("logpath")))
			updateLoggingPath(terminal->parser->value(QStringLiteral("logpath")));

		//create real terminal
		auto rTerm = new Terminal(terminal, this);
		rTerm->setAutoDelete(autoDelete);
		//emit the command
		emit q->commandReceived(rTerm->parser(), rTerm->isStarter(), App::QPrivateSignal());

		//test if stop command
		auto args = rTerm->parser()->positionalArguments();
		if(!args.isEmpty() && args.first() == QStringLiteral("stop"))
			stopMaster(rTerm);

		if(autoKill || rTerm->parser()->isSet(QStringLiteral("detached"))) {
			rTerm->setAutoDelete(true);
			rTerm->disconnectTerminal();
		} else {
			//add terminal to terminal list
			connect(rTerm, &Terminal::destroyed, this, [=](){
				activeTerminals.removeOne(rTerm);
				emit q->connectedTerminalsChanged(activeTerminals, App::QPrivateSignal());
			});
			activeTerminals.append(rTerm);
			emit q->connectedTerminalsChanged(activeTerminals, App::QPrivateSignal());
			//new terminal signal
			emit q->newTerminalConnected(rTerm, App::QPrivateSignal());
		}
	} else
		terminal->deleteLater();
}

void AppPrivate::stopMaster(Terminal *term)
{
	int eCode = EXIT_SUCCESS;
	if(q->requestAppShutdown(term, eCode)) {
		foreach(auto termin, activeTerminals)
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
	master = new MasterConnecter(instanceId, arguments, isStarter, this);
}
