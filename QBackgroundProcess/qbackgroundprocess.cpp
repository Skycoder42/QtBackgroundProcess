#include "qbackgroundprocess.h"
#include "qbackgroundprocess_p.h"

#ifdef d
#undef d
#endif
#define d this->d_ptr

QBackgroundProcess::QBackgroundProcess(int &argc, char **argv) :
	QCoreApplication(argc, argv),
	d_ptr(new QBackgroundProcessPrivate(this))
{}

QBackgroundProcess::~QBackgroundProcess(){}

bool QBackgroundProcess::isMaster() const
{
	if(d->masterLock)
		return d->masterLock->isLocked();
	else
		return false;
}

QString QBackgroundProcess::instanceID() const
{
	return d->instanceId;
}

void QBackgroundProcess::setStartupFunction(const std::function<int(QStringList)> &function)
{
	d->startupFunc = function;
}

int QBackgroundProcess::exec()
{
	d->running = true;

	//generate the single id
	if(d->instanceId.isNull())
		d->setInstanceId(QBackgroundProcessPrivate::generateSingleId());

	auto res = d->initControlFlow();
	if(res != EXIT_SUCCESS)
		return res;
	res = QCoreApplication::exec();
	d->running = false;
	return res;
}

void QBackgroundProcess::setInstanceID(QString instanceID, bool useAsSeed)
{
	if(d->running)
		throw NotAllowedInRunningStateException(QStringLiteral("Change the instance id"));

	if(useAsSeed)
		d->setInstanceId(QBackgroundProcessPrivate::generateSingleId(instanceID));
	else
		d->setInstanceId(instanceID);
}



NotAllowedInRunningStateException::NotAllowedInRunningStateException(const QString &reason) :
	QtException(QStringLiteral("You are not allowed to perform the following action while the application is running: ") + reason)
{}
