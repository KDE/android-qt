/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QGLPIXMAPFILTER_P_H
#define QGLPIXMAPFILTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qpixmapfilter_p.h>

#include <QtOpenGL/qgl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLPixelBuffer;

QT_MODULE(OpenGL)

class QGLPixmapFilterBase
{
public:
    virtual ~QGLPixmapFilterBase() {}
protected:
    void bindTexture(const QPixmap &src) const;
    void drawImpl(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect = QRectF()) const;

    virtual bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const = 0;
};

template <typename Filter>
class QGLPixmapFilter : public Filter, public QGLPixmapFilterBase
{
public:
    void draw(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect = QRectF())  const {
        const QRectF source = srcRect.isNull() ? QRectF(src.rect()) : srcRect;
        if (painter)
           drawImpl(painter, pos, src, source);
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLPIXMAPFILTER_P_H
