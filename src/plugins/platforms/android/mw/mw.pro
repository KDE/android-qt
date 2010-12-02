TARGET = QtAndroid_mw

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

SOURCES +=  main.cpp \
            androidjnimain.cpp \
            qandroidplatformintegration.cpp \
            qandroidplatformscreen.cpp \
            qandroidplatformwindow.cpp \
            qandroidwindowsurface.cpp \
            qandroidinputcontext.cpp

HEADERS +=  androidjnimain.h \
            qandroidplatformintegration.h \
            qandroidplatformscreen.h \
            qandroidplatformwindow.h \
            qandroidwindowsurface.h \
            qandroidinputcontext.h


contains(QT_CONFIG, opengl) {
    QT += opengl
    HEADERS += qandroidplatformglcontext.h   qandroidglwindowsurface.h
    SOURCES += qandroidplatformglcontext.cpp qandroidglwindowsurface.cpp
}

target.path=$$[QT_INSTALL_LIBS]

INSTALLS += target
