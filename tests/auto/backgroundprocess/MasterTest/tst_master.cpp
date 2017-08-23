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
	void commandsTest();

	void echoTest();
	void statusTest();

	void detachingTest();
#ifdef Q_OS_UNIX
	void masterTermTest();
#endif
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
	QProcess::execute(ProcessHelper::binPath(), {QStringLiteral("stop")});
}

void MasterTest::simpleTest()
{
	auto p1 = new ProcessHelper(this);
	p1->start({"start", "Test1"}, true);

	auto p2 = new ProcessHelper(this);
	p2->start({"Test2"});

	auto p3 = new ProcessHelper(this);
	p3->start({"start", "Test", "3"});

	auto p4 = new ProcessHelper(this);
	p4->start({"stop", "Test", "4"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4});

	QVERIFYERRLOG(p1);
	QVERIFYERRLOG(p2);
#ifdef Q_OS_WIN
	QVERIFYERRLOG(p3, "[Warning]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is");
#else
	QVERIFYERRLOG(p3, "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is");
#endif
	QVERIFYERRLOG(p4);
	QVERIFYMASTERLOG(
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test1\") and options: (\"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    skipping starter args: (\"start\", \"Test1\") and options: (\"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    received new command: (\"Test2\") and options: (\"terminallog\")",
				"[% Debug]    received new command: (\"start\", \"Test\", \"3\") and options: (\"terminallog\")",
				"[% Debug]    received new command: (\"stop\", \"Test\", \"4\") and options: (\"terminallog\")",
				"[% Debug]    stop requested with (\"stop\", \"Test\", \"4\") and options: (\"terminallog\")",
				"[% Debug]    I am quitting!"
	);

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
	p3->deleteLater();
	p4->deleteLater();
}

