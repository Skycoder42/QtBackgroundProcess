#include "globalterminal.h"
#include "app_p.h"
using namespace QBackgroundProcess;

QPointer<GlobalTerminal> GlobalTerminal::writeOwner(nullptr);

GlobalTerminal::GlobalTerminal(App *app, QObject *parent) :
	QIODevice(parent),
	app(app)
{
	this->open(QIODevice::WriteOnly | QIODevice::Unbuffered);
}

bool GlobalTerminal::isSequential() const
{
	return true;
}

bool GlobalTerminal::canReadLine() const
{
	return false;
}

bool GlobalTerminal::isWriteMode() const
{
	return this->writeMode;
}

bool GlobalTerminal::activateWriteMode(bool activateWriteMode)
{
	if(activateWriteMode) {
		if(this->writeMode)
			return true;
		else {
			if(writeOwner.isNull()) {
				writeOwner = this;//TODO activate
				emit writeModeChanged(activateWriteMode);
				return true;
			} else
				return false;
		}
	} else {
		if(!this->writeMode)
			return true;
		else {
			if(writeOwner == this) {
				writeOwner = nullptr;//TODO deactivate
				emit writeModeChanged(activateWriteMode);
				return true;
			} else
				return false;
		}
	}
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
