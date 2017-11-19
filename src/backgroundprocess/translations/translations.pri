TRANSLATIONS += $$PWD/qtbackgroundprocess_de.ts \
	$$PWD/qtbackgroundprocess_template.ts

OTHER_FILES += $$PWD/qtbackgroundprocess_de.ts \
	$$PWD/qtbackgroundprocess_template.ts

qtPrepareTool(LRELEASE, lrelease)
LRELEASE += -nounfinished

releaseTarget.target = lrelease
releaseTarget.commands = $$LRELEASE "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += releaseTarget

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$PWD/qtbackgroundprocess_de.qm \
	$$PWD/qtbackgroundprocess_template.ts
trInstall.CONFIG += no_check_exist
trInstall.depends = releaseTarget
!win32-g++: INSTALLS += trInstall
