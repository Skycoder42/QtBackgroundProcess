#ifndef TERMINALPRIVATE_H
#define TERMINALPRIVATE_H

#include "terminal.h"
#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>
#include <QBuffer>

namespace QBackgroundProcess {

class TerminalPrivate : public QObject
{
	Q_OBJECT

public:
	explicit TerminalPrivate(QLocalSocket *socket, QObject *parent);

	QLocalSocket *socket;
	QJsonObject status;

signals:
	void statusLoadComplete(TerminalPrivate *terminal, bool successful);

	void dataReady();

private slots:
	void disconnected();
	void error(QLocalSocket::LocalSocketError socketError);
	void readyRead();

private:
	bool isLoading;
};

}

#endif // TERMINALPRIVATE_H
