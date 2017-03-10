#include "qtexception.h"
using namespace QtBackgroundProcess;

QtException::QtException(const QString &what, int code) :
	std::exception(),
	error(what.toUtf8()),
	eCode(code)
{}

QString QtException::qWhat() const
{
	return QString::fromUtf8(this->error);
}

int QtException::code() const
{
	return this->eCode;
}

const char *QtException::what() const noexcept
{
	return this->error.constData();
}
