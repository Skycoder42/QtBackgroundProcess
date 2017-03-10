TARGET = QtBackgroundProcess

QT = core network

include(../3rdparty/QCtrlSignals/qctrlsignals.pri)

PUBLIC_HEADERS += \
	qbackgroundprocess_global.h \
	app.h \
	globalterminal.h \
	terminal.h

PRIVATE_HEADERS += \
	app_p.h \
	masterconnecter.h \
	qtexception.h \
	terminal_p.h

SOURCES += \
	app_p.cpp \
	app.cpp \
	globalterminal.cpp \
	masterconnecter.cpp \
	qtexception.cpp \
	terminal_p.cpp \
	terminal.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtBackgroundProcess"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	#LIBS += -lUser32
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
