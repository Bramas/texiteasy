#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T23:54:41
#
#-------------------------------------------------

QT       += core gui sql xml network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = texiteasy
TEMPLATE = app

APPLICATION_NAME = \\\"'TexitEasy'\\\"
VERSION = \\\"'0.9.0'\\\"
VERSION_HEX = 0x000900


DEFINES += LIB_DEPLOY
#DEFINES += PORTABLE_EXECUTABLE

DEFINES += "CURRENT_VERSION_HEX=$${VERSION_HEX}"
DEFINES += "CURRENT_VERSION=$${VERSION}"
DEFINES += "APPLICATION_NAME=$${APPLICATION_NAME}"


#DEFINES += DEBUG_DESTRUCTOR

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
    updatechecker.cpp \
    hunspell/utf_info.cxx \
    hunspell/suggestmgr.cxx \
    hunspell/replist.cxx \
    hunspell/phonet.cxx \
    hunspell/hunzip.cxx \
    hunspell/hunspell.cxx \
    hunspell/hashmgr.cxx \
    hunspell/filemgr.cxx \
    hunspell/dictmgr.cxx \
    hunspell/csutil.cxx \
    hunspell/affixmgr.cxx \
    hunspell/affentry.cxx \
    dialogaddlatexcommand.cpp \
    dialogmacros.cpp \
    macroengine.cpp \
    scriptengine.cpp

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
    dialogkeysequence.h \
    widgetstatusbar.h \
    widgettooltip.h \
    widgetproject.h \
    dialogabout.h \
    widgetfile.h \
    filemanager.h \
    widgettab.h \
    application.h \
    updatechecker.h \
    hunspell/w_char.hxx \
    hunspell/suggestmgr.hxx \
    hunspell/replist.hxx \
    hunspell/phonet.hxx \
    hunspell/langnum.hxx \
    hunspell/hunzip.hxx \
    hunspell/hunspell.hxx \
    hunspell/hunspell.h \
    hunspell/htypes.hxx \
    hunspell/hashmgr.hxx \
    hunspell/filemgr.hxx \
    hunspell/dictmgr.hxx \
    hunspell/csutil.hxx \
    hunspell/baseaffix.hxx \
    hunspell/atypes.hxx \
    hunspell/affixmgr.hxx \
    hunspell/affentry.hxx \
    dialogaddlatexcommand.h \
    dialogmacros.h \
    macroengine.h \
    scriptengine.h

FORMS    += mainwindow.ui \
    dialogwelcome.ui \
    dialogconfig.ui \
    widgetpdfviewer.ui \
    dialogclose.ui \
    widgetinsertcommand.ui \
    widgetfindreplace.ui \
    dialogkeysequence.ui \
    widgetstatusbar.ui \
    dialogabout.ui \
    dialogaddlatexcommand.ui \
    dialogmacros.ui

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
    commands.qrc \
    macros.qrc \
    icons.qrc

win32 {
    LIBS += -LC:/Qt/prerequisites-i686/lib -lpoppler-qt5
    LIBS += -LC:/Qt/prerequisites-i686/lib -lz

    INCLUDEPATH += C:/Qt/prerequisites-i686/include
    RC_FILE = win.rc

    DEFINES += OS_WINDOWS
    LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/win'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/win'\\\"

    #LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/win_portable'\\\"
    #TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/win_portable'\\\"

}
unix:!mac{
    LIBS += -lz -L/usr/local/lib -lpoppler-qt4
    DEFINES += OS_LINUX
    LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/linux'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/linux'\\\"
}
mac{
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lz -lpoppler-qt5
    ICON = texiteasy.icns
    DEFINES += OS_MAC
    LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/mac'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/mac'\\\"
    #CONFIG += x86 x86_64
}

DEFINES += "TEXITEASY_UPDATE_WEBSITE=$${TEXITEASY_UPDATE_WEBSITE}"
DEFINES += "LAST_VERSION_URL=$${LAST_VERSION_URL}"

OTHER_FILES += \
    hunspell/license.myspell \
    hunspell/license.hunspell
