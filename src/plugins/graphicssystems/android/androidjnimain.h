#ifndef ANDROID_APP_H
#define ANDROID_APP_H
#include <jni.h>

class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidGraphicsSystem;

namespace QtAndroid
{
    JavaVM* getJavaVM();
    jobject getJniObject();
    void flushImage(const QPoint & pos, const QImage & image, const QRect & rect);
    void setAndroidGraphicsSystem(QAndroidGraphicsSystem * androidGraphicsSystem);
    void setQtThread(QThread * thread);
}
#endif // ANDROID_APP_H
