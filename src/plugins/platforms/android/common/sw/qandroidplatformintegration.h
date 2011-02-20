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

#ifndef QGRAPHICSSYSTEM_MINIMAL_H
#define QGRAPHICSSYSTEM_MINIMAL_H

#include <QPlatformIntegration>
#include "../../fb_base/fb_base.h"
#include <jni.h>

QT_BEGIN_NAMESPACE

class QDesktopWidget;

class QAndroidPlatformScreen : public QFbScreen
{
    Q_OBJECT
public:
    QAndroidPlatformScreen();

public slots:
    QRegion doRedraw();

};

class QAndroidPlatformIntegration : public QPlatformIntegration
{
public:
    QAndroidPlatformIntegration();
    ~QAndroidPlatformIntegration();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId = 0) const;

    QList<QPlatformScreen *> screens() const { return mScreens; }

    QAndroidPlatformScreen * getPrimaryScreen(){return mPrimaryScreen;}
    bool isVirtualDesktop() { return true; }
    virtual void setDesktopSize(int width, int height);
    virtual void setDisplayMetrics(int width, int height);
    QPlatformFontDatabase *fontDatabase() const;

    void pauseApp();
    void resumeApp();

    static void setDefaultDisplayMetrics(int gw, int gh, int sw, int sh);
    static void setDefaultDesktopSize(int gw, int gh);

private:
    QThread * m_mainThread;
    QAndroidPlatformScreen *mPrimaryScreen;
    QList<QPlatformScreen *> mScreens;
    static int mDefaultGeometryWidth,mDefaultGeometryHeight,mDefaultPhysicalSizeWidth,mDefaultPhysicalSizeHeight;
    friend class QAndroidPlatformScreen;
    QPlatformFontDatabase *mAndroidFDB;
    QImage * mFbScreenImage;
    QPainter *compositePainter;

};

QT_END_NAMESPACE

#endif
