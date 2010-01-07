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
debug:DESTDIR = ../build/debug
release:DESTDIR = ../build/release

DEFINES += LUAGL_LIBRARY
debug:LIBS += -L../build/debug \
        -llua5
release:LIBS += -L../build/release \
        -llua5

SOURCES += luagl.cpp \
           luaglu.cpp \
           luagl_util.cpp

HEADERS += luagl.h\
           luaglu.h \
           luagl_util.h
