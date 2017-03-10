TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += backgroundprocess

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
