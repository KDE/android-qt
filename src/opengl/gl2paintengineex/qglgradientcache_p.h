/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QMultiHash>
#include <QObject>
#include <QtOpenGL/QtOpenGL>
#include <private/qgl_p.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

class QGL2GradientCache
{
    struct CacheInfo
    {
        inline CacheInfo(QGradientStops s, qreal op, QGradient::InterpolationMode mode) :
            stops(s), opacity(op), interpolationMode(mode) {}

        GLuint texId;
        QGradientStops stops;
        qreal opacity;
        QGradient::InterpolationMode interpolationMode;
    };

    typedef QMultiHash<quint64, CacheInfo> QGLGradientColorTableHash;

public:
    static QGL2GradientCache *cacheForContext(const QGLContext *context);

    QGL2GradientCache(const QGLContext *) {}
    ~QGL2GradientCache() { cleanCache(); }

    GLuint getBuffer(const QGradient &gradient, qreal opacity);
    inline int paletteSize() const { return 1024; }

private:
    inline int maxCacheSize() const { return 60; }
    inline void generateGradientColorTable(const QGradient& gradient,
                                           uint *colorTable,
                                           int size, qreal opacity) const;
    GLuint addCacheElement(quint64 hash_val, const QGradient &gradient, qreal opacity);
    void cleanCache();

    QGLGradientColorTableHash cache;
    QMutex m_mutex;
};

QT_END_NAMESPACE

