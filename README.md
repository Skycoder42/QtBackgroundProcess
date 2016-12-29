# QtBackgroundProcess
A Library to create background applications with simple, automated foreground control.

In short, with this library you can easly create cross platform applications, that are ment to run in the background, without any UI. This however, is fairly easy and would not require a library. This one however adds a few things, that make it much more comfortable to use those.

Have a look at the features.

## Features
- Cross platform, supports all Qt-Platforms, that allow multi-process applications and console applications
- Runs your application in the background, without a console window or similar (so called "master process")
- Runs as a singleton (for the current user / root)
- The background process will always start independently
- A "execution" of an application (for example via the console) will start a "terminal"
  - Terminals will connect to the running master. If none is running, you can start it with the terminal
  - Terminals send their command line arguments to the running master
  - A connected terminal can send input to the master (via stdin) and receive output (stdout)
  - Terminals can disconnect by beeing closed, leaving the master running (even the one that started the master)
  - The master continues running, even if all terminals have been disconnected
  - Terminals can ask the master to stop
- Terminals are a part of the library and work as is. You don't have to do anything with them, only "implement" you application as master process
- The master can send output to all terminals at once, or directly communicate with a terminal
- The master automates logging, by writing it to a log file, and sending it to the terminals (both optional)
- Provides a basic CLI, with usefull commands for the master
  - Can be easily extended by your application
- Integrates the [QCtrlSignals](https://github.com/Skycoder42/QCtrlSignals) to react on typical signals like `SIGINT`

## Use Case
The QtBackgroundProcess is a library for console applications. It is not intended to be used with GUI-Apps or similar, and probably won't work with them. If you are looking for simply a "singleton instance", i.e. a library that will make shure only one instance of your application runs at a time, have a look at [QSingleInstance](https://github.com/Skycoder42/QSingleInstance).

However, if you want to create an application to run silently in the background, and only interact with it to "control" it, you're at the right place.

## Usage
Just download the library and add the line `include(<path_to>/QtBackgroundProcess/qtbackgroundprocess.pri)` to your .pro-file. This way all files and required libraries will automatically be added. Use `#include <QtBackgroundProcess>` to access the class.

### Hint
Instead of simply downloading the library, you can clone the `lib` branch! It contains all the library binaries, includes and more. The best way is to add it as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to your project. This way the library is directly connected to your repository!

### Example
The following example is a very simple background application, that logs all command arguments that have been passed by the terminals. In addition to that, it will send all debug output to all terminals.

```cpp
#include <QtBackgroundProcess>
#include <QDebug>

int main(int argc, char *argv[])
{
    QtBackgroundProcess::App a(argc, argv);
    QCoreApplication::setApplicationVersion("4.2.0");

    qApp->setForwardMasterLog(true);//forwards all debug log to terminals IF this becomes the master
    a.setStartupFunction([](const QCommandLineParser &){
        QObject::connect(qApp, &QtBackgroundProcess::App::commandReceived, qApp, [](QSharedPointer<QCommandLineParser> parser, bool isStarter){
            qDebug() << "Received command with arguments:"
                        << parser->positionalArguments()
                        << "and options:"
                        << parser->optionNames()
                        << (isStarter ? "(starter)" : "");
        });

        qDebug() << "Master process succsesfully stated!";
        return EXIT_SUCCESS;
    });

    return a.exec();
}
```

To find out what you can do with this application, simply call `<path_to>/testApp --help` - this will show the command line arguments that can be used. Use `start` to start the master process and `stop` to stop it. Please remember the the master process will continue to run, even after you have closed all terminals. Use the `stop` command to stop it.

### Debugging the master process
Since all of the application logic is happening in the master process, debugging the terminals does not really help. However, with "normal" commands you can only start terminals, not the master. If you need to debug the master, it can be helpful to start the master process directly, instead of using a terminal. To do so, all you need to do is launch it with a special first argument:

If your normal command looks for example like this: `./myApp start some arguments`<br>
You can directly start the master process by using: `./myApp '__qbckgrndprcss$start#master~' some arguments`

The master process is not meant to be directly run. Thus, it will for example, not show a console window if possible. This makes it hard to get the debug output without a terminal. You can try the following options, depending on what suits you needs:
- start the master with the `--keep-console` argument. This way the master will keep it's console window. **Note:** For this to work for qDebug, the master must not transfer it's debug output to any terminal or the log file.
  - To start with disabled logfile, use: `start -L "" --keep-console ...`
- Watch the log file and reload it on changes
- Start a argument-less terminal after you started the master explicitly to attach and receive the debug output
- Try to re-attach a console window (not recommended)

## Documentation
Not created yet. However, you will already find helpful doxy-style comments in the header files.

## Technical Stuff
Please not, that this library does NOT create a real deamon/service. It merely emulates the behavior. However, this approach allows more flexibility for the different platforms.