/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef SUBMARINE_P_H
#define SUBMARINE_P_H

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

//Own
#include "animationmanager.h"
#include "submarine.h"
#include "qanimationstate.h"

//Qt
#include <QtCore/QPropertyAnimation>
#include <QtGui/QGraphicsScene>

//This state is describing when the boat is moving right
class MovementState : public QAnimationState
{
Q_OBJECT
public:
    MovementState(SubMarine *submarine, QState *parent = 0) : QAnimationState(parent)
    {
        movementAnimation = new QPropertyAnimation(submarine, "pos");
        connect(movementAnimation,SIGNAL(valueChanged(const QVariant &)),this,SLOT(onAnimationMovementValueChanged(const QVariant &)));
        setAnimation(movementAnimation);
        AnimationManager::self()->registerAnimation(movementAnimation);
        this->submarine = submarine;
    }

protected slots:
    void onAnimationMovementValueChanged(const QVariant &)
    {
        if (qrand() % 200 + 1 == 3)
            submarine->launchTorpedo(qrand() % 3 + 1);
    }

protected:
    void onEntry(QEvent *e)
    {
        if (submarine->currentDirection() == SubMarine::Left) {
            movementAnimation->setEndValue(QPointF(0,submarine->y()));
            movementAnimation->setDuration(submarine->x()/submarine->currentSpeed()*12);
        }
        else /*if (submarine->currentDirection() == SubMarine::Right)*/ {
            movementAnimation->setEndValue(QPointF(submarine->scene()->width()-submarine->size().width(),submarine->y()));
            movementAnimation->setDuration((submarine->scene()->width()-submarine->size().width()-submarine->x())/submarine->currentSpeed()*12);
        }
        QAnimationState::onEntry(e);
    }

private:
    SubMarine *submarine;
    QPropertyAnimation *movementAnimation;
};

//This state is describing when the boat is moving right
class ReturnState : public QAnimationState
{
public:
    ReturnState(SubMarine *submarine, QState *parent = 0) : QAnimationState(parent)
    {
        returnAnimation = new QPropertyAnimation(submarine->rotation(), "angle");
        returnAnimation->setDuration(500);
        AnimationManager::self()->registerAnimation(returnAnimation);
        setAnimation(returnAnimation);
        this->submarine = submarine;
    }

protected:
    void onEntry(QEvent *e)
    {
        returnAnimation->stop();
        returnAnimation->setEndValue(submarine->currentDirection() == SubMarine::Right ? 360. : 180.);
        QAnimationState::onEntry(e);
    }

    void onExit(QEvent *e)
    {
        submarine->currentDirection() == SubMarine::Right ? submarine->setCurrentDirection(SubMarine::Left) : submarine->setCurrentDirection(SubMarine::Right);
        QAnimationState::onExit(e);
    }

private:
    SubMarine *submarine;
    QPropertyAnimation *returnAnimation;
};

#endif // SUBMARINE_P_H
