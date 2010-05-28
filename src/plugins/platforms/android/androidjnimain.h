#ifndef ANDROID_APP_H
#define ANDROID_APP_H

class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidPlatformIntegration;
class QString;

namespace QtAndroid
{
    void setAndroidGraphicsSystem(QAndroidPlatformIntegration * androidGraphicsSystem);
    void quitApplication();

    void flushImage(int surfaceId, const QPoint & pos, const QImage & image, const QRect & rect);

    // Surface methods
    bool createSurface(int surfaceId, int l, int t, int r, int b);
    bool resizeSurface(int surfaceId, int l, int t, int r, int b);
    bool destroySurface(int surfaceId);
    void setSurfaceVisiblity(int surfaceId, bool visible);
    void setSurfaceOpacity(int surfaceId, double level);
    void setWindowTitle(int surfaceId, const QString & title);
    void raiseSurface(int surfaceId);
    // Surface methods

    // Egl methods
    bool makeCurrent(int surfaceId);
    bool doneCurrent();
    bool swapBuffers(int surfaceId);
    void* getProcAddress(int surfaceId, const QString& procName);
    void lockSurface();
    void unlockSurface();
    // Egl methods

    bool hasOpenGL();
}
#endif // ANDROID_APP_H
