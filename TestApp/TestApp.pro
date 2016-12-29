QT += core
QT -= gui

CONFIG += c++11

TARGET = TestApp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    testapp.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtBackgroundProcess/release/ -lQtBackgroundProcess
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtBackgroundProcess/debug/ -lQtBackgroundProcess
else:mac {
	QMAKE_LFLAGS += -F$$OUT_PWD/../QtBackgroundProcess/
	QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../QtBackgroundProcess\'' #no bundle, thus only 1 up
	LIBS += -F$$OUT_PWD/../QtBackgroundProcess/ -framework QtBackgroundProcess
} else:unix: LIBS += -L$$OUT_PWD/../QtBackgroundProcess/ -lQtBackgroundProcess

INCLUDEPATH += $$PWD/../QtBackgroundProcess
DEPENDPATH += $$PWD/../QtBackgroundProcess

HEADERS += \
    testapp.h

DISTFILES += \
    testing.md
