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

#ifdef ANDROID_PLUGIN_OPENGL
#include "qandroideglfsscreen.h"
#include "qandroideglfswindowsurface.h"
#include <private/qpixmapdata_gl_p.h>
#else
#include "qandroidplatformscreen.h"
#endif

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
        Q_UNUSED(script);
//        qDebug() << family << style << script;
        if (styleHint == QFont::Monospace)
                return QStringList() << "Droid Sans Mono";

        return QStringList() << "Droid Sans" << "Droid Sans Fallback";
    }
};

void *QAndroidPlatformNativeInterface::nativeResourceForWidget(const QByteArray &resource, QWidget *widget)
{
    if (!widget && resource=="JavaVM")
        return QtAndroid::javaVM();
    return 0;
};


QAndroidPlatformIntegration::QAndroidPlatformIntegration()
{
    m_androidPlatformNativeInterface =  new QAndroidPlatformNativeInterface();
    QApplication::setStyle("android");
#ifdef ANDROID_PLUGIN_OPENGL
    qDebug() << "QAndroidPlatformIntegration::QAndroidPlatformIntegration():  creating QAndroidEglFSScreen => Using OpenGL painting";
    m_primaryScreen = new QAndroidEglFSScreen(EGL_DEFAULT_DISPLAY);
#else
    qDebug() << "QAndroidPlatformIntegration::QAndroidPlatformIntegration():  creating QAndroidPlatformScreen => Using Raster (Software) for painting";
    m_primaryScreen = new QAndroidPlatformScreen();
#endif

    m_screens.append(m_primaryScreen);

    m_mainThread=QThread::currentThread();
    QtAndroid::setAndroidPlatformIntegration(this);
    qApp->setInputContext( new QAndroidInputContext() );
    m_androidFDB = new QAndroidPlatformFontDatabase();
    m_androidPlatformDesktopService = new QAndroidPlatformDesktopService();

}

bool QAndroidPlatformIntegration::hasCapability(Capability cap) const
{
#ifdef ANDROID_PLUGIN_OPENGL
    return cap==OpenGL;
#else
    return cap==ThreadedPixmaps;
#endif
    return false;
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

QPlatformMenu * QAndroidPlatformIntegration::platformMenu()
{
    return this;
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
#ifdef ANDROID_PLUGIN_OPENGL
    return new QGLPixmapData(type);
#else
    return new QRasterPixmapData(type);
#endif
}

QWindowSurface *QAndroidPlatformIntegration::createWindowSurface(QWidget *widget, WId /*winId*/) const
{
#ifdef ANDROID_PLUGIN_OPENGL
        Q_ASSERT(dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen) != 0);
        return new QAndroidEglFSWindowSurface(dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen), widget);
#else
    return new QFbWindowSurface(dynamic_cast<QFbScreen*>(m_primaryScreen), widget);
#endif
}

QPlatformWindow *QAndroidPlatformIntegration::createPlatformWindow(QWidget *widget, WId /*winId*/) const
{
#ifdef ANDROID_PLUGIN_OPENGL
    Q_ASSERT(dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen) != 0);
    return new QAndroidEglFSWindow(widget, dynamic_cast<QAndroidEglFSScreen*>(m_primaryScreen));
#else
    QFbWindow *w = new QFbWindow(widget);
    dynamic_cast<QFbScreen*>(m_primaryScreen)->addWindow(w);
    qDebug()<<"createPlatformWindow"<<widget->isFullScreen();
    if (widget->isFullScreen())
        QtAndroid::setFullScreen(true);
    else
        QtAndroid::setFullScreen(false);
    return w;
#endif
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

#ifdef ANDROID_PLUGIN_OPENGL
void QAndroidPlatformIntegration::surfaceChanged()
{
    if (m_primaryScreen)
        QMetaObject::invokeMethod(m_primaryScreen, "surfaceChanged", Qt::AutoConnection);
}
#endif

void QAndroidPlatformIntegration::showMenuBar(const QList<QAction*> & menuBarActionList)
{
#warning FIXME Not thread safe !!!
    m_menuBarActionList = menuBarActionList;
    QtAndroid::showOptionsMenu();
}

void QAndroidPlatformIntegration::hideMenuBar()
{
    QtAndroid::hideOptionsMenu();
}

void QAndroidPlatformIntegration::showMenu(const QList<QAction*> & menuActionList)
{
#warning FIXME Not thread safe !!!
    m_menuActionList = menuActionList;
    QtAndroid::showContextMenu();
}

void QAndroidPlatformIntegration::hideMenu()
{
    QtAndroid::hideContextMenu();
}

const QList<QAction*> & QAndroidPlatformIntegration::menuBarActionList()
{
    return m_menuBarActionList;
}

const QList<QAction*> & QAndroidPlatformIntegration::menuActionList()
{
    return m_menuActionList;
}

QT_END_NAMESPACE
