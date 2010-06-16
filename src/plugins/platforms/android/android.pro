TARGET = QtAndroid

include(../../qpluginbase.pri)

CONFIG -= dll
CONFIG += staticlib static
DESTDIR	 = $$QMAKE_LIBDIR_QT

CONFIG(jnigraphics) : DEFINES += JNIGRPAHICS

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
    HEADERS += qandroidplatformglcontext.h   qandroidglwindowsurface.h
    SOURCES += qandroidplatformglcontext.cpp qandroidglwindowsurface.cpp
}

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
