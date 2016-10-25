#include <qbackgroundprocess.h>
#include <QDebug>

int main(int argc, char *argv[])
{
	QBackgroundProcess a(argc, argv);

	a.setStartupFunction([&](const QStringList &args){
		qDebug() << "I AM THE MASTER :D I was started with:"
				 << args;
		return EXIT_SUCCESS;
	});

	return a.exec();
}
