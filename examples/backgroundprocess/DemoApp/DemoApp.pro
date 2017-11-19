TEMPLATE = app

QT += core backgroundprocess network
QT -= gui
CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = DemoApp

SOURCES += \
	testapp.cpp

HEADERS += \
	testapp.h

DISTFILES += \
	README.md

target.path = $$[QT_INSTALL_EXAMPLES]/backgroundprocess/DemoApp
INSTALLS += target

#not found by linker?
unix:!mac {
	LIBS += -L$$OUT_PWD/../../../lib #required to make this the first place to search
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

#add lib dir and qt install libs to rpath
mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\'' '-Wl,-rpath,\'$$[QT_INSTALL_LIBS]\''
