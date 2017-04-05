#ifndef QTBACKGROUNDPROCESS_GLOBALTERMINAL_H
#define QTBACKGROUNDPROCESS_GLOBALTERMINAL_H

#include "QtBackgroundProcess/qtbackgroundprocess_global.h"

#include <QtCore/qiodevice.h>
#include <QtCore/qpointer.h>

namespace QtBackgroundProcess {

class App;
class Terminal;
class GlobalTerminalPrivate;
//! A virtual terminal that forwards input to all connected terminals
class Q_BACKGROUNDPROCESS_EXPORT GlobalTerminal : public QIODevice
{
	Q_OBJECT

public:
	//! Creates a new global terminal
	explicit GlobalTerminal(QObject *parent = nullptr, bool enableBootBuffer = false);
	//! Destructor
	~GlobalTerminal();

	//! @inherit{QIODevice::isSequential}
	bool isSequential() const override;
	//! @inherit{QIODevice::canReadLine}
	bool canReadLine() const override;

public Q_SLOTS:
	//! Performs a flush on all connected terminals @sa Terminal::flush
	void flush();

protected:
	//! @inherit{QIODevice::readData}
	qint64 readData(char *data, qint64 maxlen) override;
	//! @inherit{QIODevice::writeData}
	qint64 writeData(const char *data, qint64 len) override;

private Q_SLOTS:
	bool tryPushBuffer(QList<Terminal*> terms);

private:
	QScopedPointer<GlobalTerminalPrivate> d_ptr;

	bool open(OpenMode mode) override;
	void close() override;

	void pushBuffer(QList<Terminal*> terms);
	void discardBuffer();
};

}

#endif // QTBACKGROUNDPROCESS_GLOBALTERMINAL_H
