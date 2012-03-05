/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgraphicssystem_vg_p.h"
#include <QtOpenVG/private/qpixmapdata_vg_p.h>
#include <QtOpenVG/private/qwindowsurface_vg_p.h>
#include <QtOpenVG/private/qvgimagepool_p.h>
#if defined(Q_OS_SYMBIAN)
#include <QtGui/private/qwidget_p.h>
#endif
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE

QVGGraphicsSystem::QVGGraphicsSystem()
{
    QApplicationPrivate::graphics_system_name = QLatin1String("openvg");
}

QPixmapData *QVGGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
#if !defined(QVG_NO_SINGLE_CONTEXT) && !defined(QVG_NO_PIXMAP_DATA)
    // Pixmaps can use QVGPixmapData; bitmaps must use raster.
    if (type == QPixmapData::PixmapType)
        return new QVGPixmapData(type);
    else
        return new QRasterPixmapData(type);
#else
    return new QRasterPixmapData(type);
#endif
}

QWindowSurface *QVGGraphicsSystem::createWindowSurface(QWidget *widget) const
{
#if defined(Q_OS_SYMBIAN)
    if (!QApplicationPrivate::instance()->useTranslucentEGLSurfaces) {
        QWidgetPrivate *d = qt_widget_private(widget);
        if (!d->isOpaque && widget->testAttribute(Qt::WA_TranslucentBackground))
            return d->createDefaultWindowSurface_sys();
    }
#endif
    return new QVGWindowSurface(widget);
}

#ifdef Q_OS_SYMBIAN
void QVGGraphicsSystem::releaseCachedGpuResources()
{
    QVGImagePool::instance()->hibernate();
}

QGraphicsSystemEx *QVGGraphicsSystem::platformExtension()
{
    return this;
}
#endif
QT_END_NAMESPACE
