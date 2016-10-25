#ifndef MASTERCONNECTER_H
#define MASTERCONNECTER_H

#include <QObject>
#include <QLocalSocket>

namespace QBackgroundProcess {

class MasterConnecter : public QObject
{
	Q_OBJECT
public:
	explicit MasterConnecter(const QString &instanceId,
							 const QStringList &arguments,
							 bool isStarter,
							 QObject *parent = nullptr);

private slots:
	void connected();
	void disconnected();
	void error(QLocalSocket::LocalSocketError socketError);
	void readyRead();

private:
	const QStringList arguments;
	const bool isStarter;

	QLocalSocket *socket;
};

}

#endif // MASTERCONNECTER_H
