/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBGLCONTEXT_H
#define QBBGLCONTEXT_H

#include <QtGui/QPlatformGLContext>
#include <QtGui/QPlatformWindowFormat>
#include <QSize>

#include <EGL/egl.h>

QT_BEGIN_NAMESPACE

class QBBWindow;

class QBBGLContext : public QPlatformGLContext
{
public:
    QBBGLContext(QBBWindow* platformWindow);
    virtual ~QBBGLContext();

    static void initialize();
    static void shutdown();

    virtual void makeCurrent();
    virtual void doneCurrent();
    virtual void swapBuffers();
    virtual void* getProcAddress(const QString& procName);

    virtual QPlatformWindowFormat platformWindowFormat() const { return sWindowFormat; }

    void resizeSurface(const QSize &size);
    QSize surfaceSize() const { return mSurfaceSize; }

private:
    static EGLDisplay sEglDisplay;
    static EGLConfig sEglConfig;
    static QPlatformWindowFormat sWindowFormat;

    QBBWindow *mPlatformWindow;
    EGLContext mEglContext;
    EGLSurface mEglSurface;
    QSize mSurfaceSize;

    static EGLint *contextAttrs();
    bool isCurrent() const;
    void createSurface();
    void destroySurface();
};

QT_END_NAMESPACE

#endif // QBBGLCONTEXT_H
