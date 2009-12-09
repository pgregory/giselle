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
        -L${AQSISHOME}/lib \
        -laqsis_ri2rib
    release:LIBS += -L../lua/release \
        -llua \
        -L${AQSISHOME}/lib \
        -laqsis_ri2rib
}
!win32:LIBS += -L../lua \
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
