#ifndef QTBACKGROUNDPROCESS_MASTERCONNECTER_P_H
#define QTBACKGROUNDPROCESS_MASTERCONNECTER_P_H

#include "qtbackgroundprocess_global.h"

#include <QtCore/QObject>
#include <QtCore/QFile>

#include <QtNetwork/QLocalSocket>

class QConsole;

namespace QtBackgroundProcess {

class Q_BACKGROUNDPROCESS_EXPORT MasterConnecter : public QObject
{
	Q_OBJECT

public:
	explicit MasterConnecter(const QString &instanceId,
							 const QStringList &arguments,
							 bool isStarter,
							 QObject *parent = nullptr);
	~MasterConnecter();

private Q_SLOTS:
	void connected();
	void disconnected();
	void error(QLocalSocket::LocalSocketError socketError);
	void socketReady();

	void stdinReady();

private:
	const QStringList arguments;
	const bool isStarter;

	QLocalSocket *socket;
	QConsole *console;
	QFile *outFile;
};

}

#endif // QTBACKGROUNDPROCESS_MASTERCONNECTER_P_H
