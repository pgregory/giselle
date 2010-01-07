# -------------------------------------------------
# Project created by QtCreator 2009-11-04T21:13:47
# -------------------------------------------------
QT -= core \
    gui
TARGET = lua
TEMPLATE = lib
debug:DESTDIR = ../build/debug
release:DESTDIR = ../build/release
VERSION = 5.1.4
DEFINES += LUA_LIBRARY
SOURCES += lzio.cpp \
    lvm.cpp \
    lundump.cpp \
    ltm.cpp \
    ltable.cpp \
    lstring.cpp \
    lstate.cpp \
    lparser.cpp \
    lopcodes.cpp \
    lobject.cpp \
    lmem.cpp \
    llex.cpp \
    lgc.cpp \
    lfunc.cpp \
    ldump.cpp \
    ldo.cpp \
    ldebug.cpp \
    lcode.cpp \
    lapi.cpp \
    ltablib.cpp \
    lstrlib.cpp \
    loslib.cpp \
    loadlib.cpp \
    lmathlib.cpp \
    liolib.cpp \
    linit.cpp \
    ldblib.cpp \
    lbaselib.cpp \
    lauxlib.cpp
HEADERS += lzio.h \
    lvm.h \
    lundump.h \
    ltm.h \
    ltable.h \
    lstring.h \
    lstate.h \
    lparser.h \
    lopcodes.h \
    lobject.h \
    lmem.h \
    llex.h \
    lgc.h \
    lfunc.h \
    ldo.h \
    ldebug.h \
    lcode.h \
    lapi.h \
    lauxlib.h \
    luaconf.h \
    lua.h
