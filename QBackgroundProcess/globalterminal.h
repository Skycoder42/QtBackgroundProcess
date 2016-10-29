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

	Q_PROPERTY(bool writeMode READ isWriteMode WRITE activateWriteMode NOTIFY writeModeChanged)

public:
	explicit GlobalTerminal(App *app, QObject *parent = nullptr);

	bool isSequential() const override;
	bool canReadLine() const override;

	bool isWriteMode() const;

public slots:
	bool activateWriteMode(bool activateWriteMode = true);
	inline bool deactivateWriteMode(bool deactivateWriteMode = true) {
		return this->activateWriteMode(!deactivateWriteMode);
	}

signals:
	void writeModeChanged(bool writeMode);

protected:
	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

private:
	static QPointer<GlobalTerminal> writeOwner;

	App *app;
	bool writeMode;

	bool open(OpenMode mode) override;
	void close() override;
};

}

#endif // GLOBALTERMINAL_H
