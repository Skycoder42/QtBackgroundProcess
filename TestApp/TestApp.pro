QT += core
QT -= gui

CONFIG += c++11

TARGET = TestApp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    testapp.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QBackgroundProcess/release/ -lQBackgroundProcess
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QBackgroundProcess/debug/ -lQBackgroundProcess
else:unix: LIBS += -L$$OUT_PWD/../QBackgroundProcess/ -lQBackgroundProcess

INCLUDEPATH += $$PWD/../QBackgroundProcess
DEPENDPATH += $$PWD/../QBackgroundProcess

HEADERS += \
    testapp.h
