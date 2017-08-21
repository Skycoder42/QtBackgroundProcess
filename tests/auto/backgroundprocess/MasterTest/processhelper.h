#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QObject>
#include <QProcess>

class ProcessHelper : public QObject
{
	Q_OBJECT

public:
	static const QByteArray Stamp;

	explicit ProcessHelper(QObject *parent = nullptr);

	void start(const QByteArrayList &commands);
	void setExpectedLog(const QByteArrayList &log);

	void waitForFinished();
	static void waitForFinished(const QList<ProcessHelper*> &helpers);

	static void verifyMasterLog(const QByteArrayList &log);

private Q_SLOTS:
	void errorOccurred(QProcess::ProcessError error);
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess *process;
};

#endif // PROCESSHELPER_H
