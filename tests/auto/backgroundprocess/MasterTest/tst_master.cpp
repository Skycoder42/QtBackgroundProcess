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
	p3->setExpectedLog({
						   "[Warning]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
					   });

	auto p4 = new ProcessHelper(this);
	p4->start({"stop", "Test", "4"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4});
	ProcessHelper::verifyMasterLog({
									   "[${stamp} Debug]    App Master Started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test1\") and options: ()",
									   "[${stamp} Debug]    skipping starter args: (\"start\", \"Test1\") and options: ()",
									   "[${stamp} Debug]    received new command: (\"Test2\") and options: ()",
									   "[${stamp} Debug]    received new command: (\"start\", \"Test\", \"3\") and options: ()",
									   "[${stamp} Debug]    received new command: (\"stop\", \"Test\", \"4\") and options: ()",
									   "[${stamp} Debug]    stop requested with (\"stop\", \"Test\", \"4\") and options: ()",
								   });
}

QTEST_MAIN(MasterTest)

#include "tst_master.moc"
