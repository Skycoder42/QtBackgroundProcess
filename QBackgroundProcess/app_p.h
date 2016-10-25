#ifndef QBACKGROUNDPROCESS_APPPRIVATE_H
#define QBACKGROUNDPROCESS_APPPRIVATE_H

#include "app.h"
#include <QLockFile>
#include <QScopedPointer>
#include <QLocalServer>

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

	std::function<int(QStringList)> startupFunc;

	AppPrivate(QObject *parent);

	void setInstanceId(const QString &id);

public slots:
	int initControlFlow();

private slots:
	int makeMaster(const QStringList &arguments);
	int startMaster(const QStringList &arguments, bool hideWarning = false);
	int testMasterRunning(const QStringList &arguments);

	void newTerminalConnected();

	void beginMasterConnect(const QStringList &arguments, bool isStarter);
};

}

#endif // QBACKGROUNDPROCESS_APPPRIVATE_H
