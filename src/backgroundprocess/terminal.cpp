#include "terminal.h"
#include "terminal_p.h"
#include "app_p.h"

using namespace QtBackgroundProcess;

#ifdef d
#undef d
#endif
#define d this->d_ptr

Terminal::Terminal(TerminalPrivate *d_ptr, QObject *parent) :
	QIODevice(parent),
	d_ptr(d_ptr)
{
	d->setParent(this);
	open(QIODevice::ReadWrite | QIODevice::Unbuffered);

	connect(d->socket, &QLocalSocket::disconnected,
			this, &Terminal::terminalDisconnected);
	connect(d->socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
			this, [this](QLocalSocket::LocalSocketError e) {
		if(e != QLocalSocket::PeerClosedError) {
			setErrorString(d->socket->errorString());
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

QSharedPointer<QCommandLineParser> Terminal::parser() const
{
	return d->parser;
}

bool Terminal::isAutoDelete() const
{
	return d->autoDelete;
}

bool Terminal::isSequential() const
{
	return true;
}

void Terminal::close()
{
	d->socket->close();
	QIODevice::close();
}

qint64 Terminal::bytesAvailable() const
{
	return QIODevice::bytesAvailable() + d->socket->bytesAvailable();
}

void Terminal::disconnectTerminal()
{
	d->beginSoftDisconnect();
}

void Terminal::setAutoDelete(bool autoDelete)
{
	d->autoDelete = autoDelete;
}

void Terminal::writeLine(const QByteArray &line, bool flush)
{
	d->socket->write(line + '\n');
	if(flush)
		d->socket->flush();
}

void Terminal::flush()
{
	d->socket->flush();
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
	return QIODevice::open(mode);
}
