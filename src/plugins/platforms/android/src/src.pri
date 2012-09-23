include($$QT_SOURCE_TREE/src/plugins/qpluginbase.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/basicunix/basicunix.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms/android

INCLUDEPATH += $$PWD

contains(QT, opengl): DEFINES += ANDROID_PLUGIN_OPENGL

SOURCES += $$PWD/androidplatformplugin.cpp \
           $$PWD/androidjnimain.cpp \
           $$PWD/qandroidplatformintegration.cpp \
           $$PWD/qandroidplatformdesktopservice.cpp \
           $$PWD/qandroidassetsfileenginehandler.cpp \
           $$PWD/qandroidinputcontext.cpp \
           $$PWD/qandroidplatformclipboard.cpp

HEADERS += $$PWD/qandroidplatformintegration.h \
           $$PWD/androidjnimain.h \
           $$PWD/qandroidplatformdesktopservice.h \
           $$PWD/qandroidassetsfileenginehandler.h \
           $$PWD/qandroidinputcontext.h \
           $$PWD/qandroidplatformclipboard.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms/android
