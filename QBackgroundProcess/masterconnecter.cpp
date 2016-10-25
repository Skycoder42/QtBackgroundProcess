#include "masterconnecter.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtEndian>
#include "app.h"
using namespace QBackgroundProcess;

MasterConnecter::MasterConnecter(const QString &instanceId, const QStringList &arguments, bool isStarter, QObject *parent) :
	QObject(parent),
	arguments(arguments),
	isStarter(isStarter),
	socket(new QLocalSocket(this))
{
	connect(this->socket, &QLocalSocket::connected,
			this, &MasterConnecter::connected);
	connect(this->socket, &QLocalSocket::disconnected,
			this, &MasterConnecter::disconnected);
	connect(this->socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, &MasterConnecter::error);
	connect(this->socket, &QLocalSocket::readyRead,
			this, &MasterConnecter::readyRead);

	this->socket->connectToServer(instanceId);
}

void MasterConnecter::connected()
{
	//send over the status
	QJsonObject status;
	status[QStringLiteral("isStarter")] = this->isStarter;
	status[QStringLiteral("arguments")] = QJsonArray::fromStringList(this->arguments);
	//TODO add readonly/writeonly info, if useful?

	auto data = QJsonDocument(status).toBinaryData();
	QByteArray dataSize(sizeof(quint32), Qt::Uninitialized);
	qToBigEndian<quint32>((quint32)data.size(), dataSize.data());
	this->socket->write(dataSize);
	this->socket->write(data);
	this->socket->flush();

	//TODO start background process to foreward input
}

void MasterConnecter::disconnected()
{
	this->socket->close();
	qApp->quit();
}

void MasterConnecter::error(QLocalSocket::LocalSocketError socketError)
{
	if(socketError != QLocalSocket::PeerClosedError) {
		qCritical() << "Connection to Master process failed with error:"
					<< qUtf8Printable(this->socket->errorString());
		this->socket->disconnectFromServer();
	}
}

void MasterConnecter::readyRead()
{
	//TODO write stdout
}
