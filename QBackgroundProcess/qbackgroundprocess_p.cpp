#include "qbackgroundprocess_p.h"
#include <QRegularExpression>
#include <QThread>
#include <QDir>
#include <QProcess>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

const QString QBackgroundProcessPrivate::masterArgument(QStringLiteral("__qbckgrndprcss$start#master~"));

QString QBackgroundProcessPrivate::generateSingleId(const QString &seed)
{
	auto fullId = QCoreApplication::applicationName().toLower();
	fullId.remove(QRegularExpression(QStringLiteral("[^a-zA-Z0-9_]")));
	fullId.truncate(8);
	fullId.prepend(QStringLiteral("qbackproc-"));
	QByteArray hashBase = (QCoreApplication::organizationName() +
						   QCoreApplication::organizationDomain() +
						   seed).toUtf8();
	fullId += QLatin1Char('-') +
			  QString::number(qChecksum(hashBase.data(), hashBase.size()), 16) +
			  QLatin1Char('-');

#ifdef Q_OS_WIN
	DWORD sessID;
	if(::ProcessIdToSessionId(::GetCurrentProcessId(), &sessID))
		fullId += QString::number(sessID, 16);
#else
	fullId += QString::number(::getuid(), 16);
#endif

	return fullId;
}

QBackgroundProcessPrivate::QBackgroundProcessPrivate(QObject *parent) :
	QObject(parent),
	running(false),
	instanceId(),
	startupFunc(),
	masterLock(nullptr)
{}

void QBackgroundProcessPrivate::setInstanceId(const QString &id)
{
	this->instanceId = id;
	auto lockPath = QDir::temp().absoluteFilePath(id + QStringLiteral(".lock"));
	this->masterLock.reset(new QLockFile(lockPath));
	this->masterLock->setStaleLockTime(0);
}

int QBackgroundProcessPrivate::initControlFlow()
{
	//start/make master
	auto args = QCoreApplication::arguments();
	args.removeFirst();
	if(args.size() > 0) {
		if(args[0] == masterArgument) {
			args.removeFirst();
			return this->makeMaster(args);
		} else if(args[0] == QStringLiteral("start")) {
			args.removeFirst();
			return this->startMaster(args);
		}//TODO else if user start
	}

	//neither start nor make master --> "normal" client
	return this->testMasterRunning(args);
}

int QBackgroundProcessPrivate::makeMaster(const QStringList &arguments)
{
	//TODO start server

	//get the lock
	if(!this->masterLock->tryLock(5000)) {//wait at most 5 sec
		qCritical() << "Unable to start master process. Failed with lock error:"
					<< this->masterLock->error();
		return EXIT_FAILURE;
	} else {
		if(this->startupFunc) {
			auto res = this->startupFunc(arguments);
			if(res != EXIT_SUCCESS)
				return res;//TODO close down
		}
		return EXIT_SUCCESS;
	}
}

int QBackgroundProcessPrivate::startMaster(const QStringList &arguments)
{
	//check if master already lives
	if(this->masterLock->tryLock()) {//no master alive
		auto ok = false;

		auto args = arguments;
		args.prepend(masterArgument);
		if(QProcess::startDetached(QCoreApplication::applicationFilePath(), args)) {//start MASTER with additional start params
			//wait for the master to start
			this->masterLock->unlock();
			for(auto i = 0; i < 50; i++) {//wait at most 5 sec
				qint64 pid;
				QString hostname;
				QString appname;
				if(this->masterLock->getLockInfo(&pid, &hostname, &appname)) {
					ok = true;
					break;
				} else
					QThread::msleep(100);
			}
		}

		if(ok) {//master started --> start to connect
			QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
									  Q_ARG(QStringList, arguments),
									  Q_ARG(bool, true));
			return EXIT_SUCCESS;
		} else {
			qCritical() << "Failed to start master process! No master lock was detected.";
			return EXIT_FAILURE;
		}
	} else {//master is running --> ok
		qWarning() << "Master is already running. Start arguments will be passed to it as is";
		QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
								  Q_ARG(QStringList, arguments),
								  Q_ARG(bool, false));
		return EXIT_SUCCESS;
	}
}

int QBackgroundProcessPrivate::testMasterRunning(const QStringList &arguments)
{
	if(this->masterLock->tryLock()) {
		this->masterLock->unlock();
		qCritical() << "Master process is not running! Please launch it by using:"
					<< QCoreApplication::applicationFilePath() + QStringLiteral(" start");
		return EXIT_FAILURE;
	} else {
		QMetaObject::invokeMethod(this, "beginMasterConnect", Qt::QueuedConnection,
								  Q_ARG(QStringList, arguments),
								  Q_ARG(bool, false));
		return EXIT_SUCCESS;
	}
}

void QBackgroundProcessPrivate::beginMasterConnect(const QStringList &arguments, bool isStarter)
{
	Q_UNIMPLEMENTED();
}