void MasterTest::commandsTest()
{
	auto p1 = new ProcessHelper(this);
	p1->setExitCode(EXIT_FAILURE);
	p1->start({"Test1"}, true);

	auto p2 = new ProcessHelper(this);
	p2->start({"-a", "-f", "1", "-i", "Test2"}, true);

	auto p3 = new ProcessHelper(this);
	p3->start({"start", "-i", "Test", "3"});

	auto p4 = new ProcessHelper(this);
	p4->start({"start", "Test", "4"});

	auto p5 = new ProcessHelper(this);
	p5->start({"-a", "-i", "Test5"});

	auto p6 = new ProcessHelper(this);
	p6->start({"restart", "-f", "1", "-i", "Test6"}, true, 2000);

	auto p7 = new ProcessHelper(this);
	p7->start({"-f", "0", "Test", "7"});

	auto p8 = new ProcessHelper(this);
	p8->start({"Test", "8"});

	auto p9 = new ProcessHelper(this);
	p9->start({"stop", "-f", "1", "Test9"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4, p5, p6, p7, p8, p9});

#ifdef Q_OS_WIN
	QVERIFYERRLOG(p1, "[Critical] QtBackgroundProcess: Master process is not running! Please launch it by using: % start");
#else
	QVERIFYERRLOG(p1, "[\x1B[31mCritical\x1B[0m] QtBackgroundProcess: Master process is not running! Please launch it by using: % start");
#endif

	QVERIFYOUTLOG(p2,
				  "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test2\") and options: (\"a\", \"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				  "[% Debug]    skipping starter args: (\"Test2\") and options: (\"a\", \"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				  "[% Debug]    received new command: () and options: ()",
				  "[% Debug]    received new command: (\"start\", \"Test\", \"4\") and options: (\"terminallog\")",
				  "[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
				  "[% Debug]    received new command: (\"stop\") and options: ()",
				  "[% Debug]    stop requested with (\"stop\") and options: ()",
				  "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p2);

	QVERIFYOUTLOG(p3,
				  "[% Debug]    received new command: (\"start\", \"Test\", \"4\") and options: (\"terminallog\")",
				  "[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
				  "[% Debug]    received new command: (\"stop\") and options: ()",
				  "[% Debug]    stop requested with (\"stop\") and options: ()",
				  "[% Debug]    I am quitting!"
	);
#ifdef Q_OS_WIN
	QVERIFYERRLOG(p3, "[Warning]  QtBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!");
#else
	QVERIFYERRLOG(p3, "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!");
#endif

	QVERIFYOUTLOG(p4,
				  "[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
				  "[% Debug]    received new command: (\"stop\") and options: ()",
				  "[% Debug]    stop requested with (\"stop\") and options: ()",
				  "[% Debug]    I am quitting!"
	);
#ifdef Q_OS_WIN
	QVERIFYERRLOG(p4, "[Warning]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is");
#else
	QVERIFYERRLOG(p4, "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is");
#endif

	QVERIFYOUTLOG(p5,
				  "[% Debug]    received new command: (\"stop\") and options: ()",
				  "[% Debug]    stop requested with (\"stop\") and options: ()",
				  "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p5);

	QVERIFYOUTLOG(p6,
				  "[% Debug]    I am quitting!",
				  "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test6\") and options: (\"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				  "[% Debug]    skipping starter args: (\"restart\", \"Test6\") and options: (\"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				  "[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				  "[% Debug]    I am quitting!"
	);
#ifdef Q_OS_WIN
	QVERIFYERRLOG(p6, "[Debug]    QtBackgroundProcess: Master process successfully stopped");
#else
	QVERIFYERRLOG(p6, "[\x1B[32mDebug\x1B[0m]    QtBackgroundProcess: Master process successfully stopped");
#endif

	QVERIFYOUTLOG(p7,
				  "[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				   "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p7);

	QVERIFYOUTLOG(p8,
				  "[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				  "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p8);

	QVERIFYOUTLOG(p9, "[% Debug]    I am quitting!");
	QVERIFYERRLOG(p9);

	QVERIFYMASTERLOG(
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test2\") and options: (\"a\", \"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    skipping starter args: (\"Test2\") and options: (\"a\", \"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    received new command: () and options: ()",
				"[% Debug]    received new command: (\"start\", \"Test\", \"4\") and options: (\"terminallog\")",
				"[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
				"[% Debug]    received new command: (\"stop\") and options: ()",
				"[% Debug]    stop requested with (\"stop\") and options: ()",
				"[% Debug]    I am quitting!",
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test6\") and options: (\"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    skipping starter args: (\"restart\", \"Test6\") and options: (\"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    received new command: (\"Test\", \"7\") and options: (\"f\", \"terminallog\")",
				"[% Debug]    received new command: (\"Test\", \"8\") and options: (\"terminallog\")",
				"[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				"[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
				"[% Debug]    I am quitting!"
	);

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
	p3->deleteLater();
	p4->deleteLater();
	p5->deleteLater();
	p6->deleteLater();
	p7->deleteLater();
	p8->deleteLater();
	p9->deleteLater();
}

void MasterTest::echoTest()
{
	auto p1 = new ProcessHelper(this);
	p1->start({"start", "-m", "echo"}, true);
	p1->send("test1");

	auto p2 = new ProcessHelper(this);
	p2->start({});

	auto p3 = new ProcessHelper(this);
	p3->start({"-f", "1"});
	p2->send("test2");
	p3->send("test3");

	auto p4 = new ProcessHelper(this);
	p4->start({"stop"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4});

	QVERIFYOUTLOG(p1,
				  "test1",
				  "[% Debug]    received new command: () and options: (\"f\", \"terminallog\")",
				  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
				   "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p1);
	QVERIFYOUTLOG(p2,
				  "[% Debug]    received new command: () and options: (\"f\", \"terminallog\")",
				  "test2",
				  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p2);
	QVERIFYOUTLOG(p3,
				  "test3",
				  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p3);
	QVERIFYOUTLOG(p4, "[% Debug]    I am quitting!");
	QVERIFYERRLOG(p4);

	QVERIFYMASTERLOG(
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    Master started in echo mode!",
				"[% Debug]    skipping starter args: (\"start\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    received new command: () and options: (\"terminallog\")",
				"[% Debug]    received new command: () and options: (\"f\", \"terminallog\")",
				"[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
				"[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
				"[% Debug]    I am quitting!"
	);

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
	p3->deleteLater();
	p4->deleteLater();
}

void MasterTest::statusTest()
{
	auto p1 = new ProcessHelper(this);
	p1->start({"start", "-m", "status"}, true);

	auto p2 = new ProcessHelper(this);
	p2->start({});

	auto p3 = new ProcessHelper(this);
	p3->start({"hello", "world"});

	auto p4 = new ProcessHelper(this);
	p4->start({"stop", "me"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4});

	QVERIFYOUTLOG(p1,
				  "[]",
				  "[hello, world]",
				  "[stop, me]"
	);
	QVERIFYERRLOG(p1);
	QVERIFYOUTLOG(p2,
				  "[hello, world]",
				  "[stop, me]"
	);
	QVERIFYERRLOG(p2);
	QVERIFYOUTLOG(p3, "[stop, me]");
	QVERIFYERRLOG(p3);
	QVERIFYOUTLOG(p4);
	QVERIFYERRLOG(p4);

	QVERIFYMASTERLOG(
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    Master started in status mode!",
				"[% Debug]    skipping starter args: (\"start\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    received new command: () and options: (\"terminallog\")",
				"[% Debug]    received new command: (\"hello\", \"world\") and options: (\"terminallog\")",
				"[% Debug]    received new command: (\"stop\", \"me\") and options: (\"terminallog\")",
				"[% Debug]    stop requested with (\"stop\", \"me\") and options: (\"terminallog\")",
				"[% Debug]    I am quitting!"
	);

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
	p3->deleteLater();
	p4->deleteLater();
}

void MasterTest::detachingTest()
{
	auto p1 = new ProcessHelper(this);
	p1->start({"start", "-f", "1", "--detached"}, true);
	p1->waitForFinished();

	auto p2 = new ProcessHelper(this);
	p2->start({});
	QEXPECT_FAIL("", "Undetached process does not finish", Continue);
	p2->waitForFinished(false);

	auto p3 = new ProcessHelper(this);
	p3->start({"--detached"});
	p3->waitForFinished();

	auto p4 = new ProcessHelper(this);
	p4->start({"stop"});

	ProcessHelper::waitForFinished({p2, p4});

	QVERIFYOUTLOG(p1);
	QVERIFYERRLOG(p1);
	QVERIFYOUTLOG(p2,
				  "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
				  "[% Debug]    skipping starter args: (\"start\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
				  "[% Debug]    received new command: () and options: (\"terminallog\")",
				  "[% Debug]    received new command: () and options: (\"detached\", \"terminallog\")",
				  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
				  "[% Debug]    I am quitting!"
	);
	QVERIFYERRLOG(p2);
	QVERIFYOUTLOG(p3);
	QVERIFYERRLOG(p3);
	QVERIFYOUTLOG(p4,  "[% Debug]    I am quitting!");
	QVERIFYERRLOG(p4);

	QVERIFYMASTERLOG(
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    skipping starter args: (\"start\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    received new command: () and options: (\"terminallog\")",
				"[% Debug]    received new command: () and options: (\"detached\", \"terminallog\")",
				"[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
				"[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
				"[% Debug]    I am quitting!"
	);

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
	p3->deleteLater();
	p4->deleteLater();
}

#ifdef Q_OS_UNIX
void MasterTest::masterTermTest()
{
	auto p1 = new ProcessHelper(this);
	p1->start({"start", "-m", "pid"}, true);
	p1->waitForFinished();

	auto p2 = new ProcessHelper(this);
	p2->start({});

	p1->termMaster();
	p2->waitForFinished();

	QVERIFYMASTERLOG(
				"[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    Master started in pid mode!",
				"[% Debug]    skipping starter args: (\"start\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
				"[% Debug]    pid written",
				"[% Debug]    received new command: () and options: (\"terminallog\")",
				"[% Debug]    I am quitting!"
	);

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
}
#endif

QTEST_MAIN(MasterTest)

#include "tst_master.moc"
