TARGET = android-8

DEFINES += QT_STATICPLUGIN

CONFIG += dll

INCLUDEPATH += $$NDK_ROOT/platforms/android-8/arch-arm/usr/include

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/include/android

SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/android/asset_manager.cpp

LIBS += -L$$NDK_ROOT/platforms/android-8/arch-arm/usr/lib -L$$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-8/arch-arm/lib -ljnigraphics -lutils

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-8/arch-arm/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-8/arch-arm/include/core

include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/src.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/raster/raster.pri)

INSTALLS += target
