/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSSYSTEM_OPENKODE_H
#define QGRAPHICSSYSTEM_OPENKODE_H

#include "qopenkodeeventloopintegration.h"

#include <QtCore/qsemaphore.h>

#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>
#include <QtGui/QPlatformGLContext>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

struct KDDesktopNV;
class QOpenKODECursor;

class QOpenKODEScreen : public QPlatformScreen
{
    Q_OBJECT
public:
    QOpenKODEScreen();
    ~QOpenKODEScreen() {}

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }

    EGLDisplay eglDisplay() { return mEglDisplay; }

    bool isFullScreen() const {return mIsFullScreen;}
    void setFullScreen(bool fullscreen) { mIsFullScreen = fullscreen; }
private:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    EGLDisplay mEglDisplay;
    bool mIsFullScreen;
};

class QOpenKODEIntegration : public QPlatformIntegration
{
public:
    QOpenKODEIntegration();
    ~QOpenKODEIntegration();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId = 0) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    bool hasOpenGL() const;

    QPlatformEventLoopIntegration *createEventLoopIntegration() const;

    virtual QList<QPlatformScreen *> screens() const { return mScreens; }

    static GLuint blitterProgram();

private:
    QList<QPlatformScreen *> mScreens;
    QOpenKODEEventLoopIntegration *mEventLoopIntegration;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif
