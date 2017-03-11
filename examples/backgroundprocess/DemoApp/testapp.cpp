#include "testapp.h"

#include <QDebug>
using namespace QtBackgroundProcess;

TestApp::TestApp(int &argc, char **argv) :
	App(argc, argv),
	statusTerm(nullptr)
{}

void TestApp::parseTerminalOptions()
{
	QCommandLineParser parser;
	this->setupParser(parser, true);
	parser.process(*this);
	this->setAutoStartMaster(parser.isSet("a"));
	this->setIgnoreMultiStarts(parser.isSet("i"));
}

int TestApp::startupApp(const QCommandLineParser &parser)
{
	doCommand(parser);
	qDebug() << "App Master Started with arguments:"
			 << parser.positionalArguments()
			 << "and options:"
			 << parser.optionNames();

	connect(this, &TestApp::commandReceived,
			this, &TestApp::handleCommand);

	if(parser.isSet("m")) {
		if(parser.value("m") == "echo") {
			connect(this, &TestApp::newTerminalConnected,
					this, &TestApp::addTerminal);
			qDebug() << "Master started in echo mode!";
		} else if(parser.value("m") == "status"){
			statusTerm = new GlobalTerminal(this);
			qDebug() << "Master started in status mode!";
		} else
			qWarning() << "Unknown mode! Will be ignored";
	}

	return EXIT_SUCCESS;
}

bool TestApp::requestAppShutdown(Terminal *terminal, int &)
{
	qDebug() << "stop requested with"
			 << terminal->parser()->positionalArguments()
			 << "and options:"
			 << terminal->parser()->optionNames();
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

void TestApp::handleCommand(QSharedPointer<QCommandLineParser> parser, bool starter)
{
	if(starter) {
		qDebug() << "skipping starter args:"
				 << parser->positionalArguments()
				 << "and options:"
				 << parser->optionNames();
	} else {
		doCommand(*parser.data());
		qDebug() << "received new command:"
				 << parser->positionalArguments()
				 << "and options:"
				 << parser->optionNames();
	}

	if(statusTerm)
		statusTerm->write("[" + parser->positionalArguments().join(", ").toUtf8() + "]\n");
}

void TestApp::addTerminal(Terminal *terminal)
{
	//add a simple echo to all terminals
	connect(terminal, &Terminal::readyRead, terminal, [terminal](){
		terminal->write(terminal->readAll());
	});
}

void TestApp::doCommand(const QCommandLineParser &parser)
{
	if(parser.isSet("f"))
		this->setForwardMasterLog(parser.value("f").toInt());
}

int main(int argc, char *argv[])
{
	TestApp a(argc, argv);
	TestApp::setApplicationVersion("4.2.0");

	a.parseTerminalOptions();
	return a.exec();
}
