# -------------------------------------------------
# Project created by QtCreator 2009-11-03T20:48:56
# -------------------------------------------------
QT += opengl
TARGET = app
TEMPLATE = app
INCLUDEPATH = ../lua
INCLUDEPATH += /opt/boost_1_37_0/include/boost-1_37
win32 {
    debug:LIBS += -L../lua/debug \
        -llua
    release:LIBS += -L../lua/release \
        -llua
}
!win32:LIBS += -L../lua \
    -llua
SOURCES += main.cpp \
    mainwindow.cpp \
    avarlistmodel.cpp \
    scenetreemodel.cpp \
    scenetreeitem.cpp \
    luahighlighter.cpp
HEADERS += mainwindow.h \
    avarlistmodel.h \
    scenetreemodel.h \
    scenetreeitem.h \
    luahighlighter.h
FORMS += mainwindow.ui
