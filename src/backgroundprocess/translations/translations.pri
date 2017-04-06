TRANSLATIONS += $$PWD/QtBackgroundProcess_de.ts \
	$$PWD/QtBackgroundProcess_template.ts

OTHER_FILES += $$PWD/QtBackgroundProcess_de.ts \
	$$PWD/QtBackgroundProcess_template.ts

updateTarget.target = lupdate
updateTarget.commands = lupdate -locations relative "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += updateTarget

updateObsTarget.target = lupdate_no_obs
updateObsTarget.commands = lupdate -no-obsolete -locations relative "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += updateObsTarget

releaseTarget.target = lrelease
releaseTarget.commands = lrelease -compress -nounfinished "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += releaseTarget

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$PWD/*.qm
INSTALLS += trInstall
