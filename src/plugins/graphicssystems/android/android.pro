TARGET = android
include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

QT += opengl

SOURCES = main.cpp \
    android_app.cpp \
    qgraphicssystem_android.cpp \
    qwindowsurface_android.cpp \
    android_egl.cpp \
    qandroidinput.cpp

HEADERS = android_app.h \
    qgraphicssystem_android.h \
    qwindowsurface_android.h \
    qandroidinput.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
