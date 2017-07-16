TRANSLATIONS += $$PWD/qtbackgroundprocess_de.ts \
	$$PWD/qtbackgroundprocess_template.ts

OTHER_FILES += $$PWD/qtbackgroundprocess_de.ts \
	$$PWD/qtbackgroundprocess_template.ts

releaseTarget.target = lrelease
releaseTarget.commands = lrelease -compress -nounfinished "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += releaseTarget

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$PWD/qtbackgroundprocess_de.qm \
	$$PWD/qtbackgroundprocess_template.ts
trInstall.CONFIG += no_check_exist
trInstall.depends = releaseTarget
INSTALLS += trInstall
