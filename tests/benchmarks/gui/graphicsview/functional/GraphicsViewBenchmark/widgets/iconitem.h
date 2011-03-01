/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef ICONITEM_H
#define ICONITEM_H

#include <QPainter>

#include "gvbwidget.h"

#if (QT_VERSION >= 0x040600)
class QGraphicsOpacityEffect;
#endif
class QPainter;

class IconItem : public GvbWidget
{
    Q_OBJECT

public:

    IconItem(const QString &filename = "", QGraphicsItem *parent = 0);

    virtual ~IconItem();

    QString fileName() const;
    void setFileName(const QString &filename);

#if (QT_VERSION >= 0x040600)
    void setOpacityEffectEnabled(const bool enable);
    bool isOpacityEffectEnabled() const;
#endif
    void setRotation(const qreal rotation) { m_rotation = rotation; }
    qreal rotation() const { return m_rotation; }

    void setSmoothTransformationEnabled(const bool enable) { m_smoothTransformation = enable; }
    bool isSmoothTransformationEnabled() const { return m_smoothTransformation; }

private:

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget */*widget = 0*/);
    QSizeF sizeHint(Qt::SizeHint which, 
        const QSizeF &constraint = QSizeF()) const;

private:
    Q_DISABLE_COPY(IconItem)
    void reload();

    QString m_filename;
    QPixmap m_pixmap;
    qreal m_rotation;
#if (QT_VERSION >= 0x040600)
    QGraphicsOpacityEffect *m_opacityEffect;
#endif
    bool m_smoothTransformation;
};

#endif
