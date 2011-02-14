TARGET = android-8

DEFINES = QT_STATICPLUGIN

CONFIG += dll

INCLUDEPATH += $$NDK_ROOT/platforms/android-8/arch-arm/usr/include
LIBS += -L$$NDK_ROOT/platforms/android-8/arch-arm/usr/lib -ljnigraphics

include($$QT_SOURCE_TREE/src/plugins/platforms/android/common/sw/sw.pri)

INSTALLS += target
