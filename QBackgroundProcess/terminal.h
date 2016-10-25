#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
class QLocalSocket;
#include "qbackgroundprocess_global.h"

namespace QBackgroundProcess {

class TerminalPrivate;
class QBACKGROUNDPROCESSSHARED_EXPORT Terminal : public QObject
{
	Q_OBJECT

public:
	explicit Terminal(TerminalPrivate *d_ptr, QObject *parent = nullptr);

private:
	TerminalPrivate *d_ptr;
};

}

#endif // TERMINAL_H
