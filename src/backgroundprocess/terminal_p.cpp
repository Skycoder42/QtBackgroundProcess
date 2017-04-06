#include "terminal_p.h"
#include "app_p.h"

#include <QtCore/QtEndian>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QTimer>

using namespace QtBackgroundProcess;

TerminalPrivate::TerminalPrivate(QLocalSocket *socket, QObject *parent) :
	QObject(parent),
	socket(socket),
	status(),
	parser(),
	autoDelete(false),
	isLoading(true),
	disconnecting(false)
{
	socket->setParent(this);

	connect(socket, &QLocalSocket::disconnected,
			this, &TerminalPrivate::disconnected);
	connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, &TerminalPrivate::error);
	connect(socket, &QLocalSocket::readyRead,
			this, &TerminalPrivate::readyRead);
	connect(socket, &QLocalSocket::bytesWritten,
			this, &TerminalPrivate::writeReady);
}

bool TerminalPrivate::loadParser()
{
	auto array = status[QStringLiteral("arguments")].toArray();
	QStringList lst(QCoreApplication::applicationFilePath());
	foreach(auto value, array)
		lst.append(value.toString());
	return parser->parse(lst);
}

void TerminalPrivate::beginSoftDisconnect()
{
	disconnecting = true;
	if(socket->bytesToWrite() > 0) {
		QTimer::singleShot(500, this, [=](){
			if(socket->state() == QLocalSocket::ConnectedState)
				socket->disconnectFromServer();
		});
		socket->flush();
	} else
		socket->disconnectFromServer();
}

void TerminalPrivate::disconnected()
{
	if(isLoading) {
		emit statusLoadComplete(this, false);
		socket->close();
	} else if(autoDelete && parent())
		parent()->deleteLater();
}

void TerminalPrivate::error(QLocalSocket::LocalSocketError socketError)
{
	if(socketError != QLocalSocket::PeerClosedError) {
		if(isLoading) {
			qCWarning(loggingCategory) << tr("Terminal closed due to connection error while loading terminal status:")
					   << qUtf8Printable(socket->errorString());
		}
		socket->disconnectFromServer();
	}
}

void TerminalPrivate::readyRead()
{
	if(isLoading) {
		if(socket->bytesAvailable() < (qint64)sizeof(quint32))
			return;
		auto size = qFromBigEndian<quint32>(socket->peek(sizeof(quint32)));
		if(socket->bytesAvailable() >= (qint64)(size + sizeof(quint32))) {
			socket->read(sizeof(quint32));
			auto doc = QJsonDocument::fromBinaryData(socket->read(size));

			if(doc.isNull()) {
				qCWarning(loggingCategory) << tr("Invalid Terminal status received. Data is corrupted. Terminal will be disconnected");
				socket->disconnectFromServer();
				return;
			} else {
				status = doc.object();
				isLoading = false;
				emit statusLoadComplete(this, true);
			}
		}
	}
}

void TerminalPrivate::writeReady()
{
	if(disconnecting && socket->bytesToWrite() == 0) {
		QTimer::singleShot(500, this, [=](){
			if(socket->state() == QLocalSocket::ConnectedState)
				socket->disconnectFromServer();
		});
	}
}
