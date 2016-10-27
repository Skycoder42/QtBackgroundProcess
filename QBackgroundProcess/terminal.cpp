#include "terminal.h"
#include "terminal_p.h"
#include <QJsonArray>
using namespace QBackgroundProcess;

#ifdef d
#undef d
#endif
#define d this->d_ptr

Terminal::Terminal(TerminalPrivate *d_ptr, QObject *parent) :
	QIODevice(parent),
	d_ptr(d_ptr)
{
	d->setParent(this);
	this->open(QIODevice::ReadWrite | QIODevice::Unbuffered);

	connect(d->socket, &QLocalSocket::disconnected,
			this, &Terminal::terminalDisconnected);
	connect(d->socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, [this](QLocalSocket::LocalSocketError e) {
		if(e != QLocalSocket::PeerClosedError) {
			this->setErrorString(d->socket->errorString());
			emit terminalError((int)e);
		}
	});

	connect(d->socket, &QLocalSocket::channelReadyRead,
			this, &Terminal::channelReadyRead);
	connect(d->socket, &QLocalSocket::readyRead,
			this, &Terminal::readyRead);
}

Terminal::~Terminal()
{
	if(d->socket->isOpen())
		d->socket->close();
}

bool Terminal::isStarter() const
{
	return d->status[QStringLiteral("isStarter")].toBool();
}

QStringList Terminal::arguments() const
{
	auto array = d->status[QStringLiteral("arguments")].toArray();
	QStringList lst;
	foreach(auto value, array)
		lst.append(value.toString());
	return lst;
}

bool Terminal::isSequential() const
{
	return true;
}

void Terminal::disconnectTerminal()
{
	d->beginSoftDisconnect();
}

void Terminal::close()
{
	d->socket->close();
	this->QIODevice::close();
}

qint64 Terminal::bytesAvailable() const
{
	return this->QIODevice::bytesAvailable() + d->socket->bytesAvailable();
}

bool Terminal::isAutoDelete() const
{
	return d->autoDelete;
}

void Terminal::setAutoDelete(bool autoDelete)
{
	d->autoDelete = autoDelete;
}

qint64 Terminal::readData(char *data, qint64 maxlen)
{
	return d->socket->read(data, maxlen);
}

qint64 Terminal::writeData(const char *data, qint64 len)
{
	return d->socket->write(data, len);
}

bool Terminal::open(QIODevice::OpenMode mode)
{
	return this->QIODevice::open(mode);
}
