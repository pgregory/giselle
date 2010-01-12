#-------------------------------------------------
#
# Project created by QtCreator 2009-11-13T21:34:36
#
#-------------------------------------------------

QT       -= core
QT       += opengl

TARGET = luagl_extras
TEMPLATE = lib
INCLUDEPATH = ../lua
INCLUDEPATH += ../luagl
debug:DESTDIR = ../build/debug
release:DESTDIR = ../build/release

DEFINES += LUAGL_EXTRAS_LIBRARY
debug:LIBS += -L../build/debug \
        -llua \
        -lluagl
release:LIBS += -L../build/release \
        -llua \
        -lluagl

SOURCES += luagl_extras.cpp 

HEADERS += luagl_extras.h
