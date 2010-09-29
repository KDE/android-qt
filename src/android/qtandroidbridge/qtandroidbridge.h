#ifndef QTANDROIDBRIDGE_H
#define QTANDROIDBRIDGE_H

#include <jni.h>

namespace QtAndroidBridge
{
    JavaVM *javaVM();
    jobject jniProxyObject();
}

#endif //QTANDROIDBRIDGE_H