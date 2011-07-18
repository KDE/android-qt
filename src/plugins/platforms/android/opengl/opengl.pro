TEMPLATE = subdirs

!contains(ANDROID_PLATFORM, android-9): SUBDIRS =  androidGL-5 androidGL-8

SUBDIRS += androidGL-9
