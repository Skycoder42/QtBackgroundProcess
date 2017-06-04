%modules = (
    "QtBackgroundProcess" => "$basedir/src/backgroundprocess",
);

%classnames = (
	"app.h" => "NotAllowedInRunningStateException,App",
	"globalterminal.h" => "GlobalTerminal",
	"terminal.h" => "Terminal",
);
