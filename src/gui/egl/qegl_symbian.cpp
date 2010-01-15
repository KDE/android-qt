/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include "qegl_p.h"

#include <coecntrl.h>

QT_BEGIN_NAMESPACE

EGLSurface QEglContext::createSurface(QPaintDevice *device, const QEglProperties *properties)
{
    // Create the native drawable for the paint device.
    int devType = device->devType();
    EGLNativePixmapType pixmapDrawable = 0;
    EGLNativeWindowType windowDrawable = 0;
    bool ok;
    if (devType == QInternal::Pixmap) {
        pixmapDrawable = 0;
        ok = (pixmapDrawable != 0);
    } else if (devType == QInternal::Widget) {
        QWidget *w = static_cast<QWidget *>(device);
        windowDrawable = (EGLNativeWindowType)(w->winId()->DrawableWindow());
        ok = (windowDrawable != 0);
    } else {
        ok = false;
    }
    if (!ok) {
        qWarning("QEglContext::createSurface(): Cannot create the native EGL drawable");
        return EGL_NO_SURFACE;
    }

    // Create the EGL surface to draw into, based on the native drawable.
    const int *props;
    if (properties)
        props = properties->properties();
    else
        props = 0;
    EGLSurface surf;
    if (devType == QInternal::Widget)
        surf = eglCreateWindowSurface(dpy, cfg, windowDrawable, 0);
    else
        surf = eglCreatePixmapSurface(dpy, cfg, pixmapDrawable, 0);
    if (surf == EGL_NO_SURFACE)
        qWarning("QEglContext::createSurface(): Unable to create EGL surface, error = 0x%x", eglGetError());
    return surf;
}

EGLDisplay QEglContext::getDisplay(QPaintDevice *device)
{
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (dpy == EGL_NO_DISPLAY)
        qWarning("QEglContext::defaultDisplay(): Falling back to EGL_DEFAULT_DISPLAY");
    return dpy;
}

// Set pixel format and other properties based on a paint device.
void QEglProperties::setPaintDeviceFormat(QPaintDevice *dev)
{
    if(!dev)
        return;

    int devType = dev->devType();
    if (devType == QInternal::Image)
        setPixelFormat(static_cast<QImage *>(dev)->format());
    else
        setPixelFormat(QImage::Format_RGB32);
}


QT_END_NAMESPACE
