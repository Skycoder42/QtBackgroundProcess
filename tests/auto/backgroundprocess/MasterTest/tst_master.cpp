#include <QString>
#include <QtTest>
#include <QCoreApplication>

class MasterTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void testSingleInstance();
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

void MasterTest::testSingleInstance()
{
	QVERIFY2(true, "Failure");
}

QTEST_MAIN(MasterTest)

#include "tst_master.moc"
