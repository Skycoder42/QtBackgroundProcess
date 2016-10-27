#include <QBackgroundProcess>
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[])
{
	QBackgroundProcess::App a(argc, argv);

	a.setStartupFunction([&](const QStringList &args){
		qDebug() << "I AM THE MASTER :D I was started with:"
				 << args;

		QObject::connect(&a, &QBackgroundProcess::App::newTerminalConnected, [](QBackgroundProcess::Terminal *terminal){
			qDebug() << "new connection"
					 << (terminal->isStarter() ? "[starter]" : "")
					 << "with arguments:"
					 << terminal->arguments();

			QObject::connect(terminal, &QBackgroundProcess::Terminal::terminalDisconnected, [](){
				qDebug() << "Terminal disconnected!";
			});
			QObject::connect(terminal, &QBackgroundProcess::Terminal::terminalError, [=](){
				qDebug() << "Terminal error:" << terminal->errorString();
			});
			QObject::connect(terminal, &QBackgroundProcess::Terminal::readyRead, [=](){
				qDebug() << "termout:" << terminal->readAll();
			});

			terminal->write("Baum");
			terminal->write("==");
			terminal->write("42\n");

			terminal->setAutoDelete(true);
			QTimer::singleShot(5000, [=](){
				terminal->write("Dude, do you even terminal?\n");
				terminal->disconnectTerminal();
			});
		});

		return EXIT_SUCCESS;
	});

	return a.exec();
}
