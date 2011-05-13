/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QWAYLANDREADBACKEGLGLCONTEXT_H
#define QWAYLANDREADBACKEGLGLCONTEXT_H

#include <QPlatformGLContext>
#include <QtGui/QWidget>

#include "qwaylandreadbackeglintegration.h"
#include "qwaylandreadbackeglwindow.h"

class QWaylandShmBuffer;

class QWaylandReadbackEglContext : public QPlatformGLContext
{
public:
    QWaylandReadbackEglContext(QWaylandReadbackEglIntegration *eglIntegration, QWaylandReadbackEglWindow *window);
    ~QWaylandReadbackEglContext();

    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString& procName);

    virtual QPlatformWindowFormat platformWindowFormat() const;

    void geometryChanged();

private:
    QWaylandReadbackEglIntegration *mEglIntegration;
    QWaylandReadbackEglWindow *mWindow;
    QWaylandShmBuffer *mBuffer;

    Pixmap mPixmap;

    EGLConfig mConfig;
    EGLContext mContext;
    EGLSurface mPixmapSurface;
};

#endif // QWAYLANDREADBACKEGLGLCONTEXT_H
