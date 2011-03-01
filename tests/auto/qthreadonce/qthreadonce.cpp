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


#include "qplatformdefs.h"
#include "qthreadonce.h"

#ifndef QT_NO_THREAD
#include "qmutex.h"

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, onceInitializationMutex, (QMutex::Recursive))

enum QOnceExtra {
    MustRunCode = 0x01,
    MustUnlockMutex = 0x02
};

/*!
    \internal
    Initialize the Q_ONCE structure.

    Q_ONCE consists of two variables:
     - a static POD QOnceControl::ControlVariable (it's a QBasicAtomicInt)
     - an automatic QOnceControl that controls the former

    The POD is initialized to 0.

    When QOnceControl's constructor starts, it'll lock the global
    initialization mutex. It'll then check if it's the first to up
    the control variable and will take note.

    The QOnceControl's destructor will unlock the global
    initialization mutex.
*/
QOnceControl::QOnceControl(QBasicAtomicInt *control)
{
    d = 0;
    gv = control;
    // check if code has already run once
    if (*gv == 2) {
        // uncontended case: it has already initialized
        // no waiting
        return;
    }

    // acquire the path
    onceInitializationMutex()->lock();
    extra = MustUnlockMutex;

    if (gv->testAndSetAcquire(0, 1)) {
        // path acquired, we're the first
        extra |= MustRunCode;
    }
}

QOnceControl::~QOnceControl()
{
    if (mustRunCode())
        // code wasn't run!
        gv->testAndSetRelease(1, 0);
    else
        gv->testAndSetRelease(1, 2);
    if (extra & MustUnlockMutex)
        onceInitializationMutex()->unlock();
}

/*!
    \internal
    Returns true if the initialization code must be run.

    Obviously, the initialization code must be run only once...
*/
bool QOnceControl::mustRunCode()
{
    return extra & MustRunCode;
}

void QOnceControl::done()
{
    extra &= ~MustRunCode;
}

#endif // QT_NO_THREAD
