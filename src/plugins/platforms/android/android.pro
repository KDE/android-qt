TARGET = QtAndroid

include(../../qpluginbase.pri)

CONFIG -= dll
CONFIG += staticlib static
DESTDIR	 = $$QMAKE_LIBDIR_QT

CONFIG(android-4) : DEFINES += ANDROID_PLATFORM=4
CONFIG(android-5) : DEFINES += ANDROID_PLATFORM=5
CONFIG(android-8) : DEFINES += ANDROID_PLATFORM=8

SOURCES =   main.cpp \
            androidjnimain.cpp \
            qandroidplatformintegration.cpp \
            qandroidplatformscreen.cpp \
            qandroidplatformwindow.cpp \
            qandroidwindowsurface.cpp

HEADERS =   androidjnimain.h \
            qandroidplatformintegration.h \
            qandroidplatformscreen.h \
            qandroidplatformwindow.h \
            qandroidwindowsurface.h


contains(QT_CONFIG, opengl) {
    QT += opengl
    HEADERS += qandroidplatformglcontext.h   qandroidplatformglwidgetsurface.h   qandroidglwindowsurface.h
    SOURCES += qandroidplatformglcontext.cpp qandroidplatformglwidgetsurface.cpp qandroidglwindowsurface.cpp
}

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
