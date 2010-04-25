#include "qandroidcontext.h"
#include <QPaintDevice>
#include <QDebug>
#include <QGLFormat>
#include <QPlatformGLContext>
#include "androidjnimain.h"

QAndroidContext::QAndroidContext()
{
}

QAndroidContext::~QAndroidContext()
{
}

bool QAndroidContext::create(QPaintDevice* device, const QGLFormat& format, QPlatformGLContext* shareContext)
{
    if (device->devType() != QInternal::Widget) {
        qWarning("Creating a GL context is only supported on QWidgets");
        return false;
    }

    QWidget* m_widget = static_cast<QWidget*>(device);
    if (!m_widget->isTopLevel()) {
        qWarning("Creating a GL context is only supported on top-level QWidgets");
        return false;
    }
    qDebug()<<"AndroidContext::create"<<device<</*format<<*/shareContext;
    return false;
}

void QAndroidContext::makeCurrent()
{
}

void QAndroidContext::doneCurrent()
{
}

void QAndroidContext::swapBuffers()
{

}

void* QAndroidContext::getProcAddress(const QString& procName)
{
    return NULL;
}
