#ifndef TERMINALPRIVATE_H
#define TERMINALPRIVATE_H

#include "qbackgroundprocess_global.h"
#include "terminal.h"
#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>
#include <QBuffer>

namespace QtBackgroundProcess {

class Q_BACKGROUNDPROCESS_EXPORT TerminalPrivate : public QObject
{
	Q_OBJECT

public:
	explicit TerminalPrivate(QLocalSocket *socket, QObject *parent);

	QLocalSocket *socket;
	QJsonObject status;
	QSharedPointer<QCommandLineParser> parser;
	bool autoDelete;

	bool loadParser();
	void beginSoftDisconnect();

signals:
	void statusLoadComplete(TerminalPrivate *terminal, bool successful);

	void dataReady();

private slots:
	void disconnected();
	void error(QLocalSocket::LocalSocketError socketError);
	void readyRead();
	void writeReady();

private:
	bool isLoading;
	bool disconnecting;
};

}

#endif // TERMINALPRIVATE_H
