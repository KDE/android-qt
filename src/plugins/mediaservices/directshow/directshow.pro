TARGET = dsengine
include(../../qpluginbase.pri)

QT += multimedia

HEADERS += dsserviceplugin.h
SOURCES += dsserviceplugin.cpp

include(mediaplayer/mediaplayer.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mediaservices
target.path = $$[QT_INSTALL_PLUGINS]/plugins/mediaservices
INSTALLS += target

