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

#include "qandroidplatformintegration.h"
#include "qandroidplatformscreen.h"
#include "qandroidplatformwindow.h"
#include "qandroidwindowsurface.h"
#include "androidjnimain.h"
#include "qabstracteventdispatcher.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QThread>
#include <QDebug>

#ifndef QT_NO_OPENGL
#include "qandroidcontext.h"
#endif

QT_BEGIN_NAMESPACE

int QAndroidPlatformIntegration::mDefaultGeometryWidth=320;
int QAndroidPlatformIntegration::mDefaultGeometryHeight=455;
int QAndroidPlatformIntegration::mDefaultPhysicalSizeWidth=50;
int QAndroidPlatformIntegration::mDefaultPhysicalSizeHeight=71;


QAndroidPlatformIntegration::QAndroidPlatformIntegration()
{
    mPrimaryScreen = new QAndroidPlatformScreen();
    mScreens.append(mPrimaryScreen);
    m_mainThread=QThread::currentThread();
    QtAndroid::setQtThread(m_mainThread);
    QtAndroid::setAndroidGraphicsSystem(this);
}

void QAndroidPlatformIntegration::setDefaultDisplayMetrics(int gw, int gh, int sw, int sh)
{
    mDefaultGeometryWidth=gw;
    mDefaultGeometryHeight=gh;
    mDefaultPhysicalSizeWidth=sw;
    mDefaultPhysicalSizeHeight=sh;
}

void QAndroidPlatformIntegration::setDefaultDesktopSize(int gw, int gh)
{
    mDefaultGeometryWidth=gw;
    mDefaultGeometryHeight=gh;
}

QPixmapData *QAndroidPlatformIntegration::createPixmapData(QPixmapData::PixelType type) const
{
     return new QRasterPixmapData(type);
}

QWindowSurface *QAndroidPlatformIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    qDebug()<<"QAndroidPlatformIntegration::createWindowSurface"<<widget<<widget->winId()<<winId;
    return new QAndroidWindowSurface(widget);
}

QPlatformWindow *QAndroidPlatformIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    qDebug()<<"QAndroidPlatformIntegration::createPlatformWindow"<<widget<<widget->winId()<<winId;
    return new QAndroidPlatformWindow(widget, mPrimaryScreen);
}

void QAndroidPlatformIntegration::updateScreen()
{
    if (mPrimaryScreen)
    {
        mPrimaryScreen->redrawScreen();
        if (QAbstractEventDispatcher::instance(m_mainThread))
                QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
    }
}

void QAndroidPlatformIntegration::setDesktopSize(int width, int height)
{
    if (mPrimaryScreen)
    {
        mPrimaryScreen->setGeometry(QRect(0,0,width, height));
        if (QAbstractEventDispatcher::instance(m_mainThread))
                QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
    }
}

void QAndroidPlatformIntegration::setDisplayMetrics(int width, int height)
{
    if (mPrimaryScreen)
    {
        mPrimaryScreen->setPhysicalSize(QSize(width, height));
        if (QAbstractEventDispatcher::instance(m_mainThread))
                QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
    }
}

#ifndef QT_NO_OPENGL
bool QAndroidPlatformIntegration::hasOpenGL() const
{
    return false;
}

QPlatformGLContext * QAndroidPlatformIntegration::createGLContext()
{
    return new QAndroidContext();
}
#endif // QT_NO_OPENGL

QT_END_NAMESPACE
