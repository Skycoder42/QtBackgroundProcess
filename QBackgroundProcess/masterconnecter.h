#ifndef MASTERCONNECTER_H
#define MASTERCONNECTER_H

#include <QObject>
#include <QLocalSocket>
#include <QRunnable>
#include <QThreadPool>

namespace QBackgroundProcess {

class MasterConnecter : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit MasterConnecter(const QString &instanceId,
							 const QStringList &arguments,
							 bool isStarter,
							 QObject *parent = nullptr);
	~MasterConnecter();

	// QRunnable interface
	void run() override;

private slots:
	void connected();
	void disconnected();
	void error(QLocalSocket::LocalSocketError socketError);
	void readyRead();

	void doWrite(const QByteArray &data);

private:
	const QStringList arguments;
	const bool isStarter;

	QLocalSocket *socket;
	QThreadPool *readThread;
	bool isRunning;
};

}

#endif // MASTERCONNECTER_H
