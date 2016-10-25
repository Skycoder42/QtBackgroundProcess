#include "masterconnecter.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtEndian>
#include <QFile>
#include <iostream>
#include "app.h"
using namespace QBackgroundProcess;

MasterConnecter::MasterConnecter(const QString &instanceId, const QStringList &arguments, bool isStarter, QObject *parent) :
	QObject(parent),
	arguments(arguments),
	isStarter(isStarter),
	socket(new QLocalSocket(this)),
	readThread(new QThreadPool(this)),
	isRunning(true)
{
	this->setAutoDelete(false);
	this->readThread->setMaxThreadCount(1);

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

MasterConnecter::~MasterConnecter()
{
	this->isRunning = false;
	this->readThread->waitForDone(5000);
}

void MasterConnecter::run()
{
	QFile inFile;
	inFile.open(stdin, QIODevice::ReadOnly);
	while(this->isRunning) {
		if(inFile.error() != QFile::NoError)
			break;

		auto data = inFile.readLine();
		if(data.isEmpty())
			QThread::msleep(100);
		else {
			QMetaObject::invokeMethod(this, "doWrite", Qt::QueuedConnection,
									  Q_ARG(QByteArray, data));
		}
	}

	inFile.close();
	if(this->isRunning)
		qCritical() << "stdin was unexpectedly closed! The terminal will not be able to foreward input to the master anymore!";
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

	//begin stdin reading
	this->readThread->start(this);
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
	auto data  = this->socket->readAll();
	std::cout.write(data.constData(), data.size());
}

void MasterConnecter::doWrite(const QByteArray &data)
{
	this->socket->write(data);
	this->socket->flush();
}
