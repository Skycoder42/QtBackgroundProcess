#include "masterconnecter_p.h"
#include "app_p.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include <iostream>
#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

using namespace QtBackgroundProcess;

MasterConnecter::MasterConnecter(const QString &instanceId, const QStringList &arguments, bool isStarter, QObject *parent) :
	QObject(parent),
	arguments(arguments),
	isStarter(isStarter),
	socket(new QLocalSocket(this)),
	readThread(new InThread(this))
{
	connect(socket, &QLocalSocket::connected,
			this, &MasterConnecter::connected);
	connect(socket, &QLocalSocket::disconnected,
			this, &MasterConnecter::disconnected);
	connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, &MasterConnecter::error);
	connect(socket, &QLocalSocket::readyRead,
			this, &MasterConnecter::readyRead);

	socket->connectToServer(instanceId);
}

MasterConnecter::~MasterConnecter()
{
	readThread->requestInterruption();
#ifdef Q_OS_UNIX
	//close stdin to "cancel" the blocking read
	close(fileno(stdin));
#endif
	if(!readThread->wait(500)) {
		readThread->terminate();
		readThread->wait(100);//additional wait, to complete termination
	}
}

void MasterConnecter::connected()
{
	//send over the status
	QJsonObject status;
	status[QStringLiteral("isStarter")] = isStarter;
	status[QStringLiteral("arguments")] = QJsonArray::fromStringList(arguments);

	QDataStream connectData(socket);
	connectData << QJsonDocument(status).toBinaryData();
	socket->flush();

	//begin stdin reading
	readThread->start();
}

void MasterConnecter::disconnected()
{
	socket->close();
	qApp->quit();
}

void MasterConnecter::error(QLocalSocket::LocalSocketError socketError)
{
	if(socketError != QLocalSocket::PeerClosedError) {
		qCCritical(loggingCategory) << tr("Connection to Master process failed with error:")
					<< qUtf8Printable(socket->errorString());
		socket->disconnectFromServer();
		qApp->exit(EXIT_FAILURE);
	}
}

void MasterConnecter::readyRead()
{
	auto data  = socket->readAll();
	std::cout.write(data.constData(), data.size());
	std::cout.flush();
}

void MasterConnecter::doWrite(const QByteArray &data)
{
	socket->write(data);
	socket->flush();
}



MasterConnecter::InThread::InThread(MasterConnecter *parent) :
	QThread(parent)
{
	setTerminationEnabled(true);
}

void MasterConnecter::InThread::run()
{
	QFile inFile;
	inFile.open(stdin, QIODevice::ReadOnly);
	while(!isInterruptionRequested()) {
		if(inFile.error() != QFile::NoError)
			break;

		auto data = inFile.readLine();
		if(data.isEmpty())
			QThread::msleep(100);
		else {
			QMetaObject::invokeMethod(parent(), "doWrite", Qt::QueuedConnection,
									  Q_ARG(const QByteArray &, data));
		}
	}

	inFile.close();
	if(!isInterruptionRequested())
		qCCritical(loggingCategory) << tr("stdin was unexpectedly closed! The terminal will not be able to foreward input to the master anymore!");
}
