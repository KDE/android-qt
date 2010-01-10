TARGET = QtAndroid

include(../../qpluginbase.pri)

CONFIG -= dll
CONFIG += staticlib static
DESTDIR	 = $$QMAKE_LIBDIR_QT

CONFIG(android_official_ndk) : DEFINES += QT_USE_OFFICIAL_NDK
                        else : DEFINES += QT_USE_CUSTOM_NDK

QT += opengl

SOURCES = main.cpp \
    androidjnimain.cpp \
    qgraphicssystem_android.cpp \
    qwindowsurface_android.cpp \
    android_egl.cpp \
    qandroidinput.cpp

HEADERS = qgraphicssystem_android.h \
    qwindowsurface_android.h \
    qandroidinput.h

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
