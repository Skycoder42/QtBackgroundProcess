#ifndef QTEXCEPTION_H
#define QTEXCEPTION_H

#include "qbackgroundprocess_global.h"
#include <exception>
#include <QString>

namespace QtBackgroundProcess {

//! A basic exception type for Qt Exceptions
class Q_BACKGROUNDPROCESS_EXPORT QtException : public std::exception
{
public:
	//! Creates a new QtException from an error messsage and an error code
	QtException(const QString &what = QString(), int code = 0);

	//! Returns the error string of the exception
	QString qWhat() const;
	//! Returns the error code of the exception
	int code() const;
	//! Returns the error string of the exception as charactar array
	const char *what() const noexcept final;

private:
	const QByteArray error;
	const int eCode;
};

}

#endif // QTEXCEPTION_H
