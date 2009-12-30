#ifndef ANDROID_APP_H
#define ANDROID_APP_H
#include <jni.h>

namespace QtAndroid
{
    JavaVM* getJavaVM();
    jobject getJniObject();
    long createWindow();
}
#endif // ANDROID_APP_H
