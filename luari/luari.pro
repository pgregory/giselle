# -------------------------------------------------
# Project created by QtCreator 2009-12-08T22:02:47
# -------------------------------------------------
QT -= core \
    gui
TARGET = luari
TEMPLATE = lib
DEFINES += LUARI_LIBRARY
INCLUDEPATH = ../lua

DEFINES += LUAGL_LIBRARY
win32 {
    debug:LIBS += -L../lua/debug \
        -llua \
        -L/home/pgregory/aqsis/trunk/install/lib \
        -laqsis_ri2rib
    release:LIBS += -L../lua/release \
        -llua \
        -L/home/pgregory/aqsis/trunk/install/lib \
        -laqsis_ri2rib
}
!win32:LIBS += -L../lua \
    -llua \
    -L/home/pgregory/aqsis/trunk/install/lib \
    -laqsis_ri2rib

QMAKE_CFLAGS += -I/home/pgregory/aqsis/trunk/install/include/aqsis/ri
QMAKE_CFLAGS += -I/home/pgregory/aqsis/trunk/install/include
QMAKE_CXXFLAGS += -I/home/pgregory/aqsis/trunk/install/include/aqsis/ri
QMAKE_CXXFLAGS += -I/home/pgregory/aqsis/trunk/install/include

SWIG_FLAGS = -I/home/pgregory/aqsis/trunk/install/include/aqsis/ri

SWIG_FILES = ri.i

include (swig.pri)
