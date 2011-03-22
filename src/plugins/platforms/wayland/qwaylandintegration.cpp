/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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

#include "qwaylandintegration.h"

#include "qwaylanddisplay.h"
#include "qwaylandshmsurface.h"
#include "qwaylanddrmsurface.h"
#include "qwaylandshmwindow.h"
#include "qwaylandeglwindow.h"

#include "qfontconfigdatabase.h"

#include <QtGui/QWindowSystemInterface>
#include <QtGui/QPlatformCursor>
#include <QtGui/QPlatformWindowFormat>

#include <QtGui/private/qpixmap_raster_p.h>
#ifdef QT_WAYLAND_GL_SUPPORT
#include <QtOpenGL/private/qpixmapdata_gl_p.h>
#endif

QWaylandIntegration::QWaylandIntegration(bool useOpenGL)
    : mFontDb(new QFontconfigDatabase())
    , mDisplay(new QWaylandDisplay())
    , mUseOpenGL(useOpenGL)
{
}

QList<QPlatformScreen *>
QWaylandIntegration::screens() const
{
    return mDisplay->screens();
}

QPixmapData *QWaylandIntegration::createPixmapData(QPixmapData::PixelType type) const
{
#ifdef QT_WAYLAND_GL_SUPPORT
    if (mUseOpenGL)
        return new QGLPixmapData(type);
#endif
    return new QRasterPixmapData(type);
}



QPlatformWindow *QWaylandIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    bool useOpenGL = mUseOpenGL || (widget->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenGL);
    if (useOpenGL)
        return new QWaylandEglWindow(widget);

    return new QWaylandShmWindow(widget);
}

QWindowSurface *QWaylandIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    Q_UNUSED(winId);
    bool useOpenGL = mUseOpenGL || (widget->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenGL);
    if (useOpenGL)
        return new QWaylandDrmWindowSurface(widget);

    return new QWaylandShmWindowSurface(widget);
}

QPlatformFontDatabase *QWaylandIntegration::fontDatabase() const
{
    return mFontDb;
}
