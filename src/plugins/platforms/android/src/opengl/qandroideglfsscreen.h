/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QANDROIDEGLSCREEN_H
#define QANDROIDEGLSCREEN_H

#include <QPlatformScreen>


#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "qandroideglplatformcontext.h"

QT_BEGIN_NAMESPACE

class QPlatformGLContext;

class QAndroidEglFSScreen : public QPlatformScreen
{
    Q_OBJECT

public:
    QAndroidEglFSScreen(EGLNativeDisplayType display);
    ~QAndroidEglFSScreen() {}

    QRect geometry() const;
    int depth() const;
    QSize physicalSize() const;
    QImage::Format format() const;

    QAndroidEglFSPlatformContext *platformContext() const;

    void removeWindow(QPlatformWindow * window);
    void addWindow(QPlatformWindow * window);
    void raise(QPlatformWindow * window);
    void lower(QPlatformWindow * window);
    QWidget * topWindow();

public slots:
    void surfaceChanged();
    void updateTLWindows();
    virtual void setGeometry(QRect rect);
    virtual void setPhysicalSize(QSize size);

protected:

private:
    void createWindowSurface();
    void createAndSetPlatformContext() const;
    void createAndSetPlatformContext();

    QRect mGeometry;
    QSize mPhysicalSize;

    int m_depth;
    QImage::Format m_format;
    QAndroidEglFSPlatformContext *m_platformContext;
    EGLDisplay m_display;
    EGLSurface m_windowSurface;
    EGLConfig  m_config;
    QList<QPlatformWindow *> windowStack;

};

QT_END_NAMESPACE
#endif // QANDROIDEGLSCREEN_H
