/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quikitwindowsurface.h"
#include "quikitwindow.h"

#include <QtOpenGL/private/qgl_p.h>
#include <QtOpenGL/private/qglpaintdevice_p.h>

#include <QtDebug>

class EAGLPaintDevice;

@interface PaintDeviceHelper : NSObject {
    EAGLPaintDevice *device;
}

@property (nonatomic, assign) EAGLPaintDevice *device;

- (void)eaglView:(EAGLView *)view usesFramebuffer:(GLuint)buffer;

@end

class EAGLPaintDevice : public QGLPaintDevice
{
public:
    EAGLPaintDevice(QPlatformWindow *window)
        :QGLPaintDevice(), mWindow(window)
    {
#if defined(QT_OPENGL_ES_2)
        helper = [[PaintDeviceHelper alloc] init];
        helper.device = this;
        EAGLView *view = static_cast<QUIKitWindow *>(window)->nativeView();
        view.delegate = helper;
        m_thisFBO = view.fbo;
#endif
    }

    ~EAGLPaintDevice()
    {
#if defined(QT_OPENGL_ES_2)
        [helper release];
#endif
    }

    void setFramebuffer(GLuint buffer) { m_thisFBO = buffer; }
    int devType() const { return QInternal::OpenGL; }
    QSize size() const { return mWindow->geometry().size(); }
    QGLContext* context() const { return QGLContext::fromPlatformGLContext(mWindow->glContext()); }

    QPaintEngine *paintEngine() const { return qt_qgl_paint_engine(); }

private:
    QPlatformWindow *mWindow;
    PaintDeviceHelper *helper;
};

@implementation PaintDeviceHelper
@synthesize device;

- (void)eaglView:(EAGLView *)view usesFramebuffer:(GLuint)buffer
{
    Q_UNUSED(view)
    if (device)
        device->setFramebuffer(buffer);
}

@end

QT_BEGIN_NAMESPACE

QUIKitWindowSurface::QUIKitWindowSurface(QWidget *window)
    : QWindowSurface(window), mPaintDevice(new EAGLPaintDevice(window->platformWindow()))
{
}

QPaintDevice *QUIKitWindowSurface::paintDevice()
{
    return mPaintDevice;
}

void QUIKitWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);
    widget->platformWindow()->glContext()->swapBuffers();
}

QWindowSurface::WindowSurfaceFeatures QUIKitWindowSurface::features() const
{
    return 0;
}

QT_END_NAMESPACE
