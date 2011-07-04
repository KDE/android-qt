DEFINES += QT_STATICPLUGIN
CONFIG += dll

QT += opengl

INCLUDEPATH += $$NDK_ROOT/platforms/android-5/arch-arm/usr/include

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/include/android

SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/graphics/jni/bitmap.cpp
SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/src/native/android/asset_manager.cpp

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/include/core

include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/opengl/eglfs.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/src.pri)

LIBS += -L$$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/lib -landroid_runtime -lskia -lutils

TARGET = androidGL-5
target.path = $$[QT_INSTALL_PLUGINS]/platforms/android
INSTALLS += target
