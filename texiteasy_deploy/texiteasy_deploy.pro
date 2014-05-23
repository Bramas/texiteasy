#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T20:58:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = texiteasy_deploy
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    miniz.c

HEADERS  += mainwindow.h \
    timer.h

FORMS    += mainwindow.ui

LIBS += -LC:/Qt/prerequisites-i686/lib -lz
INCLUDEPATH += C:/Qt/prerequisites-i686/include

