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

#include "qanimationstate.h"

#include <QtCore/qstate.h>

QT_BEGIN_NAMESPACE

/*!
\class QAnimationState

\brief The QAnimationState class provides state that handle an animation and emit
a signal when this animation is finished.

\ingroup statemachine

QAnimationState provides a state that handle an animation. It will start this animation
when the state is entered and stop it when it is leaved. When the animation has finished the
state emit animationFinished signal.
QAnimationState is part of \l{The State Machine Framework}.

\code
QStateMachine machine;
QAnimationState *s = new QAnimationState(machine->rootState());
QPropertyAnimation *animation = new QPropertyAnimation(obj, "pos");
s->setAnimation(animation);
QState *s2 = new QState(machine->rootState());
s->addTransition(s, SIGNAL(animationFinished()), s2);
machine.start();
\endcode

\sa QState, {The Animation Framework}
*/


#ifndef QT_NO_ANIMATION

/*!
  Constructs a new state with the given \a parent state.
*/
QAnimationState::QAnimationState(QState *parent)
    : QState(parent), m_animation(0)
{
}

/*!
  Destroys the animation state.
*/
QAnimationState::~QAnimationState()
{
}

/*!
  Set an \a animation for this QAnimationState. If an animation was previously handle by this
  state then it won't emit animationFinished for the old animation. The QAnimationState doesn't
  take the ownership of the animation.
*/
void QAnimationState::setAnimation(QAbstractAnimation *animation)
{
    if (animation == m_animation)
        return;

    //Disconnect from the previous animation if exist
    if(m_animation)
        disconnect(m_animation, SIGNAL(finished()), this, SIGNAL(animationFinished()));

    m_animation = animation;

    if (m_animation) {
        //connect the new animation
        connect(m_animation, SIGNAL(finished()), this, SIGNAL(animationFinished()));
    }
}

/*!
  Returns the animation handle by this animation state, or 0 if there is no animation.
*/
QAbstractAnimation* QAnimationState::animation() const
{
    return m_animation;
}

/*!
  \reimp
*/
void QAnimationState::onEntry(QEvent *)
{
    if (m_animation)
        m_animation->start();
}

/*!
  \reimp
*/
void QAnimationState::onExit(QEvent *)
{
    if (m_animation)
        m_animation->stop();
}

/*!
  \reimp
*/
bool QAnimationState::event(QEvent *e)
{
    return QState::event(e);
}

QT_END_NAMESPACE

#endif
