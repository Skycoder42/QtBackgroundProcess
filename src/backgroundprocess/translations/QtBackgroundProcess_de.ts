<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
<context>
    <name>NotAllowedInRunningStateException</name>
    <message>
        <location filename="../app.cpp" line="+202"/>
        <source>You are not allowed to perform this operation while the application is running!</source>
        <translation>Diese Operation ist nicht erlaubt während die Anwendung läuft!</translation>
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
        <translation>Ein Kontroll-Kommando um die Hintergrundanwendung zu starten. Mögliche Kommandos sind:
 - start: Startet die Anwendung
 - stop: Stoppt die Anwendung
 - restart: Stoppt die Anwendung und startet sie danach erneut mit den gegebenen Argumenten
 - purge_master: Entfernt lokale server und lockfiles, falls der Master-Prozess abgestürzt ist. Übergeben sie &quot;--accept&quot; als zweiten Parameter, falls sie die Abfrage überpringen möchten.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>It set, the terminal will only pass it&apos;s arguments to the master, and automatically finish after.</source>
        <translation>Wenn diese Option gesetzt ist, dann werden nur die Argumente an den Master weitergegeben und das Terminal danach geschlossen.</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Set the desired log &lt;level&gt;. Possible values are:
 - 0: log nothing
 - 1: critical errors only
 - 2: like 1 plus warnings
</source>
        <translation>Setzt das gewünschte Log-&lt;level&gt;. Mögliche Werte sind:
 - 0: Nichts loggen
 - 1: Nur Kritische Fehler loggen
 - 2: Wie 1 plus Warnungen loggen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source> - 3: like 2 plus information messages (default)
 - 4: verbose - log everything</source>
        <translation> - 3: Wie 2 plus Informationen loggen (Standardwert)
 - 4: Alles loggen</translation>
    </message>
    <message>
        <location line="+5"/>
        <source> - 3: like 2 plus information messages
 - 4: verbose - log everything (default)</source>
        <translation> - 3: Wie 2 plus Informationen loggen
 - 4: Alles loggen (Standardwert)</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Overwrites the default log &lt;path&gt;. The default path is platform and application specific. For this instance, it defaults to &quot;%1&quot;. NOTE: The application can override the value internally. Pass an empty string (--logpath &quot;&quot;) to disable logging to a file.</source>
        <translation>Überschreibt den standard Log-Pfad &lt;path&gt;. Dieser ist Plattform-abhängig und Anwendungs-spezifisch. Für diese Instanz ist es &quot;%1&quot;. HINWEIS: The Anwendung kann diesen Wert intern überschreiben. Übergeben sie einen leeren String (--logpath &quot;&quot;) um das Logging in eine Datei komplett zu deaktivieren.</translation>
    </message>
    <message>
        <location line="+9"/>
        <source>Sets the log &lt;level&gt; for terminal only messages. This does not include messages forwarded from the master. Log levels are the same as for the &lt;loglevel&gt; option.</source>
        <translation>Setzt das gewünschte Log-&lt;level&gt; für Terminal Ausgaben. Die schließt nicht die Ausgaben des Master-Prozesses ein. Die Level sind identisch zu denen der &lt;loglevel&gt; Option.</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Will prevent the master process from &quot;closing&quot; the console and other stuff that is done to daemonize the process. Can be useful for debugging purpose.</source>
        <translation>Sorgt dafür, dass der Master-Prozess seine Konsole nicht &quot;schließt&quot;, und auch andere &quot;Daemonisierungen&quot; nicht ausführt. Kann hilfreich zum debuggen sein.</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Skips the prompt and accepts where possible.</source>
        <translation>Überspringt Bestätigungen und akzeptiert sie, wo möglich.</translation>
    </message>
    <message>
        <location line="+70"/>
        <source>Failed to create local server with error:</source>
        <translation>Konnte lokalen Server nicht erstellen, mit Fehler:</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Unable to start master process. Failed with lock error:</source>
        <translation>Konnte Master-Prozess nicht starten. Sperrung fehlgeschlagen mit Fehler:</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Failed to start master process! No master lock was detected.</source>
        <translation>Konnte Master-Prozess nicht starten. Keine Sperrdatei des Masters erkannt.</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Start commands ignored because master is already running! The terminal will connect with an empty argument list!</source>
        <translation>Start-Kommando wird irgnoriert da der Master-Prozess bereits läuft. Das Terminal wird sich mit leerer Argument-Liste verbinden!</translation>
    </message>
    <message>
        <location line="+8"/>
        <source>Master is already running. Start arguments will be passed to it as is</source>
        <translation>Master-Prozess läuft bereits. Start-Argumente werden an diesen weitergegeben</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>
Failed to stop the running master process.
Do you want to restart it anyway? (y/N)</source>
        <translation>Master-Prozess konnte nicht gestoppt werden.
Trotzdem neustarten? (j/N)</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>y</source>
        <translation>j</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>Y</source>
        <translation>J</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>Master process is not running! Please launch it by using:</source>
        <translation>Master-Prozess läuft nicht! Bitte starten Sie diesen mit:</translation>
    </message>
    <message>
        <location line="+14"/>
        <source>Are you shure you want to purge the master lock and server?
Only do this if the master process is not running anymore, but the lock/server are not available (for example after a crash)
Purging while the master process is still running will crash it.
Press (Y) to purge, or (n) to cancel:</source>
        <translation>Sind Sie sicher, dass sie den Master-Server und Sperrdatei löschen möchten?
Sie sollten dies nur tun, wenn kein Master-Prozess mehr läuft, und sie aber trotzdem keine neuen starten können (z.B. nach einen Crash)
Eine Säuberung mit laufendem Master wird diesen zum Absturz bringen.
Geben sie (J) ein zum säubern, oder (n) zum abbrechen:</translation>
    </message>
    <message>
        <location line="+7"/>
        <source>n</source>
        <translation>n</translation>
    </message>
    <message>
        <location line="+0"/>
        <source>N</source>
        <translation>N</translation>
    </message>
    <message>
        <location line="+11"/>
        <source>Master lockfile successfully removed. It was locked by:</source>
        <translation>Master-Sperrdatei erfolgreich gelöscht. Gesperrt hat:</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove master lockfile. Lock data is:</source>
        <translation>Konnte Master-Sperrdatei nicht entfernen. Sie wird gesperrt von:</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>
 - PID: </source>
        <translation> - PID: </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>
 - Hostname: </source>
        <translation> - Hostname: </translation>
    </message>
    <message>
        <location line="+2"/>
        <source>
 - Appname: </source>
        <translation> - Anwendungsname: </translation>
    </message>
    <message>
        <location line="+4"/>
        <source>No lock file detected</source>
        <translation>Keine Sperrdatei gefunden</translation>
    </message>
    <message>
        <location line="+3"/>
        <source>Master server successfully removed</source>
        <translation>Master-Server erfolgreich entfernt</translation>
    </message>
    <message>
        <location line="+2"/>
        <source>Failed to remove master server</source>
        <translation>Master-Server konnte nicht entfernt werden</translation>
    </message>
    <message>
        <location line="+23"/>
        <source>Terminal with invalid commands discarded. Error:</source>
        <translation>Terminal mit ungültigen Parametern wird ignoriert. Fehler:</translation>
    </message>
</context>
<context>
    <name>QtBackgroundProcess::MasterConnecter</name>
    <message>
        <location filename="../masterconnecter.cpp" line="+69"/>
        <source>Connection to Master process failed with error:</source>
        <translation>Verbindung mit Master-Process fehlgeschlagen. Fehler:</translation>
    </message>
    <message>
        <location line="+47"/>
        <source>stdin was unexpectedly closed! The terminal will not be able to foreward input to the master anymore!</source>
        <translation>stdin wurde unerwartet geschlossen! Das Terminal ist nicht mehr in der Lage, Eingaben an den Master weiterzuleiten!</translation>
    </message>
</context>
<context>
    <name>QtBackgroundProcess::TerminalPrivate</name>
    <message>
        <location filename="../terminal_p.cpp" line="+67"/>
        <source>Terminal closed due to connection error while loading terminal status:</source>
        <translation>Terminal aufgrund von Fehler während der Verbindung geschlossen. Fehler:</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>Invalid Terminal status received. Data is corrupted. Terminal will be disconnected</source>
        <translation>Ungültigen Terminal-Status erhalten. Daten sind corrumpiert. Das Terminal wird nun geschlossen</translation>
    </message>
</context>
</TS>
