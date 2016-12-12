#ifndef TESTAPP_H
#define TESTAPP_H

#include <QtBackgroundProcess>

class TestApp : public QtBackgroundProcess::App
{
	Q_OBJECT

public:
	explicit TestApp(int &argc, char **argv);

	void parseTerminalOptions();

	// App interface
protected:
	int startupApp(const QCommandLineParser &parser) override;
	bool requestAppShutdown(QtBackgroundProcess::Terminal *terminal, int &) override;
	void setupParser(QCommandLineParser &parser, bool useShortOptions) override;

private slots:
	void handleCommand(QSharedPointer<QCommandLineParser> parser, bool starter);
	void addTerminal(QtBackgroundProcess::Terminal *terminal);

private:
	QtBackgroundProcess::GlobalTerminal *statusTerm;

	void doCommand(const QCommandLineParser &parser);
};

#endif // TESTAPP_H
