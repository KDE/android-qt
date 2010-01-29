/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QGRAPHICSSYSTEM_MINIMAL_H
#define QGRAPHICSSYSTEM_MINIMAL_H

#include "qdirectfbinput.h"

#include <QtGui/private/qgraphicssystem_p.h>
#include <directfb.h>
#include <directfb_version.h>

QT_BEGIN_NAMESPACE

class QDirectFBCursor;

class QDirectFbGraphicsSystemScreen : public QGraphicsSystemScreen
{
public:
    QDirectFbGraphicsSystemScreen(int display);
    ~QDirectFbGraphicsSystemScreen();

    QRect geometry() const { return m_geometry; }
    int depth() const { return m_depth; }
    QImage::Format format() const { return m_format; }
    QSize physicalSize() const { return m_physicalSize; }

public:
    QRect m_geometry;
    int m_depth;
    QImage::Format m_format;
    QSize m_physicalSize;

    IDirectFBDisplayLayer *m_layer;

private:
    QDirectFBCursor * cursor;

};

class QDirectFbGraphicsSystem : public QGraphicsSystem
{
public:
    QDirectFbGraphicsSystem();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QWindowSurface *createWindowSurface(QWidget *widget) const;
    QBlittable *createBlittable(const QRect &rect) const;

    QList<QGraphicsSystemScreen *> screens() const { return mScreens; }



private:
    QDirectFbGraphicsSystemScreen *mPrimaryScreen;
    QList<QGraphicsSystemScreen *> mScreens;
};

QT_END_NAMESPACE

#endif
