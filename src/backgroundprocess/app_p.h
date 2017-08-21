#ifndef QTBACKGROUNDPROCESS_APP_P_H
#define QTBACKGROUNDPROCESS_APP_P_H

#include "app.h"

#include "masterconnecter_p.h"
#include "terminal_p.h"
#include "globalterminal.h"

#include <QtCore/QLockFile>
#include <QtCore/QScopedPointer>
#include <QtCore/QPointer>
#include <QtCore/QLoggingCategory>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QMessageLogger>

#include <QtNetwork/QLocalServer>

namespace QtBackgroundProcess {

class Q_BACKGROUNDPROCESS_EXPORT AppPrivate : public QObject
{
	Q_OBJECT

public:
	static const QString masterArgument;
	static const QString purgeArgument;
	static const QString startArgument;
	static const QString restartArgument;

	static const QString terminalMessageFormat;
	static const QString masterMessageFormat;

	static QString generateSingleId(const QString &seed = QString());
	static AppPrivate *p_ptr();

	static void qbackProcMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	static bool p_valid;

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

	void setInstanceId(const QString &id);

	void setupDefaultParser(QCommandLineParser &parser, bool useShortOptions = true);
	void updateLoggingMode(int level);
	void updateLoggingPath(const QString &path);

public Q_SLOTS:
	int initControlFlow(const QCommandLineParser &parser);

private Q_SLOTS:
	int makeMaster(const QCommandLineParser &parser);
	int startMaster(bool isAutoStart = false, bool isRestart = false);
	int restartMaster(const QCommandLineParser &parser);
	int commandMaster();
	int purgeMaster(const QCommandLineParser &parser);

	void newTerminalConnected();
	void terminalLoaded(TerminalPrivate *terminal, bool success);
	void stopMaster(Terminal *term);
	void doExit(int code);

	void beginMasterConnect(const QStringList &arguments, bool isStarter);

private:
	App *q;
};

Q_DECLARE_LOGGING_CATEGORY(loggingCategory)

//custom logging operators
#undef qInfo
#define qInfo() qCInfo(loggingCategory).noquote()
#undef qDebug
#define qDebug() qCDebug(loggingCategory).noquote()
#undef qWarning
#define qWarning() qCWarning(loggingCategory).noquote()
#undef qCritical
#define qCritical() qCCritical(loggingCategory).noquote()

}

#endif // QTBACKGROUNDPROCESS_APP_P_H
