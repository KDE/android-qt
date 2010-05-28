#include "qandroidplatformglcontext.h"
#include <QPaintDevice>
#include <QDebug>
#include <QGLFormat>
#include <QPlatformGLContext>
#include "androidjnimain.h"

bool QAndroidPlatformGLContext::create(QPaintDevice* device, QGLFormat& /*format*/, QPlatformGLContext* /*shareContext*/)
{
    if (device->devType() != QInternal::Widget) {
        qWarning("Creating a GL context is only supported on QWidgets");
        return false;
    }

    QWidget* widget = static_cast<QWidget*>(device);

    if (!widget->isTopLevel()) {
        qWarning("Creating a GL context is only supported on top-level QWidgets");
        return false;
    }
    m_surfaceId=widget->winId();
    qDebug()<<"Surface ID="<<m_surfaceId;
    return QtAndroid::createSurface(m_surfaceId, widget->geometry().left(), widget->geometry().top(),
                                            widget->geometry().right(), widget->geometry().bottom());
}

void QAndroidPlatformGLContext::makeCurrent()
{
    QtAndroid::makeCurrent(m_surfaceId);
}

void QAndroidPlatformGLContext::doneCurrent()
{
    QtAndroid::doneCurrent();
}

void QAndroidPlatformGLContext::swapBuffers()
{
    QtAndroid::swapBuffers(m_surfaceId);
}

void* QAndroidPlatformGLContext::getProcAddress(const QString& procName)
{
    return QtAndroid::getProcAddress(m_surfaceId, procName);
}
