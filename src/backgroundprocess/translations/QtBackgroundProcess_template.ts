<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>NotAllowedInRunningStateException</name>
    <message>
        <location filename="../app.cpp" line="+202"/>
        <source>You are not allowed to perform this operation while the application is running!</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QtBackgroundProcess::AppPrivate</name>
    <message>
        <location filename="../app_p.cpp" line="+151"/>
        <source>A control command to control the background application. Possible options are:
 - start: starts the application
 - stop: stops the application
 - restart: stops the application and then starts it again with the given arguments
 - purge_master: purges local servers and lockfiles, in case the master process crashed. Pass &quot;--accept&quot; as second parameter, if you want to skip the prompt.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+11"/>
        <source>It set, the terminal will only pass it&apos;s arguments to the master, and automatically finish after.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set the desired log &lt;level&gt;. Possible values are:
 - 0: log nothing
 - 1: critical errors only
 - 2: like 1 plus warnings
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+5"/>
        <source> - 3: like 2 plus information messages (default)
 - 4: verbose - log everything</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+5"/>
        <source> - 3: like 2 plus information messages
 - 4: verbose - log everything (default)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Overwrites the default log &lt;path&gt;. The default path is platform and application specific. For this instance, it defaults to &quot;%1&quot;. NOTE: The application can override the value internally. Pass an empty string (--logpath &quot;&quot;) to disable logging to a file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Sets the log &lt;level&gt; for terminal only messages. This does not include messages forwarded from the master. Log levels are the same as for the &lt;loglevel&gt; option.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Will prevent the master process from &quot;closing&quot; the console and other stuff that is done to daemonize the process. Can be useful for debugging purpose.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skips the prompt and accepts where possible.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+70"/>
        <source>Failed to create local server with error:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Unable to start master process. Failed with lock error:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Failed to start master process! No master lock was detected.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Start commands ignored because master is already running! The terminal will connect with an empty argument list!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Master is already running. Start arguments will be passed to it as is</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+14"/>
        <source>
Failed to stop the running master process.
Do you want to restart it anyway? (y/N)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Master process is not running! Please launch it by using:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Are you shure you want to purge the master lock and server?
Only do this if the master process is not running anymore, but the lock/server are not available (for example after a crash)
Purging while the master process is still running will crash it.
Press (Y) to purge, or (n) to cancel:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+7"/>
        <source>n</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+0"/>
        <source>N</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Master lockfile successfully removed. It was locked by:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove master lockfile. Lock data is:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>
 - PID: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>
 - Hostname: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>
 - Appname: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No lock file detected</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Master server successfully removed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove master server</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Terminal with invalid commands discarded. Error:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QtBackgroundProcess::MasterConnecter</name>
    <message>
        <location filename="../masterconnecter.cpp" line="+69"/>
        <source>Connection to Master process failed with error:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+47"/>
        <source>stdin was unexpectedly closed! The terminal will not be able to foreward input to the master anymore!</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QtBackgroundProcess::TerminalPrivate</name>
    <message>
        <location filename="../terminal_p.cpp" line="+67"/>
        <source>Terminal closed due to connection error while loading terminal status:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid Terminal status received. Data is corrupted. Terminal will be disconnected</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
