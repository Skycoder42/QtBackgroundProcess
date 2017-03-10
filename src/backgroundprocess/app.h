#ifndef QtBACKGROUNDPROCESS_APP_H
#define QtBACKGROUNDPROCESS_APP_H

#include "qbackgroundprocess_global.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <functional>
#include "qtexception.h"

class QCtrlSignalHandler;
namespace QtBackgroundProcess {

class Terminal;

//! Will be thrown, if you perform an operation, that is not allowed in running state
class Q_BACKGROUNDPROCESS_EXPORT NotAllowedInRunningStateException : public QtException {
public:
	NotAllowedInRunningStateException(const QString &reason);
};

class AppPrivate;
//! The Background process application. The main class of QtBackgroundProcess
class Q_BACKGROUNDPROCESS_EXPORT App : public QCoreApplication
{
	Q_OBJECT
	friend class AppPrivate;

	//! The current id of the singleton instance of the master process
	Q_PROPERTY(QString instanceID READ instanceID WRITE setInstanceID RESET createDefaultInstanceID)
	//! Specifies, whether the master should forward debug output to all terminals
	Q_PROPERTY(bool forwardMasterLog READ forwardMasterLog WRITE setForwardMasterLog)
	//! If true, the master process will always be started, not only with "start"
	Q_PROPERTY(bool autoStartMaster READ autoStartMaster WRITE setAutoStartMaster)
	//! If true, "start" commands will be ignored, if the master is already running
	Q_PROPERTY(bool ignoreMultiStarts READ ignoreMultiStarts WRITE setIgnoreMultiStarts)
	//! If true, the master process will automatically delete terminals that have been disconnected
	Q_PROPERTY(bool autoDeleteTerminals READ autoDeleteTerminals WRITE setAutoDeleteTerminals)
	//! If true, the master process will automatically close terminals after it received the parameters
	Q_PROPERTY(bool autoKillTerminals READ autoKillTerminals WRITE setAutoKillTerminals)
	//! Holds a list of all currently connected terminals
	Q_PROPERTY(QList<Terminal*> connectedTerminals READ connectedTerminals NOTIFY connectedTerminalsChanged)

public:
	//! Creates a new app with it's arguments
	App(int &argc, char **argv, int flags = ApplicationFlags);
	//! Destructor
	~App();

	//! @briefReadAc{App::instanceID}
	QString instanceID() const;
	//! @briefReadAc{App::forwardMasterLog}
	bool forwardMasterLog() const;
	//! @briefReadAc{App::autoStartMaster}
	bool autoStartMaster() const;
	//! @briefReadAc{App::ignoreMultiStarts}
	bool ignoreMultiStarts() const;
	//! @briefReadAc{App::autoDeleteTerminals}
	bool autoDeleteTerminals() const;
	//! @briefReadAc{App::autoKillTerminals}
	bool autoKillTerminals() const;

	//! Sets the function to be called for the creation of the parser (Instead of overriding)
	void setParserSetupFunction(const std::function<void(QCommandLineParser &)> &function);
	//! Sets the function to be called to startup the application (Instead of overriding)
	void setStartupFunction(const std::function<int (const QCommandLineParser &)> &function);
	//! Sets the function to be called to handle shutdown requests (Instead of overriding)
	void setShutdownRequestFunction(const std::function<bool(const QCommandLineParser &)> &function);
	//! Sets the function to be called to handle shutdown requests (Instead of overriding)
	void setShutdownRequestFunction(const std::function<bool(const QCommandLineParser &, int&)> &function);
	//! Sets the function to be called to handle shutdown requests (Instead of overriding)
	void setShutdownRequestFunction(const std::function<bool(Terminal*)> &function);
	//! Sets the function to be called to handle shutdown requests (Instead of overriding)
	void setShutdownRequestFunction(const std::function<bool(Terminal*, int&)> &function);

	//! Executes the application event loop
	int exec();

	//! @briefReadAc{App::connectedTerminals}
	QList<Terminal*> connectedTerminals() const;

	//! Returns the instance of the QCtrlSignalHandler that resides in the library @sa https://github.com/Skycoder42/QCtrlSignals
	QCtrlSignalHandler *signalHandler() const;

public slots:
	//! Creates the default instance id for this application
	void createDefaultInstanceID(bool overwrite = true);
	//! @briefWriteAc{App::instanceID}
	void setInstanceID(QString instanceID, bool useAsSeed = true);
	//! @briefWriteAc{App::forwardMasterLog}
	void setForwardMasterLog(bool forwardMasterLog);
	//! @briefWriteAc{App::autoStartMaster}
	void setAutoStartMaster(bool autoStartMaster);
	//! @briefWriteAc{App::ignoreMultiStarts}
	void setIgnoreMultiStarts(bool ignoreMultiStarts);
	//! @briefWriteAc{App::autoDeleteTerminals}
	void setAutoDeleteTerminals(bool autoDeleteTerminals, bool changeCurrent = false);
	//! @briefWriteAc{App::autoKillTerminals}
	void setAutoKillTerminals(bool autoKillTerminals, bool killCurrent = false);

signals:
	//! Will be emitted when a new terminal has connected to the master
	void newTerminalConnected(QtBackgroundProcess::Terminal *terminal, QPrivateSignal);
	//! Will be emitted when a new terminal sent arguments to the master
	void commandReceived(QSharedPointer<QCommandLineParser> parser, bool isStarter, QPrivateSignal);

	//! @briefNotifyAc{App::connectedTerminals}
	void connectedTerminalsChanged(QList<Terminal*> connectedTerminals, QPrivateSignal);

protected:
	//! Sets up the parser to parse commands and arguments
	virtual void setupParser(QCommandLineParser &parser, bool useShortOptions = true);
	//! Is called as initialization function of the master process
	virtual int startupApp(const QCommandLineParser &parser);
	//! Gets called when a terminal requests a shutdown of the master
	virtual bool requestAppShutdown(Terminal *terminal, int &exitCode);

private:
	AppPrivate* d_ptr;
	bool m_forwardMasterLog;
};

}

#undef qApp
#define qApp static_cast<QtBackgroundProcess::App*>(QCoreApplication::instance())

#endif // QtBACKGROUNDPROCESS_APP_H
