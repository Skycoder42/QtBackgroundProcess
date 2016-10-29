#include <QBackgroundProcess>
#include <QDebug>
#include <QTimer>
#include <QPointer>

#define TERMINAL_MODE 0
#define MASTER_MODE 1
#define MODE MASTER_MODE

int main(int argc, char *argv[])
{
	QBackgroundProcess::App a(argc, argv);

	a.setStartupFunction([&](const QStringList &args){
		qDebug() << "I AM THE MASTER :D I was started with:"
				 << args;

		QObject::connect(&a, &QBackgroundProcess::App::connectedTerminalsChanged, [](QList<QBackgroundProcess::Terminal*> t){
			qDebug() << "Connected terminals:" << t.size();
		});

		if(MODE == TERMINAL_MODE) {
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
				QPointer<QBackgroundProcess::Terminal> pTerm(terminal);
				QTimer::singleShot(5000, [=](){
					if(pTerm) {
						pTerm->write("Dude, do you even terminal?\n");
						pTerm->disconnectTerminal();
					}
				});
			});
		} else {
			//a.setAutoKillTerminals(true, true);
			a.setAutoDeleteTerminals(true, true);
			QObject::connect(&a, &QBackgroundProcess::App::commandReceived, [](QStringList a){
				qDebug() << "Command received:" << a;
			});

			auto gTerm = new QBackgroundProcess::GlobalTerminal(&a, &a);
			auto tmr = new QTimer(&a);
			QObject::connect(tmr, &QTimer::timeout, [=](){
				gTerm->write("Mikeichalt");
			});
			tmr->start(1000);
		}

		return EXIT_SUCCESS;
	});

	return a.exec();
}
