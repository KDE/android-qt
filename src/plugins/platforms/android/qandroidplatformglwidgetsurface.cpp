#include "qandroidplatformglwidgetsurface.h"
#include "androidjnimain.h"

QAndroidPlatformGLWidgetSurface::QAndroidPlatformGLWidgetSurface()
{
}

bool QAndroidPlatformGLWidgetSurface::create(QGLWidget* widget, QGLFormat&)
{
    m_surfaceId=widget->winId();
    return QtAndroid::createSurface(m_surfaceId, widget->geometry().left(), widget->geometry().top(),
                                            widget->geometry().right(), widget->geometry().bottom());
}

void QAndroidPlatformGLWidgetSurface::setGeometry(const QRect& rect)
{
    QtAndroid::resizeSurface(m_surfaceId, rect.left(), rect.top(),
                                            rect.right(), rect.bottom());
}

bool QAndroidPlatformGLWidgetSurface::filterEvent(QEvent* event)
{
    Q_UNUSED(event)
    return false;
}
