/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q_GLX_CONTEXT_H
#define Q_GLX_CONTEXT_H

#include "qxlibwindow.h"

#include <QtGui/QPlatformGLContext>
#include <QtGui/QPlatformWindowFormat>

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)
#define Status int
#include <GL/glx.h>
#undef Status

QT_BEGIN_NAMESPACE

class QGLXContext : public QPlatformGLContext
{
public:
    QGLXContext(Window window, QXlibScreen *xd, const QPlatformWindowFormat &format);
    ~QGLXContext();

    virtual void makeCurrent();
    virtual void doneCurrent();
    virtual void swapBuffers();
    virtual void* getProcAddress(const QString& procName);

    GLXContext glxContext() const {return m_context;}

    QPlatformWindowFormat platformWindowFormat() const;

private:
    QXlibScreen  *m_screen;
    Drawable    m_drawable;
    GLXContext  m_context;
    QPlatformWindowFormat m_windowFormat;

    QGLXContext (QXlibScreen *screen, Drawable drawable, GLXContext context);
};

QT_END_NAMESPACE

#endif //!defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)

#endif
