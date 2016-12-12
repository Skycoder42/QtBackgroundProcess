#ifndef MASTERCONNECTER_H
#define MASTERCONNECTER_H

#include <QObject>
#include <QLocalSocket>
#include <QThread>

namespace QtBackgroundProcess {

class MasterConnecter : public QObject
{
	Q_OBJECT

public:
	explicit MasterConnecter(const QString &instanceId,
							 const QStringList &arguments,
							 bool isStarter,
							 QObject *parent = nullptr);
	~MasterConnecter();

private slots:
	void connected();
	void disconnected();
	void error(QLocalSocket::LocalSocketError socketError);
	void readyRead();

	void doWrite(const QByteArray &data);

private:
	class InThread : public QThread {
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
