#ifndef GLOBALTERMINAL_H
#define GLOBALTERMINAL_H

#include <QIODevice>
#include <QPointer>
#include "qtbackgroundprocess_global.h"

namespace QtBackgroundProcess {

class App;
class Terminal;
class GlobalTerminalPrivate;
class QTBACKGROUNDPROCESSSHARED_EXPORT GlobalTerminal : public QIODevice
{
	Q_OBJECT

public:
	explicit GlobalTerminal(App *app, QObject *parent = nullptr, bool enableBootBuffer = false);
	~GlobalTerminal();

	bool isSequential() const override;
	bool canReadLine() const override;

public slots:
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
