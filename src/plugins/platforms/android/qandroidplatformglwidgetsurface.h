#ifndef QANDROIDPLATFORMGLWIDGETSURFACE_H
#define QANDROIDPLATFORMGLWIDGETSURFACE_H

#include <QPlatformGLWidgetSurface>

QT_BEGIN_NAMESPACE

class QAndroidPlatformGLWidgetSurface : public QPlatformGLWidgetSurface
{
public:
    QAndroidPlatformGLWidgetSurface();
    virtual bool create(QGLWidget* widget, QGLFormat& format);
    virtual void setGeometry(const QRect& rect);
    virtual bool filterEvent(QEvent* event);

private:
    WId     m_surfaceId;
};

QT_END_NAMESPACE
#endif // QANDROIDPLATFORMGLWIDGETSURFACE_H
