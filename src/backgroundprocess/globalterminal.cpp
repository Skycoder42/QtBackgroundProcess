#include "globalterminal.h"
#include "app_p.h"

#include <QtCore/QTimer>

namespace QtBackgroundProcess {

class GlobalTerminalPrivate {
public:
	App *app;
	QBuffer *buffer;

	GlobalTerminalPrivate(bool enableBootBuffer, GlobalTerminal *q_ptr) :
		app(qApp),
		buffer(enableBootBuffer ? new QBuffer(q_ptr) : nullptr)
	{}
};

}

using namespace QtBackgroundProcess;

GlobalTerminal::GlobalTerminal(QObject *parent, bool enableBootBuffer) :
	QIODevice(parent),
	d(new GlobalTerminalPrivate(enableBootBuffer, this))
{
	if(enableBootBuffer) {
		connect(d->app, &App::connectedTerminalsChanged,
				this, &GlobalTerminal::tryPushBuffer);
		d->buffer->open(QIODevice::WriteOnly);

		QTimer::singleShot(10*1000, this, [this](){//wait up to 10 seconds for the first terminal to connect
			if(d->buffer && !tryPushBuffer(d->app->connectedTerminals()))
				discardBuffer();
		});
	}

	open(QIODevice::WriteOnly | QIODevice::Unbuffered);
}

GlobalTerminal::~GlobalTerminal()
{
	QIODevice::close();
}

bool GlobalTerminal::isSequential() const
{
	return true;
}

bool GlobalTerminal::canReadLine() const
{
	return false;
}

void GlobalTerminal::writeLine(const QByteArray &line, bool doFlush)
{
	write(line + '\n');
	if(doFlush)
		flush();
}

void GlobalTerminal::flush()
{
	auto terms = d->app->connectedTerminals();
	if(d->buffer && !terms.isEmpty())
		pushBuffer(terms);
	foreach(auto term, terms)
		term->flush();
}

qint64 GlobalTerminal::readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64 GlobalTerminal::writeData(const char *data, qint64 len)
{
	auto terms = d->app->connectedTerminals();
	if(d->buffer) {
		if(terms.isEmpty())
			return d->buffer->write(data, len);
		else
			pushBuffer(terms);
	}

	foreach(auto term, terms)
		term->write(data, len);
	return len;
}

bool GlobalTerminal::tryPushBuffer(QList<Terminal *> terms)
{
	if(d->buffer && !terms.isEmpty()) {
		pushBuffer(terms);
		return true;
	} else
		return false;
}

bool GlobalTerminal::open(QIODevice::OpenMode mode)
{
	return QIODevice::open(mode);
}

void GlobalTerminal::close() {}

void GlobalTerminal::pushBuffer(QList<Terminal *> terms)
{
	const auto &data = d->buffer->data();
	if(!data.isEmpty()) {
		foreach(auto term, terms)
			term->write(data);
	}
	discardBuffer();
}

void GlobalTerminal::discardBuffer()
{
	d->buffer->close();
	d->buffer->deleteLater();
	d->buffer = nullptr;
	disconnect(d->app, &App::connectedTerminalsChanged,
			   this, &GlobalTerminal::tryPushBuffer);
}
