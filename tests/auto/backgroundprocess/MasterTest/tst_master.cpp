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
	void logLevelTest();
#ifdef Q_OS_UNIX
	void masterTermTest();
#endif
	void testCleanup();
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

	p1->verifyLogEmpty();
	p2->verifyLogEmpty();
#ifdef Q_OS_WIN
	p3->verifyLog({
					  "[Warning]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
				  }, true);
#else
	p3->verifyLog({
					  "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
				  }, true);
#endif
	p4->verifyLogEmpty();
	ProcessHelper::verifyMasterLog({
									   "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test1\") and options: (\"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    skipping starter args: (\"start\", \"Test1\") and options: (\"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    received new command: (\"Test2\") and options: (\"terminallog\")",
									   "[% Debug]    received new command: (\"start\", \"Test\", \"3\") and options: (\"terminallog\")",
									   "[% Debug]    received new command: (\"stop\", \"Test\", \"4\") and options: (\"terminallog\")",
									   "[% Debug]    stop requested with (\"stop\", \"Test\", \"4\") and options: (\"terminallog\")",
									   "[% Debug]    I am quitting!"
								   });

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
	p6->start({"restart", "-f", "1", "-i", "Test6"}, true, 1500);

	auto p7 = new ProcessHelper(this);
	p7->start({"-f", "0", "Test", "7"});

	auto p8 = new ProcessHelper(this);
	p8->start({"Test", "8"});

	auto p9 = new ProcessHelper(this);
	p9->start({"stop", "-f", "1", "Test9"});

	ProcessHelper::waitForFinished({p1, p2, p3, p4, p5, p6, p7, p8, p9});

#ifdef Q_OS_WIN
	p1->verifyLog({
					  "[Critical] QtBackgroundProcess: Master process is not running! Please launch it by using: % start"
				  }, true);
#else
	p1->verifyLog({
					  "[\x1B[31mCritical\x1B[0m] QtBackgroundProcess: Master process is not running! Please launch it by using: % start"
				  }, true);
#endif

	p2->verifyLog({
					  "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test2\") and options: (\"a\", \"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
					  "[% Debug]    skipping starter args: (\"Test2\") and options: (\"a\", \"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
					  "[% Debug]    received new command: () and options: ()",
					  "[% Debug]    received new command: (\"start\", \"Test\", \"4\") and options: (\"terminallog\")",
					  "[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
					  "[% Debug]    received new command: (\"stop\") and options: ()",
					  "[% Debug]    stop requested with (\"stop\") and options: ()",
					  "[% Debug]    I am quitting!"
				  });
	p2->verifyLogEmpty();

	p3->verifyLog({
					  "[% Debug]    received new command: (\"start\", \"Test\", \"4\") and options: (\"terminallog\")",
					  "[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
					  "[% Debug]    received new command: (\"stop\") and options: ()",
					  "[% Debug]    stop requested with (\"stop\") and options: ()",
					  "[% Debug]    I am quitting!"
				  });
#ifdef Q_OS_WIN
	p3->verifyLog({
					  "[Warning]  QtBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!"
				  }, true);
#else
	p3->verifyLog({
					  "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Start commands ignored because master is already running! The terminal will connect with an empty argument list!"
				  }, true);
#endif

	p4->verifyLog({
					  "[% Debug]    received new command: (\"Test5\") and options: (\"a\", \"i\", \"terminallog\")",
					  "[% Debug]    received new command: (\"stop\") and options: ()",
					  "[% Debug]    stop requested with (\"stop\") and options: ()",
					  "[% Debug]    I am quitting!"
				  });
#ifdef Q_OS_WIN
	p4->verifyLog({
					  "[Warning]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
				  }, true);
#else
	p4->verifyLog({
					  "[\x1B[33mWarning\x1B[0m]  QtBackgroundProcess: Master is already running. Start arguments will be passed to it as is"
				  }, true);
