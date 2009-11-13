#-------------------------------------------------
#
# Project created by QtCreator 2009-11-13T21:34:36
#
#-------------------------------------------------

QT       -= core gui

TARGET = luagl
TEMPLATE = lib
INCLUDEPATH = ../lua

DEFINES += LUAGL_LIBRARY

SOURCES += luagl.c \
           luaglu.c \
           luagl_util.c

HEADERS += luagl.h\
           luaglu.h \
           luagl_util.h
