DEFINES += QT_STATICPLUGIN
CONFIG += dll

!contains(ANDROID_PLATFORM, android-9) {
    INCLUDEPATH += $$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/include
    LIBS += -L$$NDK_ROOT/platforms/android-9/arch-$$ANDROID_ARCHITECTURE/usr/lib -ljnigraphics -landroid
} else : LIBS += -ljnigraphics -landroid

QT += opengl

include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/opengl/eglfs.pri)
include($$QT_SOURCE_TREE/src/plugins/platforms/android/src/src.pri)


TARGET = androidGL-9
target.path = $$[QT_INSTALL_PLUGINS]/platforms/android
INSTALLS += target
