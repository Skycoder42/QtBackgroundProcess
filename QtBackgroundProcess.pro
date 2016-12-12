TEMPLATE = subdirs

SUBDIRS += \
    QtBackgroundProcess \
    TestApp

TestApp.depends = QtBackgroundProcess
