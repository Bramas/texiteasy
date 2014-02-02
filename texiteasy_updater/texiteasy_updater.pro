#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T12:03:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = texiteasy_updat
TEMPLATE = app


DEFINES += LIB_DEPLOY

SOURCES += main.cpp\
        mainwindow.cpp \
    download.cpp

HEADERS  += mainwindow.h \
    download.h

FORMS    += mainwindow.ui



win32 {

    DEFINES += OS_WINDOWS
    LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/win'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/win'\\\"

    #LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/win_portable'\\\"
    #TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/win_portable'\\\"

}
unix:!mac{
    DEFINES += OS_LINUX
    LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/linux'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/linux'\\\"
}
mac{
    DEFINES += OS_MAC
    LAST_VERSION_URL = \\\"'http://texiteasy.com/posts/latest_version/mac'\\\"
    TEXITEASY_UPDATE_WEBSITE = \\\"'http://texiteasy.com/posts/download_latest/mac'\\\"
}
DEFINES += "TEXITEASY_UPDATE_WEBSITE=$${TEXITEASY_UPDATE_WEBSITE}"
DEFINES += "LAST_VERSION_URL=$${LAST_VERSION_URL}"

