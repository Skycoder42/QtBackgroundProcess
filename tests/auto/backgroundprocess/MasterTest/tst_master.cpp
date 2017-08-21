#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "processhelper.h"

class MasterTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void simpleTest();
};

void MasterTest::initTestCase()
{
#ifdef Q_OS_LINUX
	Q_ASSERT(qgetenv("LD_PRELOAD").contains("Qt5BackgroundProcess"));
#endif
	ProcessHelper::clearLog();
}

void MasterTest::cleanupTestCase()
{
}

void MasterTest::simpleTest()
{
	auto p1 = new ProcessHelper(this);
	p1->start({"start", "Test1"});

	auto p2 = new ProcessHelper(this);
	p2->start({"Test2"});

	auto p3 = new ProcessHelper(this);
	p3->start({"start", "Test", "3"});

	auto p4 = new ProcessHelper(this);
	p4->start({"stop", "Test", "4"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4});
#ifdef Q_OS_WIN
	p3->verifyLog({
					  "[Warning]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
				  }, true);
#else
	p3->verifyLog({
					  "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
				  }, true);
#endif
	ProcessHelper::verifyMasterLog({
									   "[% Debug]    App Master Started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test1\") and options: (\"logpath\")",
									   "[% Debug]    skipping starter args: (\"start\", \"Test1\") and options: (\"logpath\")",
									   "[% Debug]    received new command: (\"Test2\") and options: (\"logpath\")",
									   "[% Debug]    received new command: (\"start\", \"Test\", \"3\") and options: (\"logpath\")",
									   "[% Debug]    received new command: (\"stop\", \"Test\", \"4\") and options: (\"logpath\")",
									   "[% Debug]    stop requested with (\"stop\", \"Test\", \"4\") and options: (\"logpath\")",
									   "[% Debug]    I am quitting!"
								   });

	QCoreApplication::processEvents();
}

QTEST_MAIN(MasterTest)

#include "tst_master.moc"
