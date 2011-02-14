TARGET = android-5

DEFINES += QT_STATICPLUGIN

CONFIG += dll

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/include
SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/graphics/jni/bitmap.cpp
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/include/core
LIBS += -L$$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-5/arch-arm/lib -landroid_runtime -lskia

include($$QT_SOURCE_TREE/src/plugins/platforms/android/common/sw/sw.pri)

INSTALLS += target
