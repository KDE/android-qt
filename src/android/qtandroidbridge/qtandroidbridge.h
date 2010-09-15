#ifndef QTANDROIDBRIDGE_H
#define QTANDROIDBRIDGE_H

#include <jni.h>

namespace QtAndroidBridge
{
    JavaVM *javaVM();
    jobject jniProxyObject();
    void setJavaVM(JavaVM *value);
    void setJniProxyObject(jobject value);
}

#endif //QTANDROIDBRIDGE_H