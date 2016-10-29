#ifndef GLOBALTERMINAL_H
#define GLOBALTERMINAL_H

#include <QIODevice>
#include <QPointer>
#include "qbackgroundprocess_global.h"

namespace QBackgroundProcess {

class App;
class QBACKGROUNDPROCESSSHARED_EXPORT GlobalTerminal : public QIODevice
{
	Q_OBJECT

public:
	explicit GlobalTerminal(App *app, QObject *parent = nullptr);
	~GlobalTerminal();

	bool isSequential() const override;
	bool canReadLine() const override;

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private:
	App *app;

	bool open(OpenMode mode) override;
	void close() override;
};

}

#endif // GLOBALTERMINAL_H
