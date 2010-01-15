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

/*!
    \class QPauseAnimation
    \brief The QPauseAnimation class provides a pause for QSequentialAnimationGroup.
    \since 4.6
    \ingroup animation

    If you wish to introduce a delay between animations in a
    QSequentialAnimationGroup, you can insert a QPauseAnimation. This
    class does not animate anything, but does not
    \l{QAbstractAnimation::finished()}{finish} before a specified
    number of milliseconds have elapsed from when it was started. You
    specify the duration of the pause in the constructor. It can also
    be set directly with setDuration().

    It is not necessary to construct a QPauseAnimation yourself.
    QSequentialAnimationGroup provides the convenience functions
    \l{QSequentialAnimationGroup::}{addPause()} and
    \l{QSequentialAnimationGroup::}{insertPause()}. These functions
    simply take the number of milliseconds the pause should last.

    \sa QSequentialAnimationGroup
*/

#include "qpauseanimation.h"
#include "qabstractanimation_p.h"


#ifndef QT_NO_ANIMATION

QT_BEGIN_NAMESPACE

class QPauseAnimationPrivate : public QAbstractAnimationPrivate
{
public:
    QPauseAnimationPrivate() : QAbstractAnimationPrivate(), duration(250)
    {
        isPause = true;
    }

    int duration;
};

/*!
    Constructs a QPauseAnimation.
    \a parent is passed to QObject's constructor.
    The default duration is 0.
*/

QPauseAnimation::QPauseAnimation(QObject *parent) : QAbstractAnimation(*new QPauseAnimationPrivate, parent)
{
}

/*!
    Constructs a QPauseAnimation.
    \a msecs is the duration of the pause.
    \a parent is passed to QObject's constructor.
*/

QPauseAnimation::QPauseAnimation(int msecs, QObject *parent) : QAbstractAnimation(*new QPauseAnimationPrivate, parent)
{
    setDuration(msecs);
}

/*!
    Destroys the pause animation.
*/
QPauseAnimation::~QPauseAnimation()
{
}

/*!
    \property QPauseAnimation::duration
    \brief the duration of the pause.

    The duration of the pause. The duration should not be negative.
    The default duration is 250 milliseconds.
*/
int QPauseAnimation::duration() const
{
    Q_D(const QPauseAnimation);
    return d->duration;
}

void QPauseAnimation::setDuration(int msecs)
{
    if (msecs < 0) {
        qWarning("QPauseAnimation::setDuration: cannot set a negative duration");
        return;
    }
    Q_D(QPauseAnimation);
    d->duration = msecs;
}

/*!
    \reimp
 */
bool QPauseAnimation::event(QEvent *e)
{
    return QAbstractAnimation::event(e);
}

/*!
    \reimp
 */
void QPauseAnimation::updateCurrentTime(int)
{
}


QT_END_NAMESPACE

#include "moc_qpauseanimation.cpp"

#endif //QT_NO_ANIMATION
