SOURCES = stltest.cpp
CONFIG -= qt dylib
mac:CONFIG -= app_bundle
android: LIBS += $$ANDROID_SOURCES_CXX_STL_LIBDIR/libgnustl_static.a
