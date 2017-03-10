TEMPLATE = app

QT += core backgroundprocess
QT -= gui
CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = DemoApp

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
	testapp.cpp

HEADERS += \
	testapp.h

DISTFILES += \
	testing.md

target.path = $$[QT_INSTALL_EXAMPLES]/backgroundprocess/DemoApp
INSTALLS += target

#not found by linker?
unix:!mac {
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}
