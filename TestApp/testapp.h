#ifndef TESTAPP_H
#define TESTAPP_H

#include <QBackgroundProcess>

class TestApp : public QBackgroundProcess::App
{
	Q_OBJECT

public:
	explicit TestApp(int &argc, char **argv);

	// App interface
protected:
	int startupApp(const QStringList &arguments) override;
	bool requestAppShutdown(QBackgroundProcess::Terminal *terminal, int &exitCode) override;
	void setupParser(QCommandLineParser &parser, bool useShortOptions) override;

private slots:
	void handleCommand(const QStringList &arguments, bool starter);
	void addTerminal(QBackgroundProcess::Terminal *terminal);

private:
	QBackgroundProcess::GlobalTerminal *statusTerm;

	void doCommand(const QStringList &args);
};

#endif // TESTAPP_H
