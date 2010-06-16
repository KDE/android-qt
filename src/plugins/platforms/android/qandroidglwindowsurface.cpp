#include "androidjnimain.h"
#include "qandroidglwindowsurface.h"

QAndroidGLWindowSurface::QAndroidGLWindowSurface(QWidget *window):QGLWindowSurface(window)
{
    m_locked=false;
}

bool QAndroidGLWindowSurface::create(QGLWidget* widget, QGLFormat&)
{
    m_surfaceId=widget->winId();
    return true;
}

void QAndroidGLWindowSurface::setGeometry(const QRect& rect)
{
    QtAndroid::resizeSurface(m_surfaceId, rect.left(), rect.top(),
                                            rect.right(), rect.bottom());
}

void QAndroidGLWindowSurface::beginPaint(const QRegion &region)
{
    if (!m_locked)
        QtAndroid::lockSurface();
    m_locked=true;
    QGLWindowSurface::beginPaint(region);
}

void QAndroidGLWindowSurface::endPaint(const QRegion &region)
{
    QGLWindowSurface::endPaint(region);
    QtAndroid::unlockSurface();
    m_locked=false;
}
