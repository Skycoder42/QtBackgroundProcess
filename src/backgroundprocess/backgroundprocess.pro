TARGET = QtBackgroundProcess

QT = core network

HEADERS += \
	app.h \
	globalterminal.h \
	terminal.h \
	app_p.h \
	masterconnecter_p.h \
	terminal_p.h \
	qtbackgroundprocess_global.h

SOURCES += \
	app_p.cpp \
	app.cpp \
	globalterminal.cpp \
	masterconnecter.cpp \
	terminal_p.cpp \
	terminal.cpp

include(./translations/translations.pri)

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtBackgroundProcess"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	LIBS += -lUser32
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

DISTFILES += \
	systemd.service

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
