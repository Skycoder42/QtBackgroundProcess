#ifndef QBACKGROUNDPROCESS_APP_H
#define QBACKGROUNDPROCESS_APP_H

#include "qbackgroundprocess_global.h"
#include <QCoreApplication>
#include <functional>
#include "qtexception.h"

namespace QBackgroundProcess {

class Terminal;

class QBACKGROUNDPROCESSSHARED_EXPORT NotAllowedInRunningStateException : public QtException {
public:
	NotAllowedInRunningStateException(const QString &reason);
};

class AppPrivate;
class QBACKGROUNDPROCESSSHARED_EXPORT App : public QCoreApplication
{
	Q_OBJECT
	friend class AppPrivate;

	Q_PROPERTY(QString instanceID READ instanceID WRITE setInstanceID)
	Q_PROPERTY(bool autoStartMaster READ autoStartMaster WRITE setAutoStartMaster)
	Q_PROPERTY(bool ignoreMultiStarts READ ignoreMultiStarts WRITE setIgnoreMultiStarts)
	//TODO auto redirect qDebug

public:
	App(int &argc, char **argv);
	~App();

	QString instanceID() const;
	bool autoStartMaster() const;
	bool ignoreMultiStarts() const;

	void setStartupFunction(const std::function<int(QStringList)> &function);
	int exec();

	QList<Terminal*> connectedTerminals() const;

public slots:
	void setInstanceID(QString instanceID, bool useAsSeed = true);
	void setAutoStartMaster(bool autoStartMaster);
	void setIgnoreMultiStarts(bool ignoreMultiStarts);

signals:
	void newTerminalConnected(QBackgroundProcess::Terminal *terminal, QPrivateSignal);

private:
	AppPrivate* d_ptr;
};

}

#undef qApp
#define qApp static_cast<QBackgroundProcess::App*>(QCoreApplication::instance())

#endif // QBACKGROUNDPROCESS_APP_H
