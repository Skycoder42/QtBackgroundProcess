#ifndef QBACKGROUNDPROCESS_APPPRIVATE_H
#define QBACKGROUNDPROCESS_APPPRIVATE_H

#include "app.h"
#include <QLockFile>
#include <QScopedPointer>
#include <QLocalServer>
#include "terminal_p.h"

namespace QBackgroundProcess {

class AppPrivate : public QObject
{
	Q_OBJECT

public:
	static const QString masterArgument;

	static QString generateSingleId(const QString &seed = QString());

	bool running;
	bool autoStart;

	QString instanceId;
	QScopedPointer<QLockFile> masterLock;
	QLocalServer *masterServer;
	QList<Terminal*> activeTerminals;

	std::function<int(QStringList)> startupFunc;

	AppPrivate(App *q_ptr);

	void setInstanceId(const QString &id);

public slots:
	int initControlFlow();

private slots:
	int makeMaster(const QStringList &arguments);
	int startMaster(const QStringList &arguments, bool hideWarning = false);
	int testMasterRunning(const QStringList &arguments);

	void newTerminalConnected();
	void terminalLoaded(TerminalPrivate *terminal, bool success);

	void beginMasterConnect(const QStringList &arguments, bool isStarter);

private:
	App *q_ptr;
};

}

#endif // QBACKGROUNDPROCESS_APPPRIVATE_H
