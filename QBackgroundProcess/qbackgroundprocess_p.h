#ifndef QBACKGROUNDPROCESSPRIVATE_H
#define QBACKGROUNDPROCESSPRIVATE_H

#include "qbackgroundprocess.h"
#include <QLockFile>
#include <QScopedPointer>

class QBackgroundProcessPrivate : public QObject
{
	Q_OBJECT

public:
	static const QString masterArgument;

	static QString generateSingleId(const QString &seed = QString());

	bool running;

	QString instanceId;
	std::function<int(QStringList)> startupFunc;

	QScopedPointer<QLockFile> masterLock;

	QBackgroundProcessPrivate(QObject *parent);

	void setInstanceId(const QString &id);

public slots:
	int initControlFlow();

private slots:
	int makeMaster(const QStringList &arguments);
	int startMaster(const QStringList &arguments);
	int testMasterRunning(const QStringList &arguments);

	void beginMasterConnect(const QStringList &arguments, bool isStarter);
};

#endif // QBACKGROUNDPROCESSPRIVATE_H
