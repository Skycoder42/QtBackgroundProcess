#include "terminal.h"
#include "terminal_p.h"
using namespace QBackgroundProcess;

Terminal::Terminal(TerminalPrivate *d_ptr, QObject *parent) :
	QObject(parent),
	d_ptr(d_ptr)
{
	this->d_ptr->setParent(this);
}
