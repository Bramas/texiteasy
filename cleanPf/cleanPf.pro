TEMPLATE = app

TARGET = TexitEasy

QT += core network

SOURCES += \
    main.cpp \
    singleapp/qtsinglecoreapplication.cpp \
    singleapp/qtlockedfile_win.cpp \
    singleapp/qtlockedfile_unix.cpp \
    singleapp/qtlockedfile.cpp \
    singleapp/qtlocalpeer.cpp

RC_FILE = win.rc

HEADERS += \
    singleapp/qtsinglecoreapplication.h \
    singleapp/qtlockedfile.h \
    singleapp/qtlocalpeer.h
