#include "processhelper.h"
#include <QtTest>

const char ProcessHelper::Stamp = '%';
bool ProcessHelper::allGreen = true;

QString ProcessHelper::binPath()
{
#if defined(Q_OS_WIN)
	return QStringLiteral(OUTDIR) + QStringLiteral("../../../../examples/backgroundprocess/DemoApp/") + QStringLiteral(RMODE) + QStringLiteral("/DemoApp");
#elif defined(Q_OS_MAC)
	return QStringLiteral(OUTDIR) + QStringLiteral("../../../../examples/backgroundprocess/DemoApp/DemoApp.app/Contents/MacOS/DemoApp");
#else
	return QStringLiteral(OUTDIR) + QStringLiteral("../../../../examples/backgroundprocess/DemoApp/DemoApp");
#endif
}

ProcessHelper::ProcessHelper(QObject *parent) :
	QObject(parent),
	process(new QProcess(this)),
	exitCode(EXIT_SUCCESS)
{
	process->setProgram(binPath());

	connect(process, &QProcess::errorOccurred,
			this, &ProcessHelper::errorOccurred);
	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			this, &ProcessHelper::finished);
}

void ProcessHelper::setExitCode(int code)
{
	exitCode = code;
}

void ProcessHelper::start(const QByteArrayList &commands, bool logpath, int timeout)
{
	QStringList s;
	foreach(auto c, commands)
		s.append(QString::fromUtf8(c));
	if(logpath) {
		s.append({QStringLiteral("--logpath"), logPath()});
		s.append({QStringLiteral("--loglevel"), QStringLiteral("4")});
	}
	s.append({QStringLiteral("--terminallog"), QStringLiteral("4")});

	process->setArguments(s);
	process->start(QIODevice::ReadWrite);
	QVERIFY2(process->waitForStarted(5000), qUtf8Printable(process->errorString()));
	QThread::msleep(timeout);
}

void ProcessHelper::send(const QByteArray &message)
{
	process->write(message + '\n');
	QCoreApplication::processEvents();
}

void ProcessHelper::waitForFinished(bool terminate)
{
	if(!process->waitForFinished(5000)) {
		if(terminate)
			process->terminate();
		QVERIFY2(false, "Process did not stop by itself");
	}
}

void ProcessHelper::verifyLog(const QByteArrayList &log, bool isError)
{
	allGreen = false;

	if(isError) {
		auto err = process->readAllStandardError();
		QBuffer buffer(&err);
		buffer.open(QIODevice::ReadOnly | QIODevice::Text);
		testLog(log, &buffer);
		buffer.close();
	} else
		testLog(log, process);

	if(!allGreen)
		qDebug() << "on arguments:" << process->arguments();
}

void ProcessHelper::verifyLogEmpty(bool isError)
{
	verifyLog(QByteArrayList(), isError);
}

void ProcessHelper::waitForFinished(const QList<ProcessHelper *> &helpers)
{
	foreach (auto h, helpers)
		h->waitForFinished();
}

void ProcessHelper::clearLog()
{
	auto logFile = logPath();
	if(QFile::exists(logFile))
		QVERIFY(QFile::remove(logFile));
}

void ProcessHelper::verifyMasterLog(const QByteArrayList &log)
{
	auto logFile = logPath();
	QFile file(logFile);
	QVERIFY(file.exists());
	QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qUtf8Printable(file.errorString()));

	allGreen = false;
	testLog(log, &file);
	if(!allGreen)
		qDebug() << "on master log";

	file.close();
	QVERIFY(file.remove());

}

void ProcessHelper::errorOccurred(QProcess::ProcessError error)
{
	Q_UNUSED(error)
	QFAIL(qUtf8Printable(process->errorString()));
}

void ProcessHelper::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QCOMPARE(exitStatus, QProcess::NormalExit);
	QCOMPARE(exitCode, this->exitCode);
}

QString ProcessHelper::logPath()
{
	return QDir::temp().absoluteFilePath(QStringLiteral("MasterTest.log"));
}

void ProcessHelper::testLog(const QByteArrayList &log, QIODevice *device)
{
	auto index = 0;
	while(!device->atEnd()) {
		auto logStr = device->readLine().trimmed();
		if(logStr.isEmpty())
			continue;
		//TODO win debug
		if(logStr.contains("QCtrlSignals"))
			continue;

		QVERIFY2(index < log.size(), logStr.constData());
		auto testStr = log[index++];

		auto testSplit = testStr.split(Stamp);
		if(testSplit.size() == 2) {
			QVERIFY2(logStr.startsWith(testSplit[0]), logStr.constData());
			QVERIFY2(logStr.endsWith(testSplit[1]), logStr.constData());
		} else
			QCOMPARE(logStr, testStr);
	}
	QCOMPARE(index, log.size());
	allGreen = true;
}
