# -------------------------------------------------
# Project created by QtCreator 2009-12-08T22:02:47
# -------------------------------------------------
QT -= core \
    gui
TARGET = luari
TEMPLATE = lib
DEFINES += LUARI_LIBRARY
INCLUDEPATH = ../lua

debug:DESTDIR = ../build/debug
release:DESTDIR = ../build/release

DEFINES += LUAGL_LIBRARY
debug:LIBS += -L../build/debug \
    -llua \
    -L${AQSISHOME}/lib \
    -laqsis_ri2rib
release:LIBS += -L../build/release \
    -llua \
    -L${AQSISHOME}/lib \
    -laqsis_ri2rib

QMAKE_CFLAGS += -I${AQSISHOME}/include/aqsis/ri
QMAKE_CFLAGS += -I${AQSISHOME}/include
QMAKE_CXXFLAGS += -I${AQSISHOME}/include/aqsis/ri
QMAKE_CXXFLAGS += -I${AQSISHOME}/include

SWIG_FLAGS = -I${AQSISHOME}/include/aqsis/ri

SWIG_FILES = ri.i

include (swig.pri)

OTHER_FILES += $$SWIG_FILES
