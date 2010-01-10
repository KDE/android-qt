#ifndef ANDROID_APP_H
#define ANDROID_APP_H
#include <jni.h>

class QImage;
class QRect;

namespace QtAndroid
{
    JavaVM* getJavaVM();
    jobject getJniObject();
    long createWindow();
    void destroyWindow(long winId);
    void flushImage(long winId, const QImage & image, const QRect & rect);
    void setWindowGeometry(long winId, const QRect &rect);
}
#endif // ANDROID_APP_H
