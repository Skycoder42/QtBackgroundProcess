#ifndef MASTERCONNECTER_H
#define MASTERCONNECTER_H

#include "qtbackgroundprocess_global.h"

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QtNetwork/QLocalSocket>

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
	void readyRead();

	void doWrite(const QByteArray &data);

private:
	class Q_BACKGROUNDPROCESS_EXPORT InThread : public QThread {
	public:
		InThread(MasterConnecter *parent);

	protected:
		void run() override;
	};

	const QStringList arguments;
	const bool isStarter;

	QLocalSocket *socket;
	InThread *readThread;
};

}

#endif // MASTERCONNECTER_H
