#ifndef ANDROID_APP_H
#define ANDROID_APP_H
#include <jni.h>

class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidPlatformIntegration;
class QString;

namespace QtAndroid
{
    void setAndroidGraphicsSystem(QAndroidPlatformIntegration * androidGraphicsSystem);
    void setQtThread(QThread * thread);
    void quitApplication();

    void flushImage(int surfaceId, const QPoint & pos, const QImage & image, const QRect & rect);

    bool createSurface(int surfaceId, int l, int t, int r, int b);
    bool resizeSurface(int surfaceId, int l, int t, int r, int b);
    bool destroySurface(int surfaceId);
    void setSurfaceVisiblity(int surfaceId, bool visible);
    void setSurfaceOpacity(int surfaceId, double level);
    void setWindowTitle(int surfaceId, const QString & title);
    void raiseSurface(int surfaceId);

    bool makeCurrent(int surfaceId);
    bool doneCurrent(int surfaceId);
    bool swapBuffers(int surfaceId);
    void* getProcAddress(int surfaceId, const QString& procName);

}
#endif // ANDROID_APP_H
