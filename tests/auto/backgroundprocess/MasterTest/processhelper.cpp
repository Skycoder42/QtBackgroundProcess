#include "processhelper.h"
#include <QtTest>

const QByteArray ProcessHelper::Stamp = "${stamp}";

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
	process->setArguments(s);
	process->start();
	QVERIFY2(process->waitForStarted(5000), qUtf8Printable(process->errorString()));
	QThread::msleep(500);
}

void ProcessHelper::setExpectedLog(const QByteArrayList &log)
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

void ProcessHelper::verifyMasterLog(const QByteArrayList &log)
{

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
