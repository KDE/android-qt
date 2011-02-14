/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY  BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
    qDebug()<<"Window ID="<<m_surfaceId;
    return QtAndroid::createWindow(true, widget, m_surfaceId, widget->geometry().left(), widget->geometry().top(),
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

QPlatformWindowFormat QAndroidPlatformGLContext::platformWindowFormat() const
{
    return m_platformWindowFormat;
}
