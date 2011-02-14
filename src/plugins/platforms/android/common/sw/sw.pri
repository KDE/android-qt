include($$QT_SOURCE_TREE/src/plugins/qpluginbase.pri)

include($$QT_SOURCE_TREE/src/plugins/platforms/fontdatabases/basicunix/basicunix.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms/android

INCLUDEPATH += $$PWD $$PWD/..

SOURCES += $$PWD/androidplatformplugin.cpp \
           $$PWD/androidjnimain.cpp \
           $$PWD/qandroidplatformintegration.cpp \
           $$PWD/../qandroidinputcontext.cpp

HEADERS += $$PWD/qandroidplatformintegration.h \
           $$PWD/../qandroidinputcontext.h

include($$QT_SOURCE_TREE/src/plugins/platforms/fb_base/fb_base.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms/android
