#ifndef ANDROID_APP_H
#define ANDROID_APP_H

class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidPlatformIntegration;
class QString;
class QWidget;

namespace QtAndroid
{
    void setAndroidPlatformIntegration(QAndroidPlatformIntegration * androidPlatformIntegration);

    void flushImage(int windowId, const QPoint & pos, const QImage & image, const QRect & rect);

    // Window methods
    bool createWindow(bool OpenGL, QWidget * tlw, int windowId, int l, int t, int r, int b);
    bool resizeWindow(int windowId, int l, int t, int r, int b);
    bool destroyWindow(int windowId);
    void setWindowVisiblity(int windowId, bool visible);
    void setWindowOpacity(int windowId, double level);
    void setWindowTitle(int windowId, const QString & title);
    void raiseWindow(int windowId);
    // Window methods

    // Egl methods
    bool makeCurrent(int windowId);
    bool doneCurrent();
    bool swapBuffers(int windowId);
    void* getProcAddress(int windowId, const QString& procName);
    void lockWindow();
    void unlockWindow();
    // Egl methods

    // Software keyboard support
    void showSoftwareKeyboard();
    void hideSoftwareKeyboard();
    // Software keyboard support

    bool hasOpenGL();
}
#endif // ANDROID_APP_H
