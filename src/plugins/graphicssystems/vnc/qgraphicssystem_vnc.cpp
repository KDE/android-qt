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

#include "qgraphicssystem_vnc.h"
#include "qwindowsurface_vnc.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/qdebug.h>

#include <qvncserver.h>




QVNCGraphicsSystemScreen::QVNCGraphicsSystemScreen()
        : mDepth(16), mFormat(QImage::Format_RGB16), mScreenImage(0)
{
    mGeometry = QRect(0,0,800, 600);

    mDepth = 32;
    mFormat = QImage::Format_RGB32;
    mPhysicalSize = (mGeometry.size()*254)/720;


    mScreenImage = new QImage(mGeometry.size(), mFormat);
    d_ptr = new QVNCGraphicsSystemScreenPrivate(this);
}


QVNCGraphicsSystemScreen::~QVNCGraphicsSystemScreen()
{
    delete mScreenImage;
}

QVNCDirtyMap *QVNCGraphicsSystemScreen::dirtyMap()
{
    return d_ptr->dirty;
}


void QVNCGraphicsSystemScreen::setDirty(const QRect &rect)
{
    d_ptr->setDirty(rect);
}


QVNCGraphicsSystem::QVNCGraphicsSystem()
{
////////    xd = new MyDisplay;

    mPrimaryScreen = new QVNCGraphicsSystemScreen();


    int dw = mPrimaryScreen->geometry().width();
    int dh = mPrimaryScreen->geometry().height();


    mScreens.append(mPrimaryScreen);
}

QPixmapData *QVNCGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QVNCGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    if (widget->windowType() == Qt::Desktop)
        return 0;   // Don't create an explicit window surface for the destkop.
    return new QVNCWindowSurface
        (const_cast<QVNCGraphicsSystem *>(this), mPrimaryScreen, widget);
}