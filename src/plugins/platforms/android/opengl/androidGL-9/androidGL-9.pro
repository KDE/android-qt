DEFINES += QT_STATICPLUGIN
CONFIG += dll

INCLUDEPATH += $$NDK_ROOT/platforms/android-9/arch-arm/usr/include
LIBS += -L$$NDK_ROOT/platforms/android-9/arch-arm/usr/lib -ljnigraphics -landroid

QT += opengl

include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/opengl/eglfs.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/src.pri)


TARGET = androidGL-9
target.path = $$[QT_INSTALL_PLUGINS]/platforms/android
INSTALLS += target
