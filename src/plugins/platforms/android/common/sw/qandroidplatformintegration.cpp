/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY  BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "qandroidplatformintegration.h"
#include "qandroidinputcontext.h"
#include "qabstracteventdispatcher.h"
#include "qbasicunixfontdatabase.h"
#include "androidjnimain.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QWindowSystemInterface>
#include <QThread>
#include <QPlatformWindow>
#include <QDir>
#include <QApplication>
#include "qdesktopwidget.h"

QT_BEGIN_NAMESPACE

int QAndroidPlatformIntegration::mDefaultGeometryWidth=320;
int QAndroidPlatformIntegration::mDefaultGeometryHeight=455;
int QAndroidPlatformIntegration::mDefaultPhysicalSizeWidth=50;
int QAndroidPlatformIntegration::mDefaultPhysicalSizeHeight=71;

class QAndroidPlatformFontDatabase: public QBasicUnixFontDatabase
{
    public:
            virtual QString fontDir() const
            {
                return QLatin1String("/system/fonts");
            }

            virtual void populateFontDatabase()
            {
                QPlatformFontDatabase::populateFontDatabase();
                QString fontpath = fontDir();

                if(!QFile::exists(fontpath)) {
                    qFatal("QFontDatabase: Cannot find font directory %s - is Qt installed correctly?",
                        qPrintable(fontpath));
                }

                QDir dir(fontpath, QLatin1String("Droid*.ttf"));
                for (int i = 0; i < int(dir.count()); ++i) {
                    const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
                    addTTFile(QByteArray(), file);
                }
            }
};

QAndroidPlatformScreen::QAndroidPlatformScreen():QFbScreen()
{
    mGeometry = QRect(0, 0, QAndroidPlatformIntegration::mDefaultGeometryWidth, QAndroidPlatformIntegration::mDefaultGeometryHeight);
    mFormat = QImage::Format_RGB16;
    mDepth = 16;
    setPhysicalSize(QSize(QAndroidPlatformIntegration::mDefaultPhysicalSizeWidth,
                          QAndroidPlatformIntegration::mDefaultPhysicalSizeHeight));
    setGeometry(QRect(0,0,QAndroidPlatformIntegration::mDefaultGeometryWidth
                      ,QAndroidPlatformIntegration::mDefaultGeometryHeight));
    setFormat(mFormat);
    qDebug()<<"QAndroidPlatformScreen::QAndroidPlatformScreen():QFbScreen()";
}

QRegion QAndroidPlatformScreen::doRedraw()
{
    QRegion touched;
    touched = QFbScreen::doRedraw();

    QVector<QRect> rects = touched.rects();
    for (int i = 0; i < rects.size(); i++)
        QtAndroid::flushImage(mGeometry.topLeft(), *mScreenImage, rects[i]);
    return touched;
}

QAndroidPlatformIntegration::QAndroidPlatformIntegration()
{
    mPrimaryScreen = new QAndroidPlatformScreen();
    mScreens.append(mPrimaryScreen);
    m_mainThread=QThread::currentThread();
    QtAndroid::setAndroidPlatformIntegration(this);
    qApp->setInputContext( new QAndroidInputContext() );
    mAndroidFDB = new QAndroidPlatformFontDatabase();
}

QAndroidPlatformIntegration::~QAndroidPlatformIntegration()
{
    delete mAndroidFDB;
    QtAndroid::setAndroidPlatformIntegration(NULL);
}

QPlatformFontDatabase *QAndroidPlatformIntegration::fontDatabase() const
{
    return mAndroidFDB;
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

QWindowSurface *QAndroidPlatformIntegration::createWindowSurface(QWidget *widget, WId /*winId*/) const
{
    return new QFbWindowSurface(mPrimaryScreen, widget);
}

QPlatformWindow *QAndroidPlatformIntegration::createPlatformWindow(QWidget *widget, WId /*winId*/) const
{
    QFbWindow *w = new QFbWindow(widget);
    mPrimaryScreen->addWindow(w);
    qDebug()<<"createPlatformWindow"<<widget->isFullScreen();
    if (widget->isFullScreen())
        QtAndroid::setFullScreen(true);
    else
        QtAndroid::setFullScreen(false);
    return w;
}

void QAndroidPlatformIntegration::setDesktopSize(int width, int height)
{
    qDebug()<<"setDesktopSize";
    if (mPrimaryScreen)
        QMetaObject::invokeMethod(mPrimaryScreen, "setGeometry", Qt::AutoConnection, Q_ARG(QRect, QRect(0,0,width, height)));
    qDebug()<<"setDesktopSize done";
}

void QAndroidPlatformIntegration::setDisplayMetrics(int width, int height)
{
    qDebug()<<"setDisplayMetrics";
    if (mPrimaryScreen)
        QMetaObject::invokeMethod(mPrimaryScreen, "setPhysicalSize", Qt::AutoConnection, Q_ARG(QSize, QSize(width, height)));
    qDebug()<<"setDisplayMetrics done";
}

void QAndroidPlatformIntegration::pauseApp()
{
    if (QAbstractEventDispatcher::instance(m_mainThread))
        QAbstractEventDispatcher::instance(m_mainThread)->interrupt();
}

void QAndroidPlatformIntegration::resumeApp()
{
    if (QAbstractEventDispatcher::instance(m_mainThread))
        QAbstractEventDispatcher::instance(m_mainThread)->wakeUp();
}
QT_END_NAMESPACE
