#include <QBackgroundProcess>
#include <QDebug>
#include <QTimer>
#include <QPointer>

#define TERMINAL_MODE 0
#define MASTER_MODE 1
#define MODE TERMINAL_MODE

int main(int argc, char *argv[])
{
	QBackgroundProcess::App a(argc, argv);
	QBackgroundProcess::App::activateTerminalDebugRedirect();

	qDebug() << "TEST";

	//instead of settings a startup func you can simply override QBackgroundProcess::App::startupApp
	a.setStartupFunction([&](const QStringList &args){
		//Uncomment the line below so send all debug output to all terminals instead of showing it in the master
		//QBackgroundProcess::App::activateMasterDebugRedirect();

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
	//instead of settings a shutdown func you can simply override QBackgroundProcess::App:shutdownApp
	a.setShutdownFunction([](QBackgroundProcess::Terminal *terminal) {
		terminal->write(QString("Shutdown invoked with: (" + terminal->arguments().join(", ") + ")\n").toUtf8());
		if(terminal->arguments().contains("baum")) {
			terminal->write("Shutdown accepted!\n");
			return true;
		} else {
			terminal->write("Shutdown denied!\n");
			return false;
		}
	});

	return a.exec();
}
