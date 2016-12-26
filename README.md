# QtBackgroundProcess
A Library to create background applications with simple, automated foreground control.

In short, with this library you can easly create cross platform applications, that are ment to run in the background, without any UI.
This however, is fairly easy and would not require a library. This one however adds a few things, that make it much more comfortable to use those.

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
The QtBackgroundProcess is a library for console applications. It is not intended to be used with GUI-Apps or similar, and probably won't work with them.
If you are looking for simply a "singleton instance", i.e. a library that will make shure only one instance of your application runs at a time, have a look at
[QSingleInstance](https://github.com/Skycoder42/QSingleInstance).

However, if you want to create an application to run silently in the background, and only interact with it to "control" it, you're at the right place.