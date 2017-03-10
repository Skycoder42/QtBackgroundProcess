TEMPLATE = subdirs

SUBDIRS = backgroundprocess

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget