#include "app.h"
#include "app_p.h"

#include <QCtrlSignals>

using namespace QtBackgroundProcess;

App::App(int &argc, char **argv, int flags) :
	QCoreApplication(argc, argv, flags),
	d(new AppPrivate(this))
{
	AppPrivate::p_valid = true;

	qSetMessagePattern(AppPrivate::terminalMessageFormat);
	qInstallMessageHandler(AppPrivate::qbackProcMessageHandler);

	QCtrlSignalHandler::instance()->setAutoQuitActive(true);
}

App::~App()
{
	if(d->logFile)
		d->logFile->close();
	AppPrivate::p_valid = false;
}

QString App::instanceID() const
{
	return d->instanceId;
}

bool App::globalInstance() const
{
	return d->globalInstance;
}

bool App::forwardMasterLog() const
{
	return d->masterLogging;
}

bool App::autoStartMaster() const
{
	return d->autoStart;
}

bool App::ignoreMultiStarts() const
{
	return d->ignoreExtraStart;
}

bool App::autoDeleteTerminals() const
{
	return d->autoDelete;
}

bool App::autoKillTerminals() const
{
	return d->autoKill;
}

void App::setParserSetupFunction(const std::function<void (QCommandLineParser &)> &function)
{
	d->parserFunc = function;
}

void App::setStartupFunction(const std::function<int (const QCommandLineParser &)> &function)
{
	d->startupFunc = function;
}

void App::setShutdownRequestFunction(const std::function<bool(const QCommandLineParser &, int&)> &function)
{
	d->shutdownFunc = [function](Terminal *t, int &r){
		return function(*t->parser().data(), r);
	};
}

void App::setShutdownRequestFunction(const std::function<bool(Terminal*, int&)> &function)
{
	d->shutdownFunc = function;
}

int App::exec()
{
	d->running = true;

	//process arguments
	QCommandLineParser parser;
	setupParser(parser);
	parser.process(*this);

	//update terminal logging
	d->updateLoggingMode(parser.value(QStringLiteral("terminallog")).toInt());

	//generate the single id (temporary disable running)
	d->running = false;
	createDefaultInstanceID(false);
	d->running = true;

	auto res = d->initControlFlow(parser);
	if(res == -1)//special case
		return EXIT_SUCCESS;
	else if(res != EXIT_SUCCESS)
		return res;
	res = QCoreApplication::exec();
	d->running = false;
	return res;
}

QList<Terminal *> App::connectedTerminals() const
{
	return d->activeTerminals;
}

void App::createDefaultInstanceID(bool overwrite)
{
	if(overwrite || d->instanceId.isNull())
		d->setInstanceId(d->generateSingleId());
}

void App::setInstanceID(QString instanceID, bool useAsSeed)
{
	if(useAsSeed)
		d->setInstanceId(d->generateSingleId(instanceID));
	else
		d->setInstanceId(instanceID);
}

void App::setGlobalInstance(bool globalInstance)
{
	d->globalInstance = globalInstance;
}

void App::setForwardMasterLog(bool forwardMasterLog)
{
	if(forwardMasterLog == d->masterLogging)
		return;

	d->masterLogging = forwardMasterLog;
	if(d->masterLock && d->masterLock->isLocked()) {//I am master
		if(forwardMasterLog)
			d->debugTerm = new GlobalTerminal(d, true);
		else {
			d->debugTerm->deleteLater();
			d->debugTerm.clear();
		}
	}
}

void App::setAutoStartMaster(bool autoStartMaster)
{
	d->autoStart = autoStartMaster;
}

void App::setIgnoreMultiStarts(bool ignoreMultiStarts)
{
	d->ignoreExtraStart = ignoreMultiStarts;
}

void App::setAutoDeleteTerminals(bool autoDeleteTerminals, bool changeCurrent)
{
	d->autoDelete = autoDeleteTerminals;
	if(changeCurrent) {
		foreach(auto terminal, d->activeTerminals)
			terminal->setAutoDelete(autoDeleteTerminals);
	}
}

void App::setAutoKillTerminals(bool autoKillTerminals, bool killCurrent)
{
	d->autoKill = autoKillTerminals;
	if(killCurrent) {
		foreach(auto terminal, d->activeTerminals) {
			terminal->setAutoDelete(true);
			terminal->disconnectTerminal();
		}
	}
}

void App::setupParser(QCommandLineParser &parser, bool useShortOptions)
{
	d->setupDefaultParser(parser, useShortOptions);
	if(d->parserFunc)
		d->parserFunc(parser);
}

int App::startupApp(const QCommandLineParser &parser)
{
	if(d->startupFunc)
		return d->startupFunc(parser);
	else
		return EXIT_SUCCESS;
}

bool App::requestAppShutdown(Terminal *terminal, int &exitCode)
{
	if(d->shutdownFunc)
		return d->shutdownFunc(terminal, exitCode);
	else
		return true;
}



NotAllowedInRunningStateException::NotAllowedInRunningStateException() :
	QException()
{}

const char *NotAllowedInRunningStateException::what() const noexcept
{
	return "You are not allowed to perform this operation while the application is running!";
}

void NotAllowedInRunningStateException::raise() const
{
	throw *this;
}

QException *NotAllowedInRunningStateException::clone() const
{
	return new NotAllowedInRunningStateException();
}
