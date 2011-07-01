TARGET = android-5

DEFINES += QT_STATICPLUGIN

CONFIG += dll

INCLUDEPATH += $$NDK_ROOT/platforms/android-5/arch-arm/usr/include

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/include/android

SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/graphics/jni/bitmap.cpp
SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/android/asset_manager.cpp

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/include/core

LIBS += -L$$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/lib -landroid_runtime -lskia -lutils

include($$QT_SOURCE_TREE/src/plugins/platforms/android/common/common.pri)

INSTALLS += target
