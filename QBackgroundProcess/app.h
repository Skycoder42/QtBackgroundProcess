#ifndef QBACKGROUNDPROCESS_APP_H
#define QBACKGROUNDPROCESS_APP_H

#include "qbackgroundprocess_global.h"
#include <QCoreApplication>
#include <functional>
#include "qtexception.h"

namespace QBackgroundProcess {

class QBACKGROUNDPROCESSSHARED_EXPORT NotAllowedInRunningStateException : public QtException {
public:
	NotAllowedInRunningStateException(const QString &reason);
};

class AppPrivate;
class QBACKGROUNDPROCESSSHARED_EXPORT App : public QCoreApplication
{
	Q_OBJECT

	Q_PROPERTY(QString instanceID READ instanceID WRITE setInstanceID)
	Q_PROPERTY(bool autoStartMaster READ autoStartMaster WRITE setAutoStartMaster)
	//TODO auto redirect qDebug

public:
	App(int &argc, char **argv);
	~App();

	Q_INVOKABLE bool isMaster() const;
	QString instanceID() const;
	bool autoStartMaster() const;

	void setStartupFunction(const std::function<int(QStringList)> &function);
	int exec();

public slots:
	void setInstanceID(QString instanceID, bool useAsSeed = true);
	void setAutoStartMaster(bool autoStartMaster);

private:
	AppPrivate* d_ptr;
};

}

#undef qApp
#define qApp static_cast<QBackgroundProcess::App*>(QCoreApplication::instance())

#endif // QBACKGROUNDPROCESS_APP_H
