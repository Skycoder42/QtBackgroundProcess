#include "masterconnecter_p.h"
#include "app_p.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include <qconsole.h>

using namespace QtBackgroundProcess;

MasterConnecter::MasterConnecter(const QString &socketName, const QStringList &arguments, bool isStarter, QObject *parent) :
	QObject(parent),
	arguments(arguments),
	isStarter(isStarter),
	socket(new QLocalSocket(this)),
	console(new QConsole(this)),
	outFile(QConsole::qStdOut(this))
{
	connect(socket, &QLocalSocket::connected,
			this, &MasterConnecter::connected);
	connect(socket, &QLocalSocket::disconnected,
			this, &MasterConnecter::disconnected);
	connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, &MasterConnecter::error);
	connect(socket, &QLocalSocket::readyRead,
			this, &MasterConnecter::socketReady,
			Qt::QueuedConnection); //queued connection, because of "socket not ready" errors on win

	connect(console, &QConsole::readyRead,
			this, &MasterConnecter::stdinReady);

	socket->connectToServer(socketName);
}

MasterConnecter::~MasterConnecter()
{
	console->close();
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
	console->open();
}

void MasterConnecter::disconnected()
{
	socket->close();
	qApp->quit();
}

void MasterConnecter::error(QLocalSocket::LocalSocketError socketError)
{
	if(socketError != QLocalSocket::PeerClosedError) {
		qCritical() << tr("Connection to Master process failed with error:")
					<< socket->errorString();
		socket->disconnectFromServer();
		qApp->exit(EXIT_FAILURE);
	}
}

void MasterConnecter::socketReady()
{
	auto data  = socket->readAll();
	outFile->write(data);
	outFile->flush();
}

void MasterConnecter::stdinReady()
{
	auto bytes = console->bytesAvailable();
	auto data  = console->read(bytes);
	socket->write(data);
	socket->flush();
}
