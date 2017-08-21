#include "processhelper.h"
#include <QtTest>

const char ProcessHelper::Stamp = '%';

ProcessHelper::ProcessHelper(QObject *parent) :
	QObject(parent),
	process(new QProcess(this))
{
	process->setProgram(QStringLiteral(OUTDIR) + QStringLiteral("../../../../examples/backgroundprocess/DemoApp/DemoApp"));

	connect(process, &QProcess::errorOccurred,
			this, &ProcessHelper::errorOccurred);
	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			this, &ProcessHelper::finished);
}

void ProcessHelper::start(const QByteArrayList &commands)
{
	QStringList s;
	foreach(auto c, commands)
		s.append(QString::fromUtf8(c));
	s.append({QStringLiteral("--logpath"), QDir::temp().absoluteFilePath(QStringLiteral("MasterTest.log"))});
	process->setArguments(s);
	process->start();
	QVERIFY2(process->waitForStarted(5000), qUtf8Printable(process->errorString()));
	QThread::msleep(500);
}

void ProcessHelper::setExpectedOutLog(const QByteArrayList &log)
{

}

void ProcessHelper::setExpectedErrLog(const QByteArrayList &log)
{

}

void ProcessHelper::waitForFinished()
{
	if(!process->waitForFinished(5000)) {
		process->terminate();
		QFAIL("Process did not stop by itself");
	}
}

void ProcessHelper::waitForFinished(const QList<ProcessHelper *> &helpers)
{
	foreach (auto h, helpers)
		h->waitForFinished();
}

void ProcessHelper::clearLog()
{
	auto logFile = QDir::temp().absoluteFilePath(QStringLiteral("MasterTest.log"));
	if(QFile::exists(logFile))
		QVERIFY(QFile::remove(logFile));
}

void ProcessHelper::verifyMasterLog(const QByteArrayList &log)
{
	auto logFile = QDir::temp().absoluteFilePath(QStringLiteral("MasterTest.log"));
	QFile file(logFile);
	QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qUtf8Printable(file.errorString()));

	auto index = 0;
	while(!file.atEnd()) {
		auto logStr = file.readLine().trimmed();
		if(logStr.isEmpty())
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

	file.close();
	QVERIFY(file.remove());
}

void ProcessHelper::errorOccurred(QProcess::ProcessError error)
{
	QFAIL(qUtf8Printable(process->errorString()));
}

void ProcessHelper::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QCOMPARE(exitStatus, QProcess::NormalExit);
	QCOMPARE(exitCode, EXIT_SUCCESS);
}
