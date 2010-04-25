TARGET = QtAndroid

include(../../qpluginbase.pri)

CONFIG -= dll
CONFIG += staticlib static
DESTDIR	 = $$QMAKE_LIBDIR_QT

CONFIG(android_official_ndk) : DEFINES += QT_USE_OFFICIAL_NDK
                        else : DEFINES += QT_USE_CUSTOM_NDK

CONFIG(android-4) : DEFINES += ANDROID_PLATFORM=4
CONFIG(android-5) : DEFINES += ANDROID_PLATFORM=5

SOURCES =   main.cpp \
            androidjnimain.cpp \
            qandroidplatformintegration.cpp \
            qandroidplatformscreen.cpp \
            qandroidplatformwindow.cpp \
            qandroidwindowsurface.cpp

HEADERS =   qandroidplatformintegration.h \
            qandroidplatformscreen.h \
            qandroidplatformwindow.h


contains(QT_CONFIG, opengl) {
    QT += opengl
    HEADERS += qandroidcontext.h
    SOURCES += qandroidcontext.cpp
}

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
