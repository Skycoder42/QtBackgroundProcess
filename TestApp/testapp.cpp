#include "testapp.h"
using namespace QBackgroundProcess;

TestApp::TestApp(int &argc, char **argv) :
	App(argc, argv)
{}

int TestApp::startupApp(const QStringList &arguments)
{
	auto parser = this->parseArguments(arguments);
	this->setForwardMasterLog(parser->isSet("f"));
	this->setAutoStartMaster(parser->isSet("a"));
	this->setIgnoreMultiStarts(parser->isSet("i"));
	return EXIT_SUCCESS;
}

bool TestApp::requestAppShutdown(Terminal *terminal, int &exitCode)
{
	return true;
}

void TestApp::setupParser(QCommandLineParser &parser, bool useShortOptions)
{
	this->App::setupParser(parser, useShortOptions);

	parser.addOption({
						 {"a", "autostart"},
						 "Starts the master automatically, if not already running"
					 });
	parser.addOption({
						 {"f", "forward"},
						 "forwards master debug output to all terminals"
					 });
	parser.addOption({
						 {"i", "ignoreStart"},
						 "If start is called a second time, the arguments will be omitted"
					 });
	parser.addOption({
						 {"T", "terminalmode"},
						 "Runs the master in terminal mode instead of argument mode"
					 });
}

int main(int argc, char *argv[])
{
	TestApp a(argc, argv);
	return a.exec();
}
