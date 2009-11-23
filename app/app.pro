# -------------------------------------------------
# Project created by QtCreator 2009-11-03T20:48:56
# -------------------------------------------------
QT += opengl
TARGET = app
TEMPLATE = app
INCLUDEPATH = ../lua
INCLUDEPATH += ../luagl
win32 { 
    debug:LIBS += -L../lua/debug \
        -llua \
        -L../luagl/debug \
        -lluagl
    release:LIBS += -L../lua/release \
        -llua \
        -L../luagl/release \
        -lluagl
}
!win32:LIBS += -L../lua \
    -llua \
    -L../luagl \
    -lluagl
SOURCES += main.cpp \
    mainwindow.cpp \
    avarlistmodel.cpp \
    scenetreemodel.cpp \
    scenetreeitem.cpp \
    luahighlighter.cpp \
    avarlistitem.cpp \
    glwindow.cpp \
    openglwidget.cpp \
    avartableview.cpp
HEADERS += mainwindow.h \
    avarlistmodel.h \
    scenetreemodel.h \
    scenetreeitem.h \
    luahighlighter.h \
    avarlistitem.h \
    glwindow.h \
    openglwidget.h \
    luaerror.h \
    avartableview.h
FORMS += mainwindow.ui \
    glwindow.ui
