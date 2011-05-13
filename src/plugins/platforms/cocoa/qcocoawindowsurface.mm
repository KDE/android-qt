/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qcocoawindowsurface.h"

#include <QtCore/qdebug.h>

#include <QtGui/QPainter>

QT_BEGIN_NAMESPACE

QRect flipedRect(const QRect &sourceRect,int height)
{
    if (!sourceRect.isValid())
        return QRect();
    QRect flippedRect = sourceRect;
    flippedRect.moveTop(height - sourceRect.y());
    return flippedRect;
}

QCocoaWindowSurface::QCocoaWindowSurface(QWidget *window, WId wId)
    : QWindowSurface(window)
{
    m_cocoaWindow = static_cast<QCocoaWindow *>(window->platformWindow());

    const QRect geo = window->geometry();
    NSRect rect = NSMakeRect(geo.x(),geo.y(),geo.width(),geo.height());
    m_contentView = [[QNSView alloc] initWithWidget:window];
    m_cocoaWindow->setContentView(m_contentView);

    m_image = new QImage(window->size(),QImage::Format_ARGB32);
}

QCocoaWindowSurface::~QCocoaWindowSurface()
{
    delete m_image;
}

QPaintDevice *QCocoaWindowSurface::paintDevice()
{
    return m_image;
}

void QCocoaWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    QRect geo = region.boundingRect();

    NSRect rect = NSMakeRect(geo.x(), geo.y(), geo.width(), geo.height());
    [m_contentView displayRect:rect];
}

void QCocoaWindowSurface::resize(const QSize &size)
{
    QWindowSurface::resize(size);
    delete m_image;
    m_image = new QImage(size,QImage::Format_ARGB32_Premultiplied);
    NSSize newSize = NSMakeSize(size.width(),size.height());
    [m_contentView setImage:m_image];

}

QT_END_NAMESPACE
