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

#include "qdirectfbcursor.h"
#include "qdirectfbconvenience.h"

QT_BEGIN_NAMESPACE

QDirectFBCursor::QDirectFBCursor(QPlatformScreen *screen)
    : QPlatformCursor(screen)
{
    m_image.reset(new QPlatformCursorImage(0, 0, 0, 0, 0, 0));
}

void QDirectFBCursor::changeCursor(QCursor *cursor, QWidget *)
{
    int xSpot;
    int ySpot;
    QPixmap map;

    if (cursor->shape() != Qt::BitmapCursor) {
        m_image->set(cursor->shape());
        xSpot = m_image->hotspot().x();
        ySpot = m_image->hotspot().y();
        QImage *i = m_image->image();
        map = QPixmap::fromImage(*i);
    } else {
        QPoint point = cursor->hotSpot();
        xSpot = point.x();
        ySpot = point.y();
        map = cursor->pixmap();
    }

    DFBResult res;
    IDirectFBDisplayLayer *layer = toDfbLayer(screen);
    IDirectFBSurface* surface(QDirectFbConvenience::dfbSurfaceForPlatformPixmap(map.pixmapData()));

    res = layer->SetCooperativeLevel(layer, DLSCL_ADMINISTRATIVE);
    if (res != DFB_OK) {
        DirectFBError("Failed to set DLSCL_ADMINISTRATIVE", res);
        return;
    }

    layer->SetCursorShape(layer, surface, xSpot, ySpot);
    layer->SetCooperativeLevel(layer, DLSCL_SHARED);
}

QT_END_NAMESPACE
