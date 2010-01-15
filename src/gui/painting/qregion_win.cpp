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

#include "qatomic.h"
#include "qbitmap.h"
#include "qbuffer.h"
#include "qimage.h"
#include "qpolygon.h"
#include "qregion.h"
#include "qt_windows.h"
#include "qpainterpath.h"

QT_BEGIN_NAMESPACE

QRegion::QRegionData QRegion::shared_empty = { Q_BASIC_ATOMIC_INITIALIZER(1), 0, 0 };

HRGN qt_tryCreateRegion(QRegion::RegionType type, int left, int top, int right, int bottom)
{
    const int tries = 10;
    for (int i = 0; i < tries; ++i) {
        HRGN region = 0;
        switch (type) {
        case QRegion::Rectangle:
            region = CreateRectRgn(left, top, right, bottom);
            break;
        case QRegion::Ellipse:
#ifndef Q_OS_WINCE
            region = CreateEllipticRgn(left, top, right, bottom);
#endif
            break;
        }
        if (region) {
            if (GetRegionData(region, 0, 0))
                return region;
            else
                DeleteObject(region);
        }
    }
    return 0;
}

QRegion qt_region_from_HRGN(HRGN rgn)
{
    int numBytes = GetRegionData(rgn, 0, 0);
    if (numBytes == 0)
        return QRegion();

    char *buf = new char[numBytes];
    if (buf == 0)
        return QRegion();

    RGNDATA *rd = reinterpret_cast<RGNDATA*>(buf);
    if (GetRegionData(rgn, numBytes, rd) == 0) {
        delete [] buf;
        return QRegion();
    }

    QRegion region;
    RECT *r = reinterpret_cast<RECT*>(rd->Buffer);
    for (uint i = 0; i < rd->rdh.nCount; ++i) {
        QRect rect;
        rect.setCoords(r->left, r->top, r->right - 1, r->bottom - 1);
        ++r;
        region |= rect;
    }

    delete [] buf;

    return region;
}

void qt_win_dispose_rgn(HRGN r)
{
    if (r)
        DeleteObject(r);
}

static void qt_add_rect(HRGN &winRegion, QRect r)
{
    HRGN rgn = CreateRectRgn(r.left(), r.top(), r.x() + r.width(), r.y() + r.height());
    if (rgn) {
        HRGN dest = CreateRectRgn(0,0,0,0);
        int result = CombineRgn(dest, winRegion, rgn, RGN_OR);
        if (result) {
            DeleteObject(winRegion);
            winRegion = dest;
        }
        DeleteObject(rgn);
    }
}

void QRegion::ensureHandle() const
{
    if (d->rgn)
        DeleteObject(d->rgn);
    d->rgn = CreateRectRgn(0,0,0,0);
    if (d->qt_rgn) {
        if (d->qt_rgn->numRects == 1) {
            QRect r = d->qt_rgn->extents;
            qt_add_rect(d->rgn, r);
            return;
        }
        for (int i = 0;i < d->qt_rgn->numRects;i++) {
            QRect r = d->qt_rgn->rects.at(i);
            qt_add_rect(d->rgn, r);
        }
    }
}


QT_END_NAMESPACE
