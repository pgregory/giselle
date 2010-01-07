# -------------------------------------------------
# Project created by QtCreator 2009-11-03T20:48:56
# -------------------------------------------------
QT += opengl
TARGET = app
TEMPLATE = app
INCLUDEPATH = ../lua
INCLUDEPATH += ../luagl
debug:DESTDIR = ../build/debug
release:DESTDIR = ../build/release
debug:LIBS += -L../build/debug \
    -llua5 \
    -lluagl \
    -lluari
release:LIBS += -L../build/release \
    -llua5 \
    -lluagl \
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
