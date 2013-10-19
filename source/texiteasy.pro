#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T23:54:41
#
#-------------------------------------------------

QT       += core gui sql xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = texiteasy
TEMPLATE = app

APPLICATION_NAME = \\\"'TexitEasy'\\\"
DEFINES += "APPLICATION_NAME=$${APPLICATION_NAME}"
TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com'\\\"
DEFINES += "TEXITEASY_UPDATE_WEBSITE=$${TEXITEASY_UPDATE_WEBSITE}"
VERSION = \\\"'0.5.4'\\\"
DEFINES += "CURRENT_VERSION=$${VERSION}"

#DEFINES += DEBUG_DESTRUCTOR

# config_revision is used to make so task for migration
# we increment it each time we need to make a task for those who update the soft
# see configManager::checkRevision() for more information
DEFINES += "CURRENT_CONFIG_REVISION=4"

SOURCES += main.cpp\
        mainwindow.cpp \
    widgetlinenumber.cpp \
    widgettextedit.cpp \
    syntaxhighlighter.cpp \
    widgetscroller.cpp \
    blockdata.cpp \
    filestructure.cpp \
    file.cpp \
    builder.cpp \
    dialogwelcome.cpp \
    dialogconfig.cpp \
    configmanager.cpp \
    viewer.cpp \
    widgetpdfdocument.cpp \
    synctex_parser.c \
    synctex_parser_utils.c \
    widgetpdfviewer.cpp \
    completionengine.cpp \
    widgetconsole.cpp \
    dialogclose.cpp \
    widgetinsertcommand.cpp \
    widgetfindreplace.cpp \
    stylehelper.cpp \
    minisplitter.cpp \
    widgetsimpleoutput.cpp \
    dialogkeysequence.cpp \
    widgetstatusbar.cpp \
    widgettooltip.cpp \
    widgetproject.cpp \
    dialogabout.cpp \
    widgetfile.cpp \
    filemanager.cpp \
    widgettab.cpp \
    application.cpp \
    updatechecker.cpp

HEADERS  += mainwindow.h \
    widgetlinenumber.h \
    widgettextedit.h \
    syntaxhighlighter.h \
    widgetscroller.h \
    blockdata.h \
    filestructure.h \
    file.h \
    builder.h \
    dialogwelcome.h \
    dialogconfig.h \
    configmanager.h \
    viewer.h \
    widgetpdfdocument.h \
    synctex_parser.h \
    synctex_parser_utils.h \
    synctex_parser_local.h \
    widgetpdfviewer.h \
    completionengine.h \
    widgetconsole.h \
    dialogclose.h \
    widgetinsertcommand.h \
    widgetfindreplace.h \
    stylehelper.h \
    minisplitter.h \
    widgetsimpleoutput.h \
    zlib/zlib.h \
    dialogkeysequence.h \
    widgetstatusbar.h \
    widgettooltip.h \
    widgetproject.h \
    dialogabout.h \
    widgetfile.h \
    filemanager.h \
    widgettab.h \
    application.h \
    updatechecker.h

FORMS    += mainwindow.ui \
    dialogwelcome.ui \
    dialogconfig.ui \
    widgetpdfviewer.ui \
    dialogclose.ui \
    widgetinsertcommand.ui \
    widgetfindreplace.ui \
    dialogkeysequence.ui \
    widgetstatusbar.ui \
    dialogabout.ui

TRANSLATIONS += translations/texiteasy_ar.ts \
                translations/texiteasy_fr.ts \
                translations/texiteasy_en.ts \
                translations/texiteasy_es.ts \
                translations/texiteasy_ja.ts \
                translations/texiteasy_vi.ts \
                translations/texiteasy_zh.ts

CODECFORTR = UTF-8

isEmpty( PREFIX ) {
    PREFIX=/usr
}
DEFINES += PREFIX=\\\"$${PREFIX}\\\"
target.path = $${PREFIX}/bin

INSTALLS = target


RESOURCES += \
    data.qrc \
    completion.qrc \
    commands.qrc

win32 {
    LIBS += -LC:/dev/Tools/poppler/lib -lpoppler-qt4

    RC_FILE = win.rc

    DEFINES += OS_WINDOWS
}
unix:!mac{
    LIBS += -lz -L/usr/local/lib -lpoppler-qt4
    DEFINES += OS_LINUX
}
mac{
    LIBS += -lz -L/usr/local/lib -lpoppler-qt4
    ICON = texiteasy.icns
    DEFINES += OS_MAC
    #CONFIG += x86 x86_64
}
