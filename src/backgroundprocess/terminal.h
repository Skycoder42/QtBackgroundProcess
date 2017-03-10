#ifndef TERMINAL_H
#define TERMINAL_H

#include "QtBackgroundProcess/qbackgroundprocess_global.h"

#include <QtCore/qcommandlineparser.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qsharedpointer.h>

class QLocalSocket;

namespace QtBackgroundProcess {

class TerminalPrivate;
//! Represents a connection to a terminal in the master process
class Q_BACKGROUNDPROCESS_EXPORT Terminal : public QIODevice
{
	Q_OBJECT

	//! Specifies, whether this terminal is the one that started the master process
	Q_PROPERTY(bool starter READ isStarter CONSTANT)
	//! Returns a reference to the parser that parsed this terminals arguments
	Q_PROPERTY(QSharedPointer<QCommandLineParser> parser READ parser CONSTANT)
	//! If true, the terminal will delete itself as soon as the connection has been closed
	Q_PROPERTY(bool autoDelete READ isAutoDelete WRITE setAutoDelete)

public:
	//! Creates a new terminal from it's private implementation
	explicit Terminal(TerminalPrivate *d_ptr, QObject *parent = nullptr);
	//! Destructor
	~Terminal();

	//! @briefReadAc{App::starter}
	bool isStarter() const;
	//! @briefReadAc{App::parser}
	QSharedPointer<QCommandLineParser> parser() const;
	//! @briefReadAc{App::autoDelete}
	bool isAutoDelete() const;

	bool isSequential() const override;
	void close() override;
	qint64 bytesAvailable() const override;

public slots:
	//! Disconnects the terminal from the master
	void disconnectTerminal();
	//! @briefWriteAc{App::autoDelete}
	void setAutoDelete(bool autoDelete);

	//! Flushes the terminal
	void flush();

signals:
	//! Will be emitted after the terminal has been disconnected
	void terminalDisconnected();
	//! Will be emitted if an error occures
	void terminalError(int errorCode);

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private:
	TerminalPrivate *d_ptr;

	bool open(OpenMode mode) override;
};

}

#endif // TERMINAL_H
