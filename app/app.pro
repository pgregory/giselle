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
        -lluagl \
        -L../luari/debug \
        -lluari
    release:LIBS += -L../lua/release \
        -llua \
        -L../luagl/release \
        -lluagl \
        -L../luari/release \
        -lluari
}
!win32:LIBS += -L../lua \
    -llua \
    -L../luagl \
    -lluagl \
    -L../luari \
    -lluari
SOURCES += main.cpp \
    mainwindow.cpp \
    avarlistmodel.cpp \
    scenetreemodel.cpp \
    scenetreeitem.cpp \
    luahighlighter.cpp \
    avarlistitem.cpp \
    openglwidget.cpp \
    avartableview.cpp \
    draggablespinbox.cpp \
    luaeditor.cpp \
    datamanager.cpp
HEADERS += mainwindow.h \
    avarlistmodel.h \
    scenetreemodel.h \
    scenetreeitem.h \
    luahighlighter.h \
    avarlistitem.h \
    openglwidget.h \
    luaerror.h \
    avartableview.h \
    draggablespinbox.h \
    luaeditor.h \
    datamanager.h
FORMS += mainwindow.ui
