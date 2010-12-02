TARGET = QtAndroid_sw

include(../../../qpluginbase.pri)

DEFINES = QT_STATICPLUGIN

include(../../fontdatabases/basicunix/basicunix.pri)

CONFIG += dll
DESTDIR = $$QMAKE_LIBDIR_QT

CONFIG(android-8) : LIBS += -ljnigraphics
else{
                    INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/include
                    SOURCES += ../common/native/graphics/jni/bitmap.cpp
CONFIG(android-4) : LIBS += -landroid_runtime -lsgl
             else : LIBS += -landroid_runtime -lskia
}


SOURCES += main.cpp \
    androidjnimain.cpp \
    qandroidplatformintegration.cpp \
    qandroidinputcontext.cpp

HEADERS += qandroidplatformintegration.h \
           qandroidinputcontext.h

contains(QT_CONFIG, opengl) {
    QT += opengl
}

include(../../fb_base/fb_base.pri)

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
