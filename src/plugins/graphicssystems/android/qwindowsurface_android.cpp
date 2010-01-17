/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
#include "androidjnimain.h"
#include "qwindowsurface_android.h"
#include "qgraphicssystem_android.h"
#include "qandroidinput.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE
using namespace QtAndroid;

QAndroidWindowSurface::QAndroidWindowSurface
        (QAndroidGraphicsSystemScreen *screen, QWidget *window)
    : QWindowSurface(window),
      mScreen(screen)
{
    qDebug()<<"QAndroidWindowSurface::QAndroidWindowSurface()";
    mWindowId = createWindow();
    connect(screen, SIGNAL(screenResized(QSize)),SLOT(screenResized(QSize)));
    QAndroidInput::androidInput()->registerWindow(mWindowId, window);
}

QAndroidWindowSurface::~QAndroidWindowSurface()
{
    QAndroidInput::androidInput()->unregisterWindow(mWindowId);
}

void QAndroidWindowSurface::screenResized(const QSize & size)
{
    qDebug()<<"QAndroidWindowSurface::screenResized()";
    QRect widgetRect=window()->geometry();
    if (widgetRect.width()>size.width())
        widgetRect.setWidth(size.width());
    if (widgetRect.height()>size.height())
        widgetRect.setHeight(size.height());
    window()->setGeometry(widgetRect);
}

void QAndroidWindowSurface::resize(const QSize & size)
{
    qDebug()<<"QAndroidWindowSurface::resize()";
    QRect widgetRect=window()->geometry();
    widgetRect.setSize(size);
    window()->setGeometry(widgetRect);
    setGeometry(widgetRect);
}

QPaintDevice *QAndroidWindowSurface::paintDevice()
{
    qDebug() << "QMinimalWindowSurface::paintDevice";
    return &mImage;
}

void QAndroidWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    qDebug() << "QMinimalWindowSurface::flush()";
/*    static int c = 0;
    QString filename = QString("output%1.png").arg(c++, 4, 10, QLatin1Char('0'));
    mImage.save(filename);
    */
    flushImage(mWindowId, mImage, region.boundingRect());
}

void QAndroidWindowSurface::setGeometry(const QRect &rect)
{
    qDebug() << "QMinimalWindowSurface::setGeometry()";
    setWindowGeometry(mWindowId, rect);
    QWindowSurface::setGeometry(rect);
    if (mImage.size() != rect.size())
        mImage = QImage(rect.size(), mScreen->format());
}

bool QAndroidWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    qDebug() << "QMinimalWindowSurface::scroll()";
    return QWindowSurface::scroll(area, dx, dy);
}

void QAndroidWindowSurface::beginPaint(const QRegion &region)
{
//    qDebug() << "QMinimalWindowSurface::beginPaint()";
    Q_UNUSED(region);
}

void QAndroidWindowSurface::endPaint(const QRegion &region)
{
//    qDebug() << "QMinimalWindowSurface::endPaint()";
    Q_UNUSED(region);
}

WId QAndroidWindowSurface::winId() const
{
    qDebug() << "QMinimalWindowSurface::winId()";
    return mWindowId;
}

QT_END_NAMESPACE
