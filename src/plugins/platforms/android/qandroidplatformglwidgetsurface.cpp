#include "qandroidplatformglwidgetsurface.h"
#include "androidjnimain.h"
#include <QDebug>

bool QAndroidPlatformGLWidgetSurface::create(QGLWidget* widget, QGLFormat&)
{
    m_surfaceId=widget->winId();
    return true;
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
