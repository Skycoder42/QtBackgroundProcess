#-------------------------------------------------
#
# Project created by QtCreator 2017-08-21T11:21:46
#
#-------------------------------------------------

QT       += testlib backgroundprocess
QT       -= gui

TARGET = tst_master
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
		tst_master.cpp \
	processhelper.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += OUTDIR=\\\"$$OUT_PWD/\\\"
CONFIG(release, debug|release): DEFINES += RMODE=\\\"release\\\"
CONFIG(debug, debug|release): DEFINES += RMODE=\\\"debug\\\"

HEADERS += \
	processhelper.h
