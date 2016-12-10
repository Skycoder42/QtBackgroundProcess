#ifndef TESTAPP_H
#define TESTAPP_H

#include <QBackgroundProcess>

class TestApp : public QBackgroundProcess::App
{
	Q_OBJECT

public:
	explicit TestApp(int &argc, char **argv);

	void parseTerminalOptions();

	// App interface
protected:
	int startupApp(const QCommandLineParser &parser) override;
	bool requestAppShutdown(QBackgroundProcess::Terminal *terminal, int &) override;
	void setupParser(QCommandLineParser &parser, bool useShortOptions) override;

private slots:
	void handleCommand(QSharedPointer<QCommandLineParser> parser, bool starter);
	void addTerminal(QBackgroundProcess::Terminal *terminal);

private:
	QBackgroundProcess::GlobalTerminal *statusTerm;

	void doCommand(const QCommandLineParser &parser);
};

#endif // TESTAPP_H
