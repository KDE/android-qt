#include "androidjnimain.h"
#include "qandroidglwindowsurface.h"

QAndroidGLWindowSurface::QAndroidGLWindowSurface(QWidget *window):QGLWindowSurface(window)
{
    m_locked=false;
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
