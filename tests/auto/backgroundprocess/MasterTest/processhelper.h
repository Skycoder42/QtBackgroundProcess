#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QObject>
#include <QProcess>

class ProcessHelper : public QObject
{
	Q_OBJECT

public:
	static const char Stamp;

	static QString binPath();

	explicit ProcessHelper(QObject *parent = nullptr);

	void setExitCode(int code);
	void start(const QByteArrayList &commands, bool logpath = false, int timeout = 1000);

	void send(const QByteArray &message);
	void termMaster();

	void waitForFinished(bool terminate = true);
	bool verifyLog(const QByteArrayList &log, bool isError, const char *file, int line);
	static void waitForFinished(const QList<ProcessHelper*> &helpers);

	static void clearLog();
	static bool verifyMasterLog(const QByteArrayList &log, const char *file, int line);

private Q_SLOTS:
	void errorOccurred(QProcess::ProcessError error);
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess *process;
	int exitCode;

	static QString logPath();
	static bool testLog(const QByteArrayList &log, const QByteArrayList &device, bool fullEqual);
};

#define QVERIFYOUTLOG(helper, ...) \
do {\
	if (!helper->verifyLog({__VA_ARGS__}, false, __FILE__, __LINE__))\
		return;\
} while (0)

#define QVERIFYERRLOG(helper, ...) \
do {\
	if (!helper->verifyLog({__VA_ARGS__}, true, __FILE__, __LINE__))\
		return;\
} while (0)

#define QVERIFYMASTERLOG(...) \
do {\
	if (!ProcessHelper::verifyMasterLog({__VA_ARGS__}, __FILE__, __LINE__))\
		return;\
} while (0)

#endif // PROCESSHELPER_H
