/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "node.h"
#include "stickman.h"

#include <QRectF>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

Node::Node(const QPointF &pos, QGraphicsItem *parent)
  : QGraphicsObject(parent), m_dragging(false)
{
    setPos(pos);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

Node::~Node()
{
}

QRectF Node::boundingRect() const
{
    return QRectF(-6.0, -6.0, 12.0, 12.0);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{    
    painter->setPen(Qt::white);
    painter->drawEllipse(QPointF(0.0, 0.0), 5.0, 5.0);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange)
        emit positionChanged();

    return QGraphicsObject::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    m_dragging = true;
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_dragging)
        setPos(mapToParent(event->pos()));    
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    m_dragging = false;
}
