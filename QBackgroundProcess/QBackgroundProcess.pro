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

SOURCES += qbackgroundprocess.cpp \
    qbackgroundprocess_p.cpp \
    qtexception.cpp

HEADERS += qbackgroundprocess.h\
        qbackgroundprocess_global.h \
    qbackgroundprocess_p.h \
    qtexception.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
