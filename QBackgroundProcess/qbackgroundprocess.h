#ifndef QBACKGROUNDPROCESS_H
#define QBACKGROUNDPROCESS_H

#include "qbackgroundprocess_global.h"
#include <QCoreApplication>
#include <QLoggingCategory>
#include <functional>
#include "qtexception.h"

class QBACKGROUNDPROCESSSHARED_EXPORT NotAllowedInRunningStateException : public QtException {
public:
	NotAllowedInRunningStateException(const QString &reason);
};

class QBackgroundProcessPrivate;
class QBACKGROUNDPROCESSSHARED_EXPORT QBackgroundProcess : public QCoreApplication
{
	Q_OBJECT

	Q_PROPERTY(QString instanceID READ instanceID WRITE setInstanceID)
	//TODO start always
	//TODO auto redirect qDebug

public:
	QBackgroundProcess(int &argc, char **argv);
	~QBackgroundProcess();

	Q_INVOKABLE bool isMaster() const;
	QString instanceID() const;

	void setStartupFunction(const std::function<int(QStringList)> &function);
	int exec();

public slots:
	void setInstanceID(QString instanceID, bool useAsSeed = true);

private:
	QBackgroundProcessPrivate* d_ptr;
};

Q_DECLARE_LOGGING_CATEGORY(logQBackgroundProcess)

#undef qApp
#define qApp static_cast<QBackgroundProcess*>(QCoreApplication::instance())

#endif // QBACKGROUNDPROCESS_H
