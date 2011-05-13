/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QBLITTABLE_P_H
#define QBLITTABLE_P_H

#include <QtCore/qsize.h>
#include <QtGui/private/qpixmap_blitter_p.h>


#ifndef QT_NO_BLITTABLE
QT_BEGIN_NAMESPACE

class QImage;
class QBlittablePrivate;

class Q_GUI_EXPORT QBlittable
{
    Q_DECLARE_PRIVATE(QBlittable);
public:
    enum Capability {

        SolidRectCapability             = 0x0001,
        SourcePixmapCapability          = 0x0002,
        SourceOverPixmapCapability      = 0x0004,
        SourceOverScaledPixmapCapability = 0x0008,

        // Internal ones
        OutlineCapability               = 0x0001000,
    };
    Q_DECLARE_FLAGS (Capabilities, Capability);

    QBlittable(const QSize &size, Capabilities caps);
    virtual ~QBlittable();

    Capabilities capabilities() const;
    QSize size() const;

    virtual void fillRect(const QRectF &rect, const QColor &color) = 0;
    virtual void drawPixmap(const QRectF &rect, const QPixmap &pixmap, const QRectF &subrect) = 0;

    QImage *lock();
    void unlock();

protected:
    virtual QImage *doLock() = 0;
    virtual void doUnlock() = 0;
    QBlittablePrivate *d_ptr;
};

QT_END_NAMESPACE
#endif //QT_NO_BLITTABLE
#endif //QBLITTABLE_P_H
