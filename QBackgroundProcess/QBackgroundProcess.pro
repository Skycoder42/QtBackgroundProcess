#-------------------------------------------------
#
# Project created by QtCreator 2016-10-25T16:34:25
#
#-------------------------------------------------

TEMPLATE = lib
QT       += network
QT       -= gui

TARGET = QBackgroundProcess
VERSION = 1.1.0

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QBackgroundProcess"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	CONFIG += skip_target_version_ext
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "com.Skycoder42."
	QMAKE_FRAMEWORK_BUNDLE_NAME = "QBackgroundProcess"

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
	QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../Frameworks\''
} else:unix {
	QMAKE_LFLAGS += '-Wl,-rpath,\'.\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../lib\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/lib\''
}

DEFINES += QBACKGROUNDPROCESS_LIBRARY

SOURCES += \
    qtexception.cpp \
    app.cpp \
    app_p.cpp \
    terminal.cpp \
    terminal_p.cpp \
    masterconnecter.cpp \
    globalterminal.cpp

HEADERS += QBackgroundProcess\
	qbackgroundprocess_global.h \
    qtexception.h \
    app.h \
	app_p.h \
    terminal.h \
    terminal_p.h \
    masterconnecter.h \
    globalterminal.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
