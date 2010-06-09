TARGET = QtAndroid_sw

include(../../qpluginbase.pri)

CONFIG -= dll
CONFIG += staticlib static
DESTDIR	 = $$QMAKE_LIBDIR_QT

CONFIG(jnigraphics) : DEFINES += JNIGRPAHICS


SOURCES = main.cpp \
    androidjnimain.cpp \
    qandroidplatformintegration.cpp

HEADERS = qandroidplatformintegration.h

contains(QT_CONFIG, opengl) {
    QT += opengl
}

include(../fb_base/fb_base.pri)

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
