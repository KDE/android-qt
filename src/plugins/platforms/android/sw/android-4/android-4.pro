TARGET = android-4

DEFINES += QT_STATICPLUGIN
CONFIG += dll

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/include

SOURCES += $$QT_SOURCE_TREE/src/plugins/platforms/android/common/native/graphics/jni/bitmap.cpp
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-4/arch-arm/include
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-4/arch-arm/include/core
LIBS += -L$$QT_SOURCE_TREE/src/3rdparty/android/precompiled/android-4/arch-arm/lib  -landroid_runtime -lsgl

include($$QT_SOURCE_TREE/src/plugins/platforms/android/common/sw/sw.pri)

INSTALLS += target
