#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QObject>
#include <QProcess>

class ProcessHelper : public QObject
{
	Q_OBJECT

public:
	static const char Stamp;

	explicit ProcessHelper(QObject *parent = nullptr);

	void setExitCode(int code);
	void start(const QByteArrayList &commands, bool logpath = false, int timeout = 500);

	void waitForFinished();
	void verifyLog(const QByteArrayList &log, bool isError = false);
	void verifyLogEmpty(bool isError = true);
	static void waitForFinished(const QList<ProcessHelper*> &helpers);

	static void clearLog();
	static void verifyMasterLog(const QByteArrayList &log);

private Q_SLOTS:
	void errorOccurred(QProcess::ProcessError error);
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess *process;
	int exitCode;
	static bool allGreen;

	static QString logPath();
	static void testLog(const QByteArrayList &log, QIODevice *device);
};

#endif // PROCESSHELPER_H
