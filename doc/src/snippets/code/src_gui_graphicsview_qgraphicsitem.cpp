/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
class SimpleItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const
    {
        qreal penWidth = 1;
        return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                      20 + penWidth, 20 + penWidth);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
    }
};
//! [0]


//! [1]
class CustomItem : public QGraphicsItem
{
   ...
   enum { Type = UserType + 1 };

   int type() const
   {
       // Enable the use of qgraphicsitem_cast with this item.
       return Type;
   }
   ...
};
//! [1]


//! [2]
item->setCursor(Qt::IBeamCursor);
//! [2]


//! [3]
item->setCursor(Qt::IBeamCursor);
//! [3]


//! [4]
QGraphicsRectItem rect;
rect.setPos(100, 100);

rect.sceneTransform().map(QPointF(0, 0));
// returns QPointF(100, 100);

rect.sceneTransform().inverted().map(QPointF(100, 100));
// returns QPointF(0, 0);
//! [4]


//! [5]
QGraphicsRectItem rect;
rect.setPos(100, 100);

rect.deviceTransform(view->viewportTransform()).map(QPointF(0, 0));
// returns the item's (0, 0) point in view's viewport coordinates

rect.deviceTransform(view->viewportTransform()).inverted().map(QPointF(100, 100));
// returns view's viewport's (100, 100) coordinate in item coordinates
//! [5]


//! [6]
// Rotate an item 45 degrees around (0, 0).
item->rotate(45);

// Rotate an item 45 degrees around (x, y).
item->setTransform(QTransform().translate(x, y).rotate(45).translate(-x, -y));
//! [6]


//! [7]
// Scale an item by 3x2 from its origin
item->scale(3, 2);

// Scale an item by 3x2 from (x, y)
item->setTransform(QTransform().translate(x, y).scale(3, 2).translate(-x, -y));
//! [7]


//! [8]
QRectF CircleItem::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-radius - penWidth / 2, -radius - penWidth / 2,
                  diameter + penWidth, diameter + penWidth);
}
//! [8]


//! [9]
QPainterPath RoundItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}
//! [9]


//! [10]
void RoundRectItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
}
//! [10]


//! [11]
static const int ObjectName = 0;

QGraphicsItem *item = scene.itemAt(100, 50);
if (item->data(ObjectName).toString().isEmpty()) {
    if (qgraphicsitem_cast<ButtonItem *>(item))
        item->setData(ObjectName, "Button");
}
//! [11]


//! [12]
QGraphicsScene scene;
QGraphicsEllipseItem *ellipse = scene.addEllipse(QRectF(-10, -10, 20, 20));
QGraphicsLineItem *line = scene.addLine(QLineF(-10, -10, 20, 20));

line->installSceneEventFilter(ellipse);
// line's events are filtered by ellipse's sceneEventFilter() function.

ellipse->installSceneEventFilter(line);
// ellipse's events are filtered by line's sceneEventFilter() function.
//! [12]


//! [13]
void CustomItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *removeAction = menu.addAction("Remove");
    QAction *markAction = menu.addAction("Mark");
    QAction *selectedAction = menu.exec(event->screenPos());
    // ...
}
//! [13]


//! [14]
CustomItem::CustomItem()
{
    setAcceptDrops(true);
    ...
}

void CustomItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(event->mimeData()->hasFormat("text/plain"));
}
//! [14]


//! [15]
QVariant Component::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
//! [15]


//! [16]
void CircleItem::setRadius(qreal newRadius)
{
    if (radius != newRadius) {
        prepareGeometryChange();
        radius = newRadius;
    }
}
//! [16]


//! [17]
// Group all selected items together
QGraphicsItemGroup *group = scene->createItemGroup(scene->selecteditems());

// Destroy the group, and delete the group item
scene->destroyItemGroup(group);
//! [17]


//! [QGraphicsItem type]
class CustomItem : public QGraphicsItem
{
   ...
   enum { Type = UserType + 1 };

   int type() const
   {
       // Enable the use of qgraphicsitem_cast with this item.
       return Type;
   }
   ...
};
//! [QGraphicsItem type]
