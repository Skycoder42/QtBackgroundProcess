#include "globalterminal.h"
#include "app_p.h"
using namespace QBackgroundProcess;

GlobalTerminal::GlobalTerminal(App *app, QObject *parent) :
	QIODevice(parent),
	app(app)
{
	this->open(QIODevice::WriteOnly | QIODevice::Unbuffered);
}

GlobalTerminal::~GlobalTerminal()
{
	this->QIODevice::close();
}

bool GlobalTerminal::isSequential() const
{
	return true;
}

bool GlobalTerminal::canReadLine() const
{
	return false;
}

qint64 GlobalTerminal::readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64 GlobalTerminal::writeData(const char *data, qint64 len)
{
	auto terms = this->app->connectedTerminals();
	foreach(auto term, terms)
		term->write(data, len);
	return len;
}

bool GlobalTerminal::open(QIODevice::OpenMode mode)
{
	return this->QIODevice::open(mode);
}

void GlobalTerminal::close() {}
