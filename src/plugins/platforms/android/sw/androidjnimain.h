#ifndef ANDROID_APP_H
#define ANDROID_APP_H

class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidPlatformIntegration;

namespace QtAndroid
{
    void flushImage(const QPoint & pos, const QImage & image, const QRect & rect);
    void setAndroidPlatformIntegration(QAndroidPlatformIntegration * androidGraphicsSystem);
    void setQtThread(QThread * thread);

    void setFullScreen(bool fullScreen);

    // Software keyboard support
    void showSoftwareKeyboard();
    void hideSoftwareKeyboard();
    // Software keyboard support
}
#endif // ANDROID_APP_H
