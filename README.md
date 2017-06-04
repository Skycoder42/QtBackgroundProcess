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

## Download/Installation
There are multiple ways to install the Qt module, sorted by preference:

1. **Arch-Linux only:** If you are building against your system Qt, you can use my AUR-Repository: [qt5-backgroundprocess](https://aur.archlinux.org/packages/qt5-backgroundprocess/)
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
	1. Open the MaintenanceTool, located in your Qt install directory (e.g. `~/Qt/MaintenanceTool`)
	2. Select `Add or remove components` and click on the `Settings` button
	3. Go to `Repositories`, scroll to the bottom, select `User defined repositories` and press `Add`
	4. In the right column (selected by default), type:
		- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
		- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
		- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
	5. Press `Ok`, make shure `Add or remove components` is still selected, and continue the install (`Next >`)
	6. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.8 > Skycoder42 Qt modules`)
	7. You can install either all of my modules, or select the one you need: `Qt Background Process`
	8. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt Version
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation!
4. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
	- `qmake`
	- `make qmake_all`
	- `make`
	- `make install`
	- To include translations and documentation, run the `qmake && make install` once again

However, if you want to create an application to run silently in the background, and only interact with it to "control" it, you're at the right place.

## Usage
The background process is provided as a Qt module. Thus, all you have to do is add the module, and then, in your project, add `QT += backgroundprocess` to your `.pro` file!

All you need to do in your code is to use `QtBackgroundProcess::App` instead of `QCoreApplication` and put your code in the startup function or extend it.

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
The first few commands (i.e. Version number) are done for every instance, including the terminals. The part inside of `setStartupFunction` will be executed on the master only.

To find out what you can do with this application, simply call `<path_to>/myApp --help` - this will show the command line arguments that can be used. Use `start` to start the master process and `stop` to stop it. Please remember the the master process will continue to run, even after you have closed all terminals. Use the `stop` command to stop it. For a demo app, see `examples/backgroundprocess/DemoApp`.

### Debugging the master process
Since all of the application logic is happening in the master process, debugging the terminals does not really help. However, with "normal" commands you can only start terminals, not the master, and thus not debug it. If you need to debug the master, it can be helpful to start the master process directly, instead of using a terminal. To do so, all you need to do is launch it with a special first argument:

If your normal command looks for example like this: `./myApp start some arguments`<br>
You can directly start the master process by using: `./myApp '__qbckgrndprcss$start#master~' some arguments`

The master process is not meant to be directly run. Thus, it will for example, not show a console window, disconnect stdin/stdout, etc. This makes it hard to get the debug without a terminal. You can try the following options, depending on what suits you needs:
- Start the master with a working console. This will only work as long as no terminals are attached, no logfile is used and you don't need stdin. For most situations, this is the recommended way of debugging. Use `-L "" --keep-console` as additional parameters for the master to enable this.
- Watch the log file and reload it on changes, i.e. use `tail -f logfile`. To get the location, run `--help` and see the `-L` parameter. You can use terminals normally
- Enable log forwarding to pass qDebug messages to all connected terminals. See QtBackgroundProcess::App::forwardMasterLog. Simply start a parameterless terminal after launching the master to see the output.

## Documentation
The documentation is available on [github pages](https://skycoder42.github.io/QtBackgroundProcess/). It was created using [doxygen](http://www.doxygen.org/). The HTML-documentation and Qt-Help files are shipped
together with the module for both the custom repository and the package on the release page. Please note that doxygen docs do not perfectly integrate with QtCreator/QtAssistant.

## Translations
The project is prepared for translation. But since I speak only english and german, those are the only languages I can provide translations for. However, you can easily create the translations yourself. The file `src/backgroundprocess/translations/qtbackgroundprocess_template.ts` is a ready-made TS file. Just rename it (e.g. to `qtbackgroundprocess_jp.ts`) and open it with the QtLinguist to create the translations.

## Technical Stuff
Please not, that this library does **not** create a real daemon/service. It merely creates a background process, with advanced terminal connections. This approach allows more flexibility and an easier cross-platform implementation. In addition to that, interaction with the process becomes possible everywhere, without any additional code.
