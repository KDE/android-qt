/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativeflipable_p.h"

#include "qdeclarativeitem_p.h"

#include <qdeclarativeinfo.h>

#include <QtGui/qgraphicstransform.h>

QT_BEGIN_NAMESPACE

class QDeclarativeFlipablePrivate : public QDeclarativeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeFlipable)
public:
    QDeclarativeFlipablePrivate() : current(QDeclarativeFlipable::Front), front(0), back(0) {}

    void updateSceneTransformFromParent();

    QDeclarativeFlipable::Side current;
    QDeclarativeItem *front;
    QDeclarativeItem *back;
};

/*!
    \qmlclass Flipable QDeclarativeFlipable
    \since 4.7
    \brief The Flipable item provides a surface that can be flipped.
    \inherits Item

    Flipable allows you to specify a front and a back and then flip between those sides.

    Here's an example that flips between the front and back sides when clicked:

    \qml

    Flipable {
        id: flipable
        width: 250; height: 250
        property int angle: 0

        transform: Rotation {
            id: rotation
            origin.x: flipable.width/2; origin.y: flipable.height/2
            axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
            angle: flipable.angle
        }

        front: Image { source: "front.png" }
        back: Image { source: "back.png" }

        states: State {
            name: "back"
            PropertyChanges { target: flipable; angle: 180 }
        }

        transitions: Transition {
            NumberAnimation { properties: "angle"; duration: 2000 }
        }

        MouseArea {
            // change between default and 'back' states
            onClicked: flipable.state = (flipable.state == 'back' ? '' : 'back')
            anchors.fill: parent
        }
    }
    \endqml

    \image flipable.gif
*/

/*!
    \internal
    \class QDeclarativeFlipable
    \brief The QDeclarativeFlipable class provides a flipable surface.

    \ingroup group_widgets

    QDeclarativeFlipable allows you to specify a front and a back, as well as an
    axis for the flip.
*/

QDeclarativeFlipable::QDeclarativeFlipable(QDeclarativeItem *parent)
: QDeclarativeItem(*(new QDeclarativeFlipablePrivate), parent)
{
}

QDeclarativeFlipable::~QDeclarativeFlipable()
{
}

/*!
  \qmlproperty Item Flipable::front
  \qmlproperty Item Flipable::back

  The front and back sides of the flipable.
*/

QDeclarativeItem *QDeclarativeFlipable::front()
{
    Q_D(const QDeclarativeFlipable);
    return d->front;
}

void QDeclarativeFlipable::setFront(QDeclarativeItem *front)
{
    Q_D(QDeclarativeFlipable);
    if (d->front) {
        qmlInfo(this) << tr("front is a write-once property");
        return;
    }
    d->front = front;
    d->front->setParentItem(this);
    if (Back == d->current)
        d->front->setOpacity(0.);
}

QDeclarativeItem *QDeclarativeFlipable::back()
{
    Q_D(const QDeclarativeFlipable);
    return d->back;
}

void QDeclarativeFlipable::setBack(QDeclarativeItem *back)
{
    Q_D(QDeclarativeFlipable);
    if (d->back) {
        qmlInfo(this) << tr("back is a write-once property");
        return;
    }
    d->back = back;
    d->back->setParentItem(this);
    if (Front == d->current)
        d->back->setOpacity(0.);
}

/*!
  \qmlproperty enumeration Flipable::side

  The side of the Flippable currently visible. Possible values are \c
  Front and \c Back.
*/
QDeclarativeFlipable::Side QDeclarativeFlipable::side() const
{
    Q_D(const QDeclarativeFlipable);
    if (d->dirtySceneTransform)
        const_cast<QDeclarativeFlipablePrivate *>(d)->updateSceneTransformFromParent();

    return d->current;
}

// determination on the currently visible side of the flipable
// has to be done on the complete scene transform to give
// correct results.
void QDeclarativeFlipablePrivate::updateSceneTransformFromParent()
{
    Q_Q(QDeclarativeFlipable);

    QDeclarativeItemPrivate::updateSceneTransformFromParent();
    QPointF p1(0, 0);
    QPointF p2(1, 0);
    QPointF p3(1, 1);

    p1 = sceneTransform.map(p1);
    p2 = sceneTransform.map(p2);
    p3 = sceneTransform.map(p3);

    qreal cross = (p1.x() - p2.x()) * (p3.y() - p2.y()) -
                  (p1.y() - p2.y()) * (p3.x() - p2.x());

    QDeclarativeFlipable::Side newSide;
    if (cross > 0) {
       newSide = QDeclarativeFlipable::Back;
    } else {
        newSide = QDeclarativeFlipable::Front;
    }

    if (newSide != current) {
        current = newSide;
        if (current == QDeclarativeFlipable::Back) {
            QTransform mat;
            mat.translate(back->width()/2,back->height()/2);
            if (back->width() && p1.x() >= p2.x())
                mat.rotate(180, Qt::YAxis);
            if (back->height() && p2.y() >= p3.y())
                mat.rotate(180, Qt::XAxis);
            mat.translate(-back->width()/2,-back->height()/2);
            back->setTransform(mat);
        }
        if (front)
            front->setOpacity((current==QDeclarativeFlipable::Front)?1.:0.);
        if (back)
            back->setOpacity((current==QDeclarativeFlipable::Back)?1.:0.);
        emit q->sideChanged();
    }
}

QT_END_NAMESPACE
