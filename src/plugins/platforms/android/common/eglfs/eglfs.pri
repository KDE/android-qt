include($$QT_BUILD_TREE/src/plugins/qpluginbase.pri)

include($$QT_BUILD_TREE/src/plugins/platforms/fontdatabases/basicunix/basicunix.pri)
#include(../../../fontdatabases/genericunix/genericunix.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms/android

QT += opengl

LIBS += -lEGL

INCLUDEPATH += $$PWD $$PWD/..

SOURCES +=  $$PWD/main.cpp \
            $$PWD/qandroideglfsintegration.cpp \
            $$PWD/qeglconvenience.cpp \
            $$PWD/qandroideglplatformcontext.cpp \
            $$PWD/qandroideglfswindow.cpp \
            $$PWD/qandroideglfswindowsurface.cpp \
            $$PWD/qandroideglfsscreen.cpp \
            $$PWD/androidjnimain.cpp \
    ../../common/eglfs/qandroideglfspaintdevice.cpp

HEADERS +=  $$PWD/qandroideglfsintegration.h \
            $$PWD/qeglconvenience.h \
            $$PWD/qandroideglplatformcontext.h \
            $$PWD/qandroideglfswindow.h \
            $$PWD/qandroideglfswindowsurface.h \
            $$PWD/qandroideglfsscreen.h \
            $$PWD/androidjnimain.h \
            $$PWD/gl_code.h \
    ../../common/eglfs/qandroideglfspaintdevice.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms/android
