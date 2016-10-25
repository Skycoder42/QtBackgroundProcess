TEMPLATE = subdirs

SUBDIRS += \
    QBackgroundProcess \
    TestApp

TestApp.depends = QBackgroundProcess
