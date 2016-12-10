#ifndef QBACKGROUNDPROCESS_APPPRIVATE_H
#define QBACKGROUNDPROCESS_APPPRIVATE_H

#include "app.h"
#include <QLockFile>
#include <QScopedPointer>
#include <QLocalServer>
#include <QPointer>
#include <QLoggingCategory>
#include <QFile>
#include "masterconnecter.h"
#include "terminal_p.h"
#include "globalterminal.h"

namespace QBackgroundProcess {

class AppPrivate : public QObject
{
	Q_OBJECT

public:
	static const QString masterArgument;
	static const QString purgeArgument;
	static const QString startArgument;

	static const QString terminalMessageFormat;
	static const QString masterMessageFormat;

	static QString generateSingleId(const QString &seed = QString());
	static AppPrivate *p_ptr();

	static void qbackProcMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	bool running;
	bool masterLogging;
	bool autoStart;
	bool ignoreExtraStart;
	bool autoDelete;
	bool autoKill;

	QString instanceId;

	QScopedPointer<QLockFile> masterLock;
	QLocalServer *masterServer;
	QList<Terminal*> activeTerminals;
	std::function<void(QCommandLineParser&)> parserFunc;
	std::function<int(const QCommandLineParser &)> startupFunc;
	std::function<bool(Terminal*, int&)> shutdownFunc;

	MasterConnecter *master;

	QPointer<GlobalTerminal> debugTerm;
	QPointer<QFile> logFile;

	AppPrivate(App *q_ptr);
	~AppPrivate();

	void setInstanceId(const QString &id);

	void setupDefaultParser(QCommandLineParser &parser, bool useShortOptions = true);
	void updateLoggingMode(int level);
	void updateLoggingPath(const QString &path);

public slots:
	int initControlFlow(const QCommandLineParser &parser);

private slots:

	int makeMaster(const QCommandLineParser &parser);
	int startMaster(bool isAutoStart = false);
	int testMasterRunning();
	int purgeMaster(const QCommandLineParser &parser);

	void newTerminalConnected();
	void terminalLoaded(TerminalPrivate *terminal, bool success);
	void stopMaster(Terminal *term);
	void doExit(int code);

	void beginMasterConnect(const QStringList &arguments, bool isStarter);

private:
	App *q_ptr;
};

Q_DECLARE_LOGGING_CATEGORY(loggingCategory)

}

#endif // QBACKGROUNDPROCESS_APPPRIVATE_H
