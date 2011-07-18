TEMPLATE = subdirs

INCLUDEPATH += $$QT_SOURCE_TREE/src/plugins/platforms/android/src
!contains(ANDROID_PLATFORM, android-5): !contains(ANDROID_PLATFORM, android-9): SUBDIRS  =  android-4
!contains(ANDROID_PLATFORM, android-9): SUBDIRS +=  android-5 android-8
SUBDIRS += android-9
