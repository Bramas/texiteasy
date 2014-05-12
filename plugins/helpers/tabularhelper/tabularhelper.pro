#-------------------------------------------------
#
# Project created by QtCreator 2014-05-11T15:33:18
#
#-------------------------------------------------

TARGET = tabularhelper
TEMPLATE = lib

DEFINES += TABULARHELPER_LIBRARY

SOURCES += tabularhelper.cpp

HEADERS += tabularhelper.h
#HEADERS += tabularhelper_global.h

TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
TARGET          = $$qtLibraryTarget(tabularhelper)
DESTDIR         = ..
