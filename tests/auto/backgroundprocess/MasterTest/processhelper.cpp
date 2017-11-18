#include "processhelper.h"
#include <QtTest>
#ifdef Q_OS_UNIX
#include <sys/types.h>
#include <signal.h>
#endif

const char ProcessHelper::Stamp = '%';

QString ProcessHelper::binPath()
{
#if defined(Q_OS_WIN)
	return QStringLiteral(OUTDIR) + QStringLiteral("../../../../examples/backgroundprocess/DemoApp/") + QStringLiteral(RMODE) + QStringLiteral("/DemoApp");
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

void ProcessHelper::termMaster()
{
	auto line = process->readAll();
	auto ok = false;
	auto pid = line.toInt(&ok);
	QVERIFY2(ok, line.constData());

#ifdef Q_OS_UNIX
	kill(pid, SIGTERM);
#endif
}

void ProcessHelper::waitForFinished(bool terminate)
{
	if(!process->waitForFinished(5000)) {
		if(terminate)
			process->terminate();
		QVERIFY2(false, "Process did not stop by itself");
	}
}

bool ProcessHelper::verifyLog(const QByteArrayList &log, bool isError, const char *file, int line)
{
	QByteArray data;
	if(isError)
		data = process->readAllStandardError();
	else
		data = process->readAllStandardOutput();
	auto dataList = data.split('\n');

	if(!testLog(log, dataList, isError)) {
		QByteArray resStr = "Log does not contain device output for ";
		if(isError)
			resStr += "stderr:\n";
		else
			resStr += "stdout:\n";
		resStr += "Expected Log:";
		foreach(auto line, log)
			resStr += "\n\t" + line;
		resStr += "\nActual Log:";
		foreach(auto line, dataList)
			resStr += "\n\t" + line;
		QTest::qFail(resStr.constData(), file, line);
		return false;
	} else
		return true;
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

bool ProcessHelper::verifyMasterLog(const QByteArrayList &log, const char *file, int line)
{
	auto logFile = logPath();
	QFile masterFile(logFile);
	if (!QTest::qVerify(masterFile.exists(), "masterLog.exists", "File does not exist", file, line))
		return false;
	if (!QTest::qVerify(masterFile.open(QIODevice::ReadOnly | QIODevice::Text), "masterLog.open", qUtf8Printable(masterFile.errorString()), file, line))
		return false;

	auto dataList = masterFile.readAll().split('\n');
	auto res = testLog(log, dataList, true);
	if(!res) {
		QByteArray resStr = "Master Log is not as expected:\nExpected Log:";
		foreach(auto line, log)
			resStr += "\n\t" + line;
		resStr += "\nActual Log:";
		foreach(auto line, dataList)
			resStr += "\n\t" + line;
		QTest::qFail(resStr.constData(), file, line);
	}

	masterFile.close();
	if (!QTest::qVerify(masterFile.remove(), "masterLog.remove", qUtf8Printable(masterFile.errorString()), file, line))
		return false;

	return res;
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

bool ProcessHelper::testLog(const QByteArrayList &log, const QByteArrayList &device, bool fullEqual)
{
	auto index = 0;

	foreach(auto line, device) {
		auto logStr = line.trimmed();
		if(logStr.isEmpty())
			continue;
		//TODO win debug
		if(logStr.contains("QCtrlSignals") ||
		   logStr.contains("XDG_RUNTIME_DIR"))
			continue;

		bool fullOk;
		do {
			fullOk = true;
			if(index >= log.size())
				return false;
			auto testStr = log[index++];

			auto testSplit = testStr.split(Stamp);
			if(testSplit.size() == 2) {
				if(logStr.startsWith(testSplit[0]) &&
				   logStr.endsWith(testSplit[1]))
					break;
			}

			if(logStr == testStr)
				break;

			fullOk = false;
		} while(!fullEqual);

		if(!fullOk)
			return false;
	}

	return !fullEqual || index == log.size();
}
