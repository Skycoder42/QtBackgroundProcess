#include "testapp.h"

#include <QDebug>
using namespace QBackgroundProcess;

TestApp::TestApp(int &argc, char **argv) :
	App(argc, argv),
	doEcho(true)
{}

int TestApp::startupApp(const QStringList &arguments)
{
	doCommand(arguments);
	qDebug() << "App Master Started with arguments:" << arguments;

	connect(this, &TestApp::commandReceived,
			this, &TestApp::handleCommand);

	return EXIT_SUCCESS;
}

bool TestApp::requestAppShutdown(Terminal *terminal, int &exitCode)
{
	qDebug() << "stop requested with" << terminal->arguments();
	return true;
}

void TestApp::setupParser(QCommandLineParser &parser, bool useShortOptions)
{
	this->App::setupParser(parser, useShortOptions);

	parser.addOption({
						 {"a", "autostart"},
						 "Starts the master automatically, if not already running."
					 });
	parser.addOption({
						 {"i", "ignoreStart"},
						 "If start is called a second time, the arguments will be omitted."
					 });
	parser.addOption({
						 {"f", "forward"},
						 "forwards master debug output to all terminals if <active> = 1, disables it if <active> = 0.",
						 "active",
						 "1"
					 });
	parser.addOption({
						 {"m", "mode"},
						 "Tells the master to run <mode>. Can be \"echo\" to simply echo all terminals, "
						 "or \"status\" to simply broadcast new arguments to all terminals. Until explicitly set, "
						 "nothing will be done",
						 "mode",
						 "echo"
					 });
}

void TestApp::handleCommand(const QStringList &arguments, bool starter)
{
	if(starter)
		qDebug() << "skipping starter args:" << arguments;
	else {
		doCommand(arguments);
		qDebug() << "received new command:" << arguments;
	}
}

void TestApp::doCommand(const QStringList &args)
{
	auto parser = this->parseArguments(args);
	if(parser->isSet("f"))
		this->setForwardMasterLog(parser->value("f").toInt());
	if(parser->isSet("m")) {
		if(parser->value("m") == "echo")
			doEcho = true;
		else if(parser->value("m") == "status")
			doEcho = false;
		else
			qWarning() << "Unknown mode! Will be ignored";
	}
}

int main(int argc, char *argv[])
{
	TestApp a(argc, argv);
	TestApp::setApplicationVersion("4.2.0");

	//handle those here, because the apply to the terminals, not the master
	auto parser = a.parseArguments(a.arguments());
	a.setAutoStartMaster(parser->isSet("a"));
	a.setIgnoreMultiStarts(parser->isSet("i"));

	return a.exec();
}
