#include "app.h"
#include "app_p.h"
using namespace QBackgroundProcess;

#ifdef d
#undef d
#endif
#define d this->d_ptr

App::App(int &argc, char **argv) :
	QCoreApplication(argc, argv),
	d_ptr(new AppPrivate(this))
{}

App::~App(){}

bool App::isMaster() const
{
	if(d->masterLock)
		return d->masterLock->isLocked();
	else
		return false;
}

QString App::instanceID() const
{
	return d->instanceId;
}

bool App::autoStartMaster() const
{
	return d->autoStart;
}

void App::setStartupFunction(const std::function<int(QStringList)> &function)
{
	d->startupFunc = function;
}

int App::exec()
{
	d->running = true;

	//generate the single id
	if(d->instanceId.isNull())
		d->setInstanceId(AppPrivate::generateSingleId());

	auto res = d->initControlFlow();
	if(res != EXIT_SUCCESS)
		return res;
	res = QCoreApplication::exec();
	d->running = false;
	return res;
}

void App::setInstanceID(QString instanceID, bool useAsSeed)
{
	if(d->running)
		throw NotAllowedInRunningStateException(QStringLiteral("Change the instance id"));

	if(useAsSeed)
		d->setInstanceId(AppPrivate::generateSingleId(instanceID));
	else
		d->setInstanceId(instanceID);
}

void App::setAutoStartMaster(bool autoStartMaster)
{
	d->autoStart = autoStartMaster;
}



NotAllowedInRunningStateException::NotAllowedInRunningStateException(const QString &reason) :
	QtException(QStringLiteral("You are not allowed to perform the following action while the application is running: ") + reason)
{}
