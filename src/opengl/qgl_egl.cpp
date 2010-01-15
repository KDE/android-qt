/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include <QtOpenGL/qgl.h>
#include "qgl_p.h"
#include "qgl_egl_p.h"

QT_BEGIN_NAMESPACE

// Set device configuration attributes from a QGLFormat instance.
void qt_egl_set_format(QEglProperties& props, int deviceType, const QGLFormat& f)
{
    if (deviceType == QInternal::Pixmap || deviceType == QInternal::Image)
        props.setValue(EGL_SURFACE_TYPE, EGL_PIXMAP_BIT);
    else if (deviceType == QInternal::Pbuffer)
        props.setValue(EGL_SURFACE_TYPE, EGL_PBUFFER_BIT);
    else
        props.setValue(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);

    // Set the pixel format to that contained in the QGLFormat
    // if the system hasn't already chosen a fixed format to
    // match the pixmap, widget, etc.
    if (props.value(EGL_RED_SIZE) == 0 || f.redBufferSize() != -1)
        props.setValue(EGL_RED_SIZE, f.redBufferSize() == -1 ? 1 : f.redBufferSize());
    if (props.value(EGL_GREEN_SIZE) == 0 || f.greenBufferSize() != -1)
        props.setValue(EGL_GREEN_SIZE, f.greenBufferSize() == -1 ? 1 : f.greenBufferSize());
    if (props.value(EGL_BLUE_SIZE) == 0 || f.blueBufferSize() != -1)
        props.setValue(EGL_BLUE_SIZE, f.blueBufferSize() == -1 ? 1 : f.blueBufferSize());
    if (f.alpha()) {
        if (props.value(EGL_ALPHA_SIZE) == 0 || f.alphaBufferSize() != -1)
            props.setValue(EGL_ALPHA_SIZE, f.alphaBufferSize() == -1 ? 1 : f.alphaBufferSize());
    }

    if (f.depth())
        props.setValue(EGL_DEPTH_SIZE, f.depthBufferSize() == -1 ? 1 : f.depthBufferSize());
    if (f.stencil())
        props.setValue(EGL_STENCIL_SIZE, f.stencilBufferSize() == -1 ? 1 : f.stencilBufferSize());
    if (f.sampleBuffers()) {
        props.setValue(EGL_SAMPLE_BUFFERS, 1);
        props.setValue(EGL_SAMPLES, f.samples() == -1 ? 1 : f.samples());
    } else {
        props.setValue(EGL_SAMPLE_BUFFERS, 0);
    }
    if (deviceType == QInternal::Widget)
        props.setValue(EGL_LEVEL, f.plane());
}

// Updates "format" with the parameters of the selected configuration.
void qt_egl_update_format(const QEglContext& context, QGLFormat& format)
{
    EGLint value = 0;

    if (context.configAttrib(EGL_RED_SIZE, &value))
        format.setRedBufferSize(value);
    if (context.configAttrib(EGL_GREEN_SIZE, &value))
        format.setGreenBufferSize(value);
    if (context.configAttrib(EGL_BLUE_SIZE, &value))
        format.setBlueBufferSize(value);
    if (context.configAttrib(EGL_ALPHA_SIZE, &value)) {
        format.setAlpha(value != 0);
        if (format.alpha())
            format.setAlphaBufferSize(value);
    }

    if (context.configAttrib(EGL_DEPTH_SIZE, &value)) {
        format.setDepth(value != 0);
        if (format.depth())
            format.setDepthBufferSize(value);
    }

    if (context.configAttrib(EGL_LEVEL, &value))
        format.setPlane(value);

    if (context.configAttrib(EGL_SAMPLE_BUFFERS, &value)) {
        format.setSampleBuffers(value != 0);
        if (format.sampleBuffers()) {
            context.configAttrib(EGL_SAMPLES, &value);
            format.setSamples(value);
        }
    }

    if (context.configAttrib(EGL_STENCIL_SIZE, &value)) {
        format.setStencil(value != 0);
        if (format.stencil())
            format.setStencilBufferSize(value);
    }

    // Clear the EGL error state because some of the above may
    // have errored out because the attribute is not applicable
    // to the surface type.  Such errors don't matter.
    context.clearError();
}

bool QGLFormat::hasOpenGL()
{
    return true;
}

void QGLContext::reset()
{
    Q_D(QGLContext);
    if (!d->valid)
        return;
    d->cleanup();
    doneCurrent();
    if (d->eglContext) {
        d->destroyEglSurfaceForDevice();
        delete d->eglContext;
    }
    d->eglContext = 0;
    d->eglSurface = EGL_NO_SURFACE;
    d->crWin = false;
    d->sharing = false;
    d->valid = false;
    d->transpColor = QColor();
    d->initDone = false;
    QGLContextGroup::removeShare(this);
}

void QGLContext::makeCurrent()
{
    Q_D(QGLContext);
    if (!d->valid || !d->eglContext || d->eglSurface == EGL_NO_SURFACE) {
        qWarning("QGLContext::makeCurrent(): Cannot make invalid context current");
        return;
    }

    if (d->eglContext->makeCurrent(d->eglSurface))
        QGLContextPrivate::setCurrentContext(this);
}

void QGLContext::doneCurrent()
{
    Q_D(QGLContext);
    if (d->eglContext)
        d->eglContext->doneCurrent();

    QGLContextPrivate::setCurrentContext(0);
}


void QGLContext::swapBuffers() const
{
    Q_D(const QGLContext);
    if (!d->valid || !d->eglContext)
        return;

    d->eglContext->swapBuffers(d->eglSurface);
}

void QGLContextPrivate::destroyEglSurfaceForDevice()
{
    if (eglSurface != EGL_NO_SURFACE) {
#ifdef Q_WS_X11
        // Make sure we don't call eglDestroySurface on a surface which
        // was created for a different winId:
        if (paintDevice->devType() == QInternal::Widget) {
            QGLWidget* w = static_cast<QGLWidget*>(paintDevice);

            if (w->d_func()->eglSurfaceWindowId == w->winId())
                eglDestroySurface(eglContext->display(), eglSurface);
            else
                qWarning("WARNING: Potential EGL surface leak!");
        } else
#endif
            eglDestroySurface(eglContext->display(), eglSurface);
        eglSurface = EGL_NO_SURFACE;
    }
}

void QGLWidget::setMouseTracking(bool enable)
{
    QWidget::setMouseTracking(enable);
}

QColor QGLContext::overlayTransparentColor() const
{
    return d_func()->transpColor;
}

uint QGLContext::colorIndex(const QColor &c) const
{
    Q_UNUSED(c);
    return 0;
}

void QGLContext::generateFontDisplayLists(const QFont & fnt, int listBase)
{
    Q_UNUSED(fnt);
    Q_UNUSED(listBase);
}

void *QGLContext::getProcAddress(const QString &proc) const
{
    return (void*)eglGetProcAddress(reinterpret_cast<const char *>(proc.toLatin1().data()));
}

bool QGLWidgetPrivate::renderCxPm(QPixmap*)
{
    return false;
}

QT_END_NAMESPACE
