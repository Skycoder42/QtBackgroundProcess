#ifndef TERMINALPRIVATE_H
#define TERMINALPRIVATE_H

#include "terminal.h"

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QBuffer>

#include <QtNetwork/QLocalSocket>

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

Q_SIGNALS:
	void statusLoadComplete(TerminalPrivate *terminal, bool successful);

	void dataReady();

private Q_SLOTS:
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
