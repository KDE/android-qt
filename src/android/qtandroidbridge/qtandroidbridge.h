#ifndef QTANDROIDBRIDGE_H
#define QTANDROIDBRIDGE_H

#include <jni.h>
#include <QtCore/qglobal.h>
#if !defined(Q_ANDROID_EXPORT)
#  if defined(QT_SHARED)
#    define Q_ANDROID_EXPORT Q_DECL_EXPORT
#  else
#    define Q_ANDROID_EXPORT
#  endif
#endif

namespace QtAndroidBridge
{
    Q_ANDROID_EXPORT JavaVM *javaVM();
    Q_ANDROID_EXPORT jobject jniProxyObject();
}

#endif //QTANDROIDBRIDGE_H