/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QGLFRAMEBUFFEROBJECT_P_H
#define QGLFRAMEBUFFEROBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

QT_BEGIN_INCLUDE_NAMESPACE

#include <qglframebufferobject.h>
#include <private/qglpaintdevice_p.h>
#include <private/qgl_p.h>

QT_END_INCLUDE_NAMESPACE

#ifndef QT_OPENGL_ES
#define DEFAULT_FORMAT GL_RGBA8
#else
#define DEFAULT_FORMAT GL_RGBA
#endif

class QGLFramebufferObjectFormatPrivate
{
public:
    QGLFramebufferObjectFormatPrivate()
        : ref(1),
          samples(0),
          attachment(QGLFramebufferObject::NoAttachment),
          target(GL_TEXTURE_2D),
          internal_format(DEFAULT_FORMAT)
    {
    }
    QGLFramebufferObjectFormatPrivate
            (const QGLFramebufferObjectFormatPrivate *other)
        : ref(1),
          samples(other->samples),
          attachment(other->attachment),
          target(other->target),
          internal_format(other->internal_format)
    {
    }
    bool equals(const QGLFramebufferObjectFormatPrivate *other)
    {
        return samples == other->samples &&
               attachment == other->attachment &&
               target == other->target &&
               internal_format == other->internal_format;
    }

    QAtomicInt ref;
    int samples;
    QGLFramebufferObject::Attachment attachment;
    GLenum target;
    GLenum internal_format;
};

class QGLFBOGLPaintDevice : public QGLPaintDevice
{
public:
    virtual QPaintEngine* paintEngine() const {return fbo->paintEngine();}
    virtual QSize size() const {return fbo->size();}
    virtual QGLContext* context() const;
    virtual QGLFormat format() const {return fboFormat;}

    void setFBO(QGLFramebufferObject* f,
                QGLFramebufferObject::Attachment attachment);

private:
    bool wasBound;
    QGLFramebufferObject* fbo;
    QGLFormat fboFormat;
};

class QGLFramebufferObjectPrivate
{
public:
    QGLFramebufferObjectPrivate() : fbo_guard(0), texture(0), depth_stencil_buffer(0)
                                  , color_buffer(0), valid(false), engine(0) {}
    ~QGLFramebufferObjectPrivate() {}

    void init(QGLFramebufferObject *q, const QSize& sz,
              QGLFramebufferObject::Attachment attachment,
              GLenum internal_format, GLenum texture_target, GLint samples = 0);
    bool checkFramebufferStatus() const;
    QGLSharedResourceGuard fbo_guard;
    GLuint texture;
    GLuint depth_stencil_buffer;
    GLuint color_buffer;
    GLenum target;
    QSize size;
    QGLFramebufferObjectFormat format;
    uint valid : 1;
    QGLFramebufferObject::Attachment fbo_attachment;
    mutable QPaintEngine *engine;
    QGLFBOGLPaintDevice glDevice;

    inline GLuint fbo() const { return fbo_guard.id(); }
};


QT_END_NAMESPACE

#endif // QGLFRAMEBUFFEROBJECT_P_H
