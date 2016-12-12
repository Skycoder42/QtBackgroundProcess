//#include <QtBackgroundProcess>
//#include <QDebug>
//#include <QTimer>
//#include <QPointer>

//#define TERMINAL_MODE 0
//#define MASTER_MODE 1
//#define MODE TERMINAL_MODE

//int main(int argc, char *argv[])
//{
//	QtBackgroundProcess::App a(argc, argv);
//	QtBackgroundProcess::App::activateTerminalDebugRedirect();

//	qDebug() << "Log file located at:" << QtBackgroundProcess::App::defaultLogPath();

//	//instead of settings a startup func you can simply override QtBackgroundProcess::App::startupApp
//	a.setStartupFunction([&](const QStringList &args){
//		QtBackgroundProcess::App::activateMasterLogging();
//		//Uncomment the line below so send all debug output to all terminals instead of showing it in the master
//		QtBackgroundProcess::App::activateMasterDebugRedirect();//this can be used together with the logging, if both are activated, both will work

//		qDebug() << "I AM THE MASTER :D I was started with:"
//				 << args;

//		QObject::connect(&a, &QtBackgroundProcess::App::connectedTerminalsChanged, [](QList<QtBackgroundProcess::Terminal*> t){
//			qDebug() << "Connected terminals:" << t.size();
//		});

//		if(MODE == TERMINAL_MODE) {
//			QObject::connect(&a, &QtBackgroundProcess::App::newTerminalConnected, [](QtBackgroundProcess::Terminal *terminal){
//				qDebug() << "new connection"
//						 << (terminal->isStarter() ? "[starter]" : "")
//						 << "with arguments:"
//						 << terminal->arguments();

//				QObject::connect(terminal, &QtBackgroundProcess::Terminal::terminalDisconnected, [](){
//					qDebug() << "Terminal disconnected!";
//				});
//				QObject::connect(terminal, &QtBackgroundProcess::Terminal::terminalError, [=](){
//					qDebug() << "Terminal error:" << terminal->errorString();
//				});
//				QObject::connect(terminal, &QtBackgroundProcess::Terminal::readyRead, [=](){
//					qDebug() << "termout:" << terminal->readAll();
//				});

//				terminal->write("Baum");
//				terminal->write("==");
//				terminal->write("42\n");

//				terminal->setAutoDelete(true);
//				QPointer<QtBackgroundProcess::Terminal> pTerm(terminal);
//				QTimer::singleShot(5000, [=](){
//					if(pTerm) {
//						pTerm->write("Dude, do you even terminal?\n");
//						pTerm->disconnectTerminal();
//					}
//				});
//			});
//		} else {
//			//a.setAutoKillTerminals(true, true);
//			a.setAutoDeleteTerminals(true, true);
//			QObject::connect(&a, &QtBackgroundProcess::App::commandReceived, [](QStringList a){
//				qDebug() << "Command received:" << a;
//			});

//			auto gTerm = new QtBackgroundProcess::GlobalTerminal(&a, &a);
//			auto tmr = new QTimer(&a);
//			QObject::connect(tmr, &QTimer::timeout, [=](){
//				gTerm->write("Mikeichalt");
//			});
//			tmr->start(1000);
//		}

//		return EXIT_SUCCESS;
//	});
//	//instead of settings a shutdown func you can simply override QtBackgroundProcess::App:shutdownApp
//	a.setShutdownFunction([](QtBackgroundProcess::Terminal *terminal) {
//		terminal->write(QString("Shutdown invoked with: (" + terminal->arguments().join(", ") + ")\n").toUtf8());
//		if(terminal->arguments().contains("baum")) {
//			terminal->write("Shutdown accepted!\n");
//			return true;
//		} else {
//			terminal->write("Shutdown denied!\n");
//			return false;
//		}
//	});

//	return a.exec();
//}
