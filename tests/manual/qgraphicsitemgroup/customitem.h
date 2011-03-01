/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QGraphicsItem>
#include <QBrush>
#include <QGraphicsScene>

class CustomGroup : public QGraphicsItemGroup
{
public:
    CustomGroup();
    virtual ~CustomGroup() { }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;
};

class CustomItem : public QGraphicsRectItem
{
public:
    CustomItem(qreal x, qreal y, qreal width, qreal height, const QBrush & brush = QBrush());
    virtual ~CustomItem() { }
};

class CustomScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CustomScene() : QGraphicsScene() { }

    QList<CustomItem*> selectedCustomItems() const;
    QList<CustomGroup*> selectedCustomGroups() const;
};

#endif // CUSTOMITEM_H
