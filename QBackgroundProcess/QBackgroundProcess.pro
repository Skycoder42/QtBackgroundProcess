#-------------------------------------------------
#
# Project created by QtCreator 2016-10-25T16:34:25
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = QBackgroundProcess
TEMPLATE = lib

DEFINES += QBACKGROUNDPROCESS_LIBRARY

SOURCES += \
    qtexception.cpp \
    app.cpp \
    app_p.cpp \
    terminal.cpp \
    terminal_p.cpp

HEADERS += QBackgroundProcess\
	qbackgroundprocess_global.h \
    qtexception.h \
    app.h \
	app_p.h \
    terminal.h \
    terminal_p.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
