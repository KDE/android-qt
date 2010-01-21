#ifndef ANDROID_APP_H
#define ANDROID_APP_H
#include <jni.h>

class QImage;
class QRect;
class QPoint;

namespace QtAndroid
{
    JavaVM* getJavaVM();
    jobject getJniObject();
    void flushImage(const QPoint & pos, const QImage & image, const QRect & rect);
}
#endif // ANDROID_APP_H
