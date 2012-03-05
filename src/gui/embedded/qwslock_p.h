/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QWSLOCK_P_H
#define QWSLOCK_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//

#include <qglobal.h>

#ifndef QT_NO_QWS_MULTIPROCESS

#ifdef QT_POSIX_IPC
#  include <semaphore.h>
#endif

QT_BEGIN_NAMESPACE

class QWSLock
{
public:
    enum LockType { BackingStore, Communication, RegionEvent };

    QWSLock(int lockId = -1);
    ~QWSLock();

    bool lock(LockType type, int timeout = -1);
    void unlock(LockType type);
    bool wait(LockType type, int timeout = -1);
    bool hasLock(LockType type);
    int id() const { return semId; }

private:
    bool up(unsigned short semNum);
    bool down(unsigned short semNum, int timeout);
    int getValue(unsigned short semNum) const;

    int semId;
    int lockCount[2];
#ifdef QT_POSIX_IPC
    sem_t *sems[3];
    bool owned;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_QWS_MULTIPROCESS

#endif // QWSLOCK_P_H
