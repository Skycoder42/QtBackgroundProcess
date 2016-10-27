#ifndef TERMINAL_H
#define TERMINAL_H

#include <QIODevice>
class QLocalSocket;
#include "qbackgroundprocess_global.h"

namespace QBackgroundProcess {

class TerminalPrivate;
class QBACKGROUNDPROCESSSHARED_EXPORT Terminal : public QIODevice
{
	Q_OBJECT

	Q_PROPERTY(bool starter READ isStarter CONSTANT)
	Q_PROPERTY(QStringList arguments READ arguments CONSTANT)
	Q_PROPERTY(bool autoDelete READ isAutoDelete WRITE setAutoDelete)

public:
	explicit Terminal(TerminalPrivate *d_ptr, QObject *parent = nullptr);
	~Terminal();

	bool isStarter() const;
	QStringList arguments() const;

	bool isSequential() const override;
	void close() override;
	qint64 bytesAvailable() const override;

	bool isAutoDelete() const;

public slots:
	void disconnectTerminal();
	void setAutoDelete(bool autoDelete);

signals:
	void terminalDisconnected();
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
