TEMPLATE = app
TARGET = cloudlab-client

CONFIG += release

QT = core concurrent network gui widgets webkit webkitwidgets
BEQT = core network sql widgets networkwidgets
TSMP = core network widgets networkwidgets

isEmpty(BEQT_PREFIX) {
    mac|unix {
        BEQT_PREFIX=/usr/share/beqt
    } else:win32 {
        BEQT_PREFIX=$$(systemdrive)/PROGRA~1/BeQt
    }
}
include($${BEQT_PREFIX}/share/beqt/depend.pri)

isEmpty(TSMP_PREFIX) {
    mac|unix {
        TSMP_PREFIX=/usr/share/texsample
    } else:win32 {
        TSMP_PREFIX=$$(systemdrive)/PROGRA~1/TeXSample
    }
}
include($${TSMP_PREFIX}/share/texsample/depend.pri)

SOURCES += \
    application.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    application.h \
    mainwindow.h \
    settings.h

include(settingstab/settingstab.pri)
include(texsample/texsample.pri)

TRANSLATIONS += \
    ../translations/cloudlab-client_ru.ts

RC_FILE = win.rc

##############################################################################
################################ Generating translations #####################
##############################################################################

#Gets a file name
#Returns the given file name.
#On Windows slash characters will be replaced by backslashes
defineReplace(nativeFileName) {
    fileName=$${1}
    win32:fileName=$$replace(fileName, "/", "\\")
    return($${fileName})
}

translationsTs=$$files($${PWD}/../translations/*.ts)
for(fileName, translationsTs) {
    system(lrelease $$nativeFileName($${fileName}))
}

contains(CCLT_CONFIG, builtin_resources) {
    DEFINES += BUILTIN_RESOURCES
    RESOURCES += \
        cloudlab_client.qrc \
        cloudlab_client_doc.qrc \
        ../translations/cloudlab_client_translations.qrc
}

##############################################################################
################################ Installing ##################################
##############################################################################

!contains(CCLT_CONFIG, no_install) {

mac|unix {
    isEmpty(PREFIX):PREFIX=/usr
} else:win32 {
    isEmpty(PREFIX):PREFIX=$$(systemdrive)/PROGRA~1/CloudLab-Client
}

isEmpty(BINARY_INSTALLS_PATH):BINARY_INSTALLS_PATH=$${PREFIX}/bin
isEmpty(RESOURCES_INSTALLS_PATH):RESOURCES_INSTALLS_PATH=$${PREFIX}/share/cloudlab-client

##############################################################################
################################ Binaries ####################################
##############################################################################

target.path = $${BINARY_INSTALLS_PATH}
INSTALLS = target

##############################################################################
################################ Translations ################################
##############################################################################

!contains(CCLT_CONFIG, builtin_resources) {
    installsTranslations.files=$$files($${PWD}/../translations/*.qm)
    installsTranslations.path=$${RESOURCES_INSTALLS_PATH}/translations
    INSTALLS += installsTranslations
}

##############################################################################
################################ Other resources #############################
##############################################################################

!contains(CCLT_CONFIG, builtin_resources) {
    installsChangelog.files=$$files($${PWD}/changelog/*.txt)
    installsChangelog.path=$${RESOURCES_INSTALLS_PATH}/changelog
    INSTALLS += installsChangelog
    installsCopying.files=$$files($${PWD}/copying/*.txt)
    installsCopying.path=$${RESOURCES_INSTALLS_PATH}/copying
    INSTALLS += installsCopying
    installsDescription.files=$$files($${PWD}/description/*.txt)
    installsDescription.path=$${RESOURCES_INSTALLS_PATH}/description
    INSTALLS += installsDescription
    installsDocs.files=$$files($${PWD}/doc/*)
    installsDocs.path=$${RESOURCES_INSTALLS_PATH}/doc
    INSTALLS += installsDocs
    installsIcons.files=$$files($${PWD}/icons/*)
    installsIcons.path=$${RESOURCES_INSTALLS_PATH}/icons
    INSTALLS += installsIcons
    installsInfos.files=$$files($${PWD}/infos/*.beqt-info)
    installsInfos.path=$${RESOURCES_INSTALLS_PATH}/infos
    INSTALLS += installsInfos
    installsPixmaps.files=$$files($${PWD}/pixmaps/*)
    installsPixmaps.path=$${RESOURCES_INSTALLS_PATH}/pixmaps
    INSTALLS += installsPixmaps
}

} #end !contains(CCLT_CONFIG, no_install)
