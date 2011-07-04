DEFINES += QT_STATICPLUGIN
CONFIG += dll

QT += opengl

INCLUDEPATH += $$NDK_ROOT/platforms/android-8/arch-arm/usr/include

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/include/android

SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/android/asset_manager.cpp

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-8/arch-arm/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-8/arch-arm/include/core

include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/opengl/eglfs.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/src.pri)

LIBS += -L$$NDK_ROOT/platforms/android-8/arch-arm/usr/lib -L$$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-8/arch-arm/lib -ljnigraphics -lutils

TARGET = androidGL-8
target.path = $$[QT_INSTALL_PLUGINS]/platforms/android
INSTALLS += target
