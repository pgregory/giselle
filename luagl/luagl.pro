#-------------------------------------------------
#
# Project created by QtCreator 2009-11-13T21:34:36
#
#-------------------------------------------------

QT       -= core gui
QT       += opengl

TARGET = luagl
TEMPLATE = lib
INCLUDEPATH = ../lua

DEFINES += LUAGL_LIBRARY
win32 {
    debug:LIBS += -L../lua/debug \
        -llua
    release:LIBS += -L../lua/release \
        -llua
}
!win32:LIBS += -L../lua \
    -llua
SOURCES += luagl.c \
           luaglu.c \
           luagl_util.c

HEADERS += luagl.h\
           luaglu.h \
           luagl_util.h
