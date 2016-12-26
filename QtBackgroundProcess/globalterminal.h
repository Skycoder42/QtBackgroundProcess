#ifndef GLOBALTERMINAL_H
#define GLOBALTERMINAL_H

#include <QIODevice>
#include <QPointer>
#include "qtbackgroundprocess_global.h"

namespace QtBackgroundProcess {

class App;
class Terminal;
class GlobalTerminalPrivate;
//! A virtual terminal that forwards input to all connected terminals
class QTBACKGROUNDPROCESSSHARED_EXPORT GlobalTerminal : public QIODevice
{
	Q_OBJECT

public:
	//! Creates a new global terminal
	explicit GlobalTerminal(QObject *parent = nullptr, bool enableBootBuffer = false);
	//! Destructor
	~GlobalTerminal();

	bool isSequential() const override;
	bool canReadLine() const override;

public slots:
	//! Performs a flush on all connected terminals
	void flush();

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private slots:
	bool tryPushBuffer(QList<Terminal*> terms);

private:
	QScopedPointer<GlobalTerminalPrivate> d_ptr;

	bool open(OpenMode mode) override;
	void close() override;

	void pushBuffer(QList<Terminal*> terms);
	void discardBuffer();
};

}

#endif // GLOBALTERMINAL_H