#endif

	p5->verifyLog({
					  "[% Debug]    received new command: (\"stop\") and options: ()",
					  "[% Debug]    stop requested with (\"stop\") and options: ()",
					  "[% Debug]    I am quitting!"
				  });
	p5->verifyLogEmpty();

	p6->verifyLog({
					  "[% Debug]    I am quitting!",
					  "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\", \"Test6\") and options: (\"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
					  "[% Debug]    skipping starter args: (\"restart\", \"Test6\") and options: (\"f\", \"i\", \"logpath\", \"loglevel\", \"terminallog\")",
					  "[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
#ifdef Q_OS_WIN
	p6->verifyLog({
					  "[Debug]    QtBackgroundProcess: Master process successfully stopped"
				  }, true);
#else
	p6->verifyLog({
					  "[\x1B[32mDebug\x1B[0m]    QtBackgroundProcess: Master process successfully stopped"
				  }, true);
#endif

	p7->verifyLog({
					  "[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
	p7->verifyLogEmpty();

	p8->verifyLog({
					  "[% Debug]    received new command: (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\", \"Test9\") and options: (\"f\", \"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
	p8->verifyLogEmpty();

	p9->verifyLog({
					  "[% Debug]    I am quitting!"
				  });
	p9->verifyLogEmpty();

	ProcessHelper::verifyMasterLog({
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
								   });

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

	p1->verifyLog({
					  "test1",
					  "[% Debug]    received new command: () and options: (\"f\", \"terminallog\")",
					  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
	p1->verifyLogEmpty();
	p2->verifyLog({
					  "[% Debug]    received new command: () and options: (\"f\", \"terminallog\")",
					  "test2",
					  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
	p2->verifyLogEmpty();
	p3->verifyLog({
					  "test3",
					  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
	p3->verifyLogEmpty();
	p4->verifyLog({
					  "[% Debug]    I am quitting!"
				  });
	p4->verifyLogEmpty();

	ProcessHelper::verifyMasterLog({
									   "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    Master started in echo mode!",
									   "[% Debug]    skipping starter args: (\"start\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    received new command: () and options: (\"terminallog\")",
									   "[% Debug]    received new command: () and options: (\"f\", \"terminallog\")",
									   "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
									   "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
									   "[% Debug]    I am quitting!"
								   });

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

	p1->verifyLog({
					  "[]",
					  "[hello, world]",
					  "[stop, me]"
				  });
	p1->verifyLogEmpty();
	p2->verifyLog({
					  "[hello, world]",
					  "[stop, me]"
				  });
	p2->verifyLogEmpty();
	p3->verifyLog({
					  "[stop, me]"
				  });
	p3->verifyLogEmpty();
	p4->verifyLogEmpty(false);
	p4->verifyLogEmpty();

	ProcessHelper::verifyMasterLog({
									   "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    Master started in status mode!",
									   "[% Debug]    skipping starter args: (\"start\") and options: (\"m\", \"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    received new command: () and options: (\"terminallog\")",
									   "[% Debug]    received new command: (\"hello\", \"world\") and options: (\"terminallog\")",
									   "[% Debug]    received new command: (\"stop\", \"me\") and options: (\"terminallog\")",
									   "[% Debug]    stop requested with (\"stop\", \"me\") and options: (\"terminallog\")",
									   "[% Debug]    I am quitting!"
								   });

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

	p1->verifyLogEmpty(false);
	p1->verifyLogEmpty();
	p2->verifyLog({
					  "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
					  "[% Debug]    skipping starter args: (\"start\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
					  "[% Debug]    received new command: () and options: (\"terminallog\")",
					  "[% Debug]    received new command: () and options: (\"detached\", \"terminallog\")",
					  "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
					  "[% Debug]    I am quitting!"
				  });
	p2->verifyLogEmpty();
	p3->verifyLogEmpty(false);
	p3->verifyLogEmpty();
	p4->verifyLog({
					  "[% Debug]    I am quitting!"
				  });
	p4->verifyLogEmpty();

	ProcessHelper::verifyMasterLog({
									   "[% Debug]    App Master started with arguments: (\"__qbckgrndprcss$start#master~\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    skipping starter args: (\"start\") and options: (\"f\", \"detached\", \"logpath\", \"loglevel\", \"terminallog\")",
									   "[% Debug]    received new command: () and options: (\"terminallog\")",
									   "[% Debug]    received new command: () and options: (\"detached\", \"terminallog\")",
									   "[% Debug]    received new command: (\"stop\") and options: (\"terminallog\")",
									   "[% Debug]    stop requested with (\"stop\") and options: (\"terminallog\")",
									   "[% Debug]    I am quitting!"
								   });

	QCoreApplication::processEvents();
	p1->deleteLater();
	p2->deleteLater();
	p3->deleteLater();
	p4->deleteLater();
}

void MasterTest::logLevelTest()
{

}

#ifdef Q_OS_UNIX
void MasterTest::masterTermTest()
{
}
#endif

void MasterTest::testCleanup()
{

}

QTEST_MAIN(MasterTest)

#include "tst_master.moc"
