#include "app.h"
#include "app_p.h"
#include <iostream>
#ifdef Q_OS_WIN
#include <QDir>
#include <QStandardPaths>
#endif
using namespace QBackgroundProcess;

#ifdef d
#undef d
#endif
#define d this->d_ptr

QString App::defaultLogPath()
{
#ifdef Q_OS_WIN
	auto basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
	QDir(basePath).mkpath(".");
	return QStringLiteral("%1/%2.log")
			.arg(basePath)
			.arg(QCoreApplication::applicationName());
#else
	//TODO check if root, else use user directory!
	return QStringLiteral("/var/log/%1.log").arg(QCoreApplication::applicationName());
#endif
}

QtMessageHandler App::activateTerminalDebugRedirect(bool format)
{
	if(format)
		qSetMessagePattern(AppPrivate::terminalMessageFormat);

	return qInstallMessageHandler(AppPrivate::termDebugMessage);
}

QtMessageHandler App::activateMasterDebugRedirect(bool format)
{
	auto self = AppPrivate::p_ptr();
	if(self->debugTerm)
		self->debugTerm->deleteLater();
	self->debugTerm = new GlobalTerminal(qApp, self, true);

	if(format)
		qSetMessagePattern(AppPrivate::masterMessageFormat);

	return qInstallMessageHandler(AppPrivate::masterDebugMessage);
}

QtMessageHandler App::activateMasterLogging(QString logFile, bool format)
{
	auto self = AppPrivate::p_ptr();
	if(self->logFile) {
		self->logFile->close();
		self->logFile->deleteLater();
	}
	if(!logFile.isNull()) {
		self->logFile = new QFile(logFile, self);
		if(!self->logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
			self->logFile->deleteLater();
	}

	if(format)
		qSetMessagePattern(AppPrivate::masterMessageFormat);

	return qInstallMessageHandler(AppPrivate::masterDebugMessage);
}

App::App(int &argc, char **argv) :
	QCoreApplication(argc, argv),
	d_ptr(new AppPrivate(this))
{}

App::~App(){}

QString App::instanceID() const
{
	return d->instanceId;
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

QSharedPointer<QCommandLineParser> App::parseArguments(const QStringList &arguments)
{
	auto ptr = QSharedPointer<QCommandLineParser>::create();
	this->setupParser(*ptr.data());
	if(ptr->parse(arguments))
		return ptr;
	else
		throw InvalidArgumentsException(ptr->errorText());
}

void App::setParserSetupFunction(const std::function<void (QCommandLineParser &)> &function)
{
	d->parserFunc = function;
}

void App::setStartupFunction(const std::function<int (QStringList)> &function)
{
	d->startupFunc = function;
}

void App::setShutdownFunction(const std::function<bool(QStringList)> &function)
{
	d->shutdownFunc = [function](Terminal *t, int&){
		return function(t->arguments());
	};
}

void App::setShutdownFunction(const std::function<bool(QStringList, int&)> &function)
{
	d->shutdownFunc = [function](Terminal *t, int &r){
		return function(t->arguments(), r);
	};
}

void App::setShutdownFunction(const std::function<bool(Terminal*)> &function)
{
	d->shutdownFunc = [function](Terminal *t, int&){
		return function(t);
	};
}

void App::setShutdownFunction(const std::function<bool(Terminal*, int&)> &function)
{
	d->shutdownFunc = function;
}

int App::exec()
{
	d->running = true;

	//process arguments
	QCommandLineParser parser;
	this->setupParser(parser);
	parser.process(*this);

	//generate the single id
	this->createDefaultInstanceID(false);

	auto res = d->initControlFlow();
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
		d->setInstanceId(AppPrivate::generateSingleId());
}

void App::setInstanceID(QString instanceID, bool useAsSeed)
{
	if(d->running)
		throw NotAllowedInRunningStateException(QStringLiteral("Change the instance id"));

	if(useAsSeed)
		d->setInstanceId(AppPrivate::generateSingleId(instanceID));
	else
		d->setInstanceId(instanceID);
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

int App::startupApp(const QStringList &arguments)
{
	if(d->startupFunc)
		return d->startupFunc(arguments);
	else
		return EXIT_SUCCESS;
}

bool App::shutdownApp(Terminal *terminal, int &exitCode)
{
	if(d->shutdownFunc)
		return d->shutdownFunc(terminal, exitCode);
	else
		return true;
}



NotAllowedInRunningStateException::NotAllowedInRunningStateException(const QString &reason) :
	QtException(QStringLiteral("You are not allowed to perform the following action while the application is running: ") + reason)
{}

InvalidArgumentsException::InvalidArgumentsException(const QString &errorText) :
	QtException(errorText)
{}
