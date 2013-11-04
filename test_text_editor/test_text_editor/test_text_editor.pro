#-------------------------------------------------
#
# Project created by QtCreator 2013-11-04T23:22:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_text_editor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    texteditor.cpp \
    syntaxhighlighter.cpp

HEADERS  += mainwindow.h \
    texteditor.h \
    syntaxhighlighter.h

FORMS    += mainwindow.ui
