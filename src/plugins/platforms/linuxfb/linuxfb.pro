TARGET = qlinuxfbgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = main.cpp qgraphicssystem_linuxfb.cpp
HEADERS = qgraphicssystem_linuxfb.h

include(../fb_base/fb_base.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
