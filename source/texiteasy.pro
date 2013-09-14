#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T23:54:41
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = texiteasy
TEMPLATE = app

APPLICATION_NAME = \\\"'TexitEasy'\\\"
DEFINES += "APPLICATION_NAME=$${APPLICATION_NAME}"
VERSION = \\\"'0.2.4'\\\"
DEFINES += "CURRENT_VERSION=$${VERSION}"

# config_revision is used to make so task for migration
# we increment it each time we need to make a task for those who update the soft
# see configManager::checkRevision() for more information
DEFINES += "CURRENT_CONFIG_REVISION=2"

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
    widgetstatusbar.cpp

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
    widgetstatusbar.h

FORMS    += mainwindow.ui \
    dialogwelcome.ui \
    dialogconfig.ui \
    widgetpdfviewer.ui \
    dialogclose.ui \
    widgetinsertcommand.ui \
    widgetfindreplace.ui \
    dialogkeysequence.ui \
    widgetstatusbar.ui

#LIBS         += -LG:\poppler -lpoppler-qt4
#LIBS         += -LG:\poppler\cpp\bin -lpoppler-cpp
#LIBS         += -LG:\poppler\cpp\bin -lpoppler-cpp
#LIBS         += G:\poppler\bin\poppler-cpp.dll
#LIBS         += G:\poppler\bin\libpng14-14.dll

isEmpty( PREFIX ) {
    PREFIX=/usr
}
DEFINES += PREFIX=\\\"$${PREFIX}\\\"
target.path = $${PREFIX}/bin

INSTALLS = target


RESOURCES += \
    data.qrc \
    completion.qrc

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
    #CONFIG += x86 x86_64
}
