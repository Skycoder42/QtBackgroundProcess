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

	void start(const QByteArrayList &commands);

	void waitForFinished();
	void verifyLog(const QByteArrayList &log, bool isError);
	static void waitForFinished(const QList<ProcessHelper*> &helpers);

	static void clearLog();
	static void verifyMasterLog(const QByteArrayList &log);

private Q_SLOTS:
	void errorOccurred(QProcess::ProcessError error);
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess *process;

	static void testLog(const QByteArrayList &log, QIODevice *device);
};

#endif // PROCESSHELPER_H
