# -------------------------------------------------
# Project created by QtCreator 2009-11-04T21:13:47
# -------------------------------------------------
QT -= core \
    gui
TARGET = lua
TEMPLATE = lib
CONFIG += create_prl
VERSION = 5.1.4
DEFINES += LUA_LIBRARY
SOURCES += lzio.c \
    lvm.c \
    lundump.c \
    ltm.c \
    ltable.c \
    lstring.c \
    lstate.c \
    lparser.c \
    lopcodes.c \
    lobject.c \
    lmem.c \
    llex.c \
    lgc.c \
    lfunc.c \
    ldump.c \
    ldo.c \
    ldebug.c \
    lcode.c \
    lapi.c \
    ltablib.c \
    lstrlib.c \
    loslib.c \
    loadlib.c \
    lmathlib.c \
    liolib.c \
    linit.c \
    ldblib.c \
    lbaselib.c \
    lauxlib.c
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
    luaconf.h
