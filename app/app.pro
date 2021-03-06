# -------------------------------------------------
# Project created by QtCreator 2009-11-03T20:48:56
# -------------------------------------------------
QT += opengl
CONFIG -= app_bundle
TARGET = app
TEMPLATE = app
INCLUDEPATH = ../lua
INCLUDEPATH += ../luagl
INCLUDEPATH += ../luagl_extras
debug:DESTDIR = ../build/debug
release:DESTDIR = ../build/release
debug:LIBS += -L../build/debug \
    -llua \
    -lluagl \
    -lluagl_extras
release:LIBS += -L../build/release \
    -llua \
    -lluagl \
    -lluagl_extras
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

DISTFILES += \
    TA/Avar.lua \
    TA/Camera.lua \
    TA/complex.lua \
    TA/CoordinateFrame.lua \
    TA/ExposureSheet.lua \
    TA/GLRenderer.lua \
    TA/init.lua \
    TA/matrix.lua \
    TA/Model.lua \
    TA/Object.lua \
    TA/Renderable.lua \
    TA/Renderer.lua \
    TA/RenderMan.lua \
    TA/Serialize.lua \
    TA/tablesave.lua \
    TA/World.lua

copydata.commands = $(COPY_DIR) $$PWD/TA $$DESTDIR/TA
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
