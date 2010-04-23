TARGET = QtAndroid

include(../../qpluginbase.pri)

CONFIG -= dll
CONFIG += staticlib static
DESTDIR	 = $$QMAKE_LIBDIR_QT

CONFIG(android_official_ndk) : DEFINES += QT_USE_OFFICIAL_NDK
                        else : DEFINES += QT_USE_CUSTOM_NDK

CONFIG(android-4) : DEFINES += ANDROID_PLATFORM=4
CONFIG(android-5) : DEFINES += ANDROID_PLATFORM=5

QT += opengl

SOURCES = main.cpp \
    androidjnimain.cpp \
    qandroidplatformintegration.cpp

HEADERS = qandroidplatformintegration.h

include(../fb_base/fb_base.pri)

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
