#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T23:54:41
#
#-------------------------------------------------


CONFIG += qt

equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 6) {
QT += core gui widgets sql xml network concurrent core-private script
equals(INTERNALBROWSER,yes){
QT += webenginewidgets
}  
} else {
message("Qt>=5.7 is required.")
}


TARGET = texiteasy
TEMPLATE = app

APPLICATION_NAME = \\\"'TexitEasy'\\\"

VERSION = \\\"'0.26.4'\\\"
VERSION_HEX = 0x002604

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x


#DEFINES += PORTABLE_EXECUTABLE

DEFINES += "CURRENT_VERSION_HEX=$${VERSION_HEX}"
DEFINES += "CURRENT_VERSION=$${VERSION}"
DEFINES += "APPLICATION_NAME=$${APPLICATION_NAME}"


#DEFINES += DEBUG_DESTRUCTOR

SOURCES += main.cpp\
    githelper.cpp \
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
    scriptengine.cpp \
    singleapp/qtsinglecoreapplication.cpp \
    singleapp/qtsingleapplication.cpp \
    singleapp/qtlockedfile.cpp \
    singleapp/qtlockedfile_win.cpp \
    singleapp/qtlockedfile_unix.cpp \
    singleapp/qtlocalpeer.cpp \
    textdocumentlayout.cpp \
    dialogsendfeedback.cpp \
    tools.cpp \
    grammarchecker.cpp \
    textaction.cpp \
    pdfsynchronizer.cpp \
    textdocument.cpp \
    svnhelper.cpp \
    githelper.cpp \
    latexoutputfilter.cpp \
    taskpane/taskmodel.cpp \
    taskpane/taskwindow.cpp \
    taskpane/task.cpp \
    qt4panecallback.cpp \
    helpwidget.cpp \
    paneautocorrector.cpp

HEADERS  += mainwindow.h \
    githelper.h \
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
    scriptengine.h \
    singleapp/qtsinglecoreapplication.h \
    singleapp/qtsingleapplication.h \
    singleapp/qtlockedfile.h \
    singleapp/qtlocalpeer.h \
    textdocumentlayout.h \
    dialogsendfeedback.h \
    tools.h \
    grammarchecker.h \
    textaction.h \
    pdfsynchronizer.h \
    textdocument.h \
    svnhelper.h \
    githelper.h \
    latexoutputfilter.h \
    taskpane/taskmodel.h \
    taskpane/taskwindow.h \
    taskpane/task.h \
    taskpane/icontext.h \
    iplugin.h \
    ipane.h \
    qt4panecallback.h \
    helpwidget.h \
    paneautocorrector.h

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
    dialogmacros.ui \
    dialogsendfeedback.ui \
    helpwidget.ui


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

    SOURCES +=  dialogtexdownloadassistant.cpp \
                dialogdownloadupdate.cpp
    HEADERS +=  dialogdownloadupdate.h \
                dialogtexdownloadassistant.h
    FORMS +=    dialogdownloadupdate.ui \
                dialogtexdownloadassistant.ui


    LIBS += -LC:/dev/poppler/lib -lpoppler-qt5
    LIBS += -LC:/dev/zlib/lib -lz

    INCLUDEPATH += "C:\dev\poppler\include"
    RC_FILE = win.rc

    DEFINES += OS_WINDOWS
    LAST_VERSION_URL = \\\"'http://texiteasy.com/downloads/latest_version/win'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/downloads/latest/win'\\\"
    TEXITEASY_UPDATE_FILE_URL = \\\"'http://texiteasy.com/downloads/update_files/win'\\\"
    LATEX_URL = \\\"'http://texiteasy.com/links/latex/win'\\\"

    #LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/win_portable'\\\"
    #TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/win_portable'\\\"

}



unix:!mac{

    LIBS += -lz
    lessThan(QT_MAJOR_VERSION, 5) {
        LIBS += -L/usr/local/lib -lpoppler-qt4
    } else {
        LIBS += -L/usr/local/lib -lpoppler-qt5
    }
    DEFINES += OS_LINUX
    LAST_VERSION_URL = \\\"'http://texiteasy.com/downloads/latest_version/linux'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/downloads/latest/linux'\\\"
    LATEX_URL = \\\"'http://texiteasy.com/links/latex/linux'\\\"


    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj

    isEmpty( DESKTOPDIR ) {
        DESKTOPDIR=/usr/share/applications
    }
    isEmpty( ICONDIR ) {
        ICONDIR=/usr/share/pixmaps
    }

    desktop.path = $${DESKTOPDIR}
    desktop.files = utilities/texiteasy.desktop
    INSTALLS += desktop

    icon.path = $${ICONDIR}
    icon.files = utilities/texiteasy.png
    INSTALLS += icon


    utilities.path = $${PREFIX}/share/texiteasy/dictionaries
    utilities.files = data/dictionaries/*
    INSTALLS += utilities

}
mac{
    QMAKE_MAC_SDK = macosx #macosx10.12

    # including the whole folder can make errors so now, on mac, just include the needed library include folder (for the lib folder too)
    INCLUDEPATH += /usr/local/opt/poppler/include
    LIBS += -lz -L/usr/local/opt/poppler/lib  -lpoppler-qt5
    ICON = texiteasy.icns
    DEFINES += OS_MAC
    LAST_VERSION_URL = \\\"'http://texiteasy.com/downloads/latest_version/mac'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/downloads/latest/mac'\\\"
    LATEX_URL = \\\"'http://texiteasy.com/links/latex/mac'\\\"

}

DEFINES += "TEXITEASY_UPDATE_WEBSITE=$${TEXITEASY_UPDATE_WEBSITE}"
DEFINES += "TEXITEASY_UPDATE_FILE_URL=$${TEXITEASY_UPDATE_FILE_URL}"
DEFINES += "LAST_VERSION_URL=$${LAST_VERSION_URL}"
DEFINES += "LATEX_URL=$${LATEX_URL}"

OTHER_FILES += \
    hunspell/license.myspell \
    hunspell/license.hunspell
