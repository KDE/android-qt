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

#include "qgraphicssystem_android.h"
#include "qwindowsurface_android.h"
#include "qabstracteventdispatcher.h"
#include "androidjnimain.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <private/qapplication_p.h>
#include "qdesktopwidget.h"

QT_BEGIN_NAMESPACE

int QAndroidGraphicsSystem::mDefaultGeometryWidth=320;
int QAndroidGraphicsSystem::mDefaultGeometryHeight=455;
int QAndroidGraphicsSystem::mDefaultPhysicalSizeWidth=50;
int QAndroidGraphicsSystem::mDefaultPhysicalSizeHeight=71;

QAndroidGraphicsSystem::QAndroidGraphicsSystem()
{
    mDesktopWidget=0;
    mPrimaryScreen = new QAndroidGraphicsSystemScreen();
    mPrimaryScreen->mGeometry = QRect(0, 0, mDefaultGeometryWidth, mDefaultGeometryHeight);
    mPrimaryScreen->mDepth = 16;
    mPrimaryScreen->mFormat = QImage::Format_RGB16;
    mPrimaryScreen->mPhysicalSize = QSize(mDefaultPhysicalSizeWidth, mDefaultPhysicalSizeHeight);
    mScreens.append(mPrimaryScreen);
    m_mainThread=QThread::currentThread();
    QtAndroid::setQtThread(m_mainThread);
    QtAndroid::setAndroidGraphicsSystem(this);

}

void QAndroidGraphicsSystem::setDefaultDisplayMetrics(int gw, int gh, int sw, int sh)
{
    mDefaultGeometryWidth=gw;
    mDefaultGeometryHeight=gh;
    mDefaultPhysicalSizeWidth=sw;
    mDefaultPhysicalSizeHeight=sh;
}

void QAndroidGraphicsSystem::setDefaultDesktopSize(int gw, int gh)
{
    mDefaultGeometryWidth=gw;
    mDefaultGeometryHeight=gh;
}

QPixmapData *QAndroidGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWindowSurface *QAndroidGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    qDebug()<<"createWindowSurface"<<widget;
    if (widget->windowType() == Qt::Desktop)
    {
        mDesktopWidget = widget;
        qDebug()<<"DesktopWidget="<<mDesktopWidget;
        return 0;   // Don't create an explicit window surface for the destkop.
    }
    else
        if (!mDesktopWidget)
            mDesktopWidget = widget;
    return new QAndroidWindowSurface(mPrimaryScreen, widget);
}

void QAndroidGraphicsSystem::updateScreen()
{
    if (mDesktopWidget)
    {
        mDesktopWidget->update();
        if (QAbstractEventDispatcher::instance(m_mainThread))
                QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
    }
}

void QAndroidGraphicsSystem::setDesktopSize(int width, int height)
{
    if (mDesktopWidget)
    {
        QApplicationPrivate::handleGeometryChange(mDesktopWidget,QRect(0,0,width,height));
        if (QAbstractEventDispatcher::instance(m_mainThread))
                QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
    }
}

QT_END_NAMESPACE
