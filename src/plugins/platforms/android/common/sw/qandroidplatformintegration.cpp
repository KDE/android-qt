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

    THIS SOFTWARE IS PROVIDED BY BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL BogDan Vatra <bog_dan_ro@yahoo.com> BE LIABLE FOR ANY
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
#include "qandroidplatformdesktopservice.h"

QT_BEGIN_NAMESPACE

int QAndroidPlatformIntegration::m_defaultGeometryWidth=320;
int QAndroidPlatformIntegration::m_defaultGeometryHeight=455;
int QAndroidPlatformIntegration::m_defaultPhysicalSizeWidth=50;
int QAndroidPlatformIntegration::m_defaultPhysicalSizeHeight=71;

class QAndroidPlatformFontDatabase: public QBasicUnixFontDatabase
{
public:
    QString fontDir() const {
        return QLatin1String("/system/fonts");
    }

    void populateFontDatabase() {
        QPlatformFontDatabase::populateFontDatabase();
        QString fontpath = fontDir();

        if(!QFile::exists(fontpath)) {
            qFatal("QFontDatabase: Cannot find font directory %s - is Qt installed correctly?",
                   qPrintable(fontpath));
        }

        QDir dir(fontpath, QLatin1String("*.ttf"));
        for (int i = 0; i < int(dir.count()); ++i) {
            const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
            addTTFile(QByteArray(), file);
        }
    }

    QStringList fallbacksForFamily(const QString family, const QFont::Style &style, const QFont::StyleHint &styleHint, const QUnicodeTables::Script &script) const {
        Q_UNUSED(family);
        Q_UNUSED(style);
        Q_UNUSED(styleHint);
        Q_UNUSED(script);
        return QStringList() << "Droid Sans Fallback";
    }
};

QAndroidPlatformScreen::QAndroidPlatformScreen():QFbScreen()
{
    mGeometry = QRect(0, 0, QAndroidPlatformIntegration::m_defaultGeometryWidth, QAndroidPlatformIntegration::m_defaultGeometryHeight);
    mFormat = QImage::Format_RGB16;
    mDepth = 16;
    setPhysicalSize(QSize(QAndroidPlatformIntegration::m_defaultPhysicalSizeWidth,
                          QAndroidPlatformIntegration::m_defaultPhysicalSizeHeight));
    setGeometry(QRect(0,0,QAndroidPlatformIntegration::m_defaultGeometryWidth
                      ,QAndroidPlatformIntegration::m_defaultGeometryHeight));
    setFormat(mFormat);
    qDebug()<<"QAndroidPlatformScreen::QAndroidPlatformScreen():QFbScreen()";
}

QRegion QAndroidPlatformScreen::doRedraw()
{
    QRegion touched;
    touched = QFbScreen::doRedraw();
    if (touched.isEmpty())
        return touched;
//    QVector<QRect> rects = touched.rects();
//    for (int i = 0; i < rects.size(); i++)
    QtAndroid::flushImage(mGeometry.topLeft(), *mScreenImage, touched.boundingRect());
    return touched;
}

void *QAndroidPlatformNativeInterface::nativeResourceForWidget(const QByteArray &resource, QWidget *widget)
{
    if (!widget && resource=="JavaVM")
        return QtAndroid::javaVM();
    return 0;
};

QAndroidPlatformIntegration::QAndroidPlatformIntegration()
{
    m_androidPlatformNativeInterface =  new QAndroidPlatformNativeInterface();
    m_primaryScreen = new QAndroidPlatformScreen();
    m_screens.append(m_primaryScreen);
    m_mainThread=QThread::currentThread();
    QtAndroid::setAndroidPlatformIntegration(this);
    qApp->setInputContext( new QAndroidInputContext() );
    m_androidFDB = new QAndroidPlatformFontDatabase();
    m_androidPlatformDesktopService = new QAndroidPlatformDesktopService();
}

QAndroidPlatformIntegration::~QAndroidPlatformIntegration()
{
    delete m_androidPlatformNativeInterface;
    delete m_androidFDB;
    QtAndroid::setAndroidPlatformIntegration(NULL);
}

QPlatformFontDatabase *QAndroidPlatformIntegration::fontDatabase() const
{
    return m_androidFDB;
}

QPlatformNativeInterface *QAndroidPlatformIntegration::nativeInterface() const
{
    return m_androidPlatformNativeInterface;
}

QPlatformDesktopService * QAndroidPlatformIntegration::platformDesktopService()
{
    return m_androidPlatformDesktopService;
}

void QAndroidPlatformIntegration::setDefaultDisplayMetrics(int gw, int gh, int sw, int sh)
{
    m_defaultGeometryWidth=gw;
    m_defaultGeometryHeight=gh;
    m_defaultPhysicalSizeWidth=sw;
    m_defaultPhysicalSizeHeight=sh;
}

void QAndroidPlatformIntegration::setDefaultDesktopSize(int gw, int gh)
{
    m_defaultGeometryWidth=gw;
    m_defaultGeometryHeight=gh;
}

QPixmapData *QAndroidPlatformIntegration::createPixmapData(QPixmapData::PixelType type) const
{
     return new QRasterPixmapData(type);
}

QWindowSurface *QAndroidPlatformIntegration::createWindowSurface(QWidget *widget, WId /*winId*/) const
{
    return new QFbWindowSurface(m_primaryScreen, widget);
}

QPlatformWindow *QAndroidPlatformIntegration::createPlatformWindow(QWidget *widget, WId /*winId*/) const
{
    QFbWindow *w = new QFbWindow(widget);
    m_primaryScreen->addWindow(w);
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
    if (m_primaryScreen)
        QMetaObject::invokeMethod(m_primaryScreen, "setGeometry", Qt::AutoConnection, Q_ARG(QRect, QRect(0,0,width, height)));
    qDebug()<<"setDesktopSize done";
}

void QAndroidPlatformIntegration::setDisplayMetrics(int width, int height)
{
    qDebug()<<"setDisplayMetrics";
    if (m_primaryScreen)
        QMetaObject::invokeMethod(m_primaryScreen, "setPhysicalSize", Qt::AutoConnection, Q_ARG(QSize, QSize(width, height)));
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
