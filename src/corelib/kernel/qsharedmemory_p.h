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

#ifndef QSHAREDMEMORY_P_H
#define QSHAREDMEMORY_P_H

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

#include "qsharedmemory.h"

#ifdef QT_NO_SHAREDMEMORY
# ifndef QT_NO_SYSTEMSEMAPHORE
namespace QSharedMemoryPrivate
{
    int createUnixKeyFile(const QString &fileName);
    QString makePlatformSafeKey(const QString &key,
            const QString &prefix = QLatin1String("qipc_sharedmemory_"));
}
#endif
#else

#include "qsystemsemaphore.h"
#include "private/qobject_p.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#elif defined(Q_OS_SYMBIAN)
#include <e32std.h>
#include <sys/types.h>
#elif defined(Q_OS_ANDROID)
#include <semaphore.h>
typedef int key_t;
#else
#include <sys/sem.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SYSTEMSEMAPHORE
/*!
  Helper class
  */
class QSharedMemoryLocker
{

public:
    inline QSharedMemoryLocker(QSharedMemory *sharedMemory) : q_sm(sharedMemory)
    {
        Q_ASSERT(q_sm);
    }

    inline ~QSharedMemoryLocker()
    {
        if (q_sm)
            q_sm->unlock();
    }

    inline bool lock()
    {
        if (q_sm && q_sm->lock())
            return true;
        q_sm = 0;
        return false;
    }

private:
    QSharedMemory *q_sm;
};
#endif // QT_NO_SYSTEMSEMAPHORE

class Q_AUTOTEST_EXPORT QSharedMemoryPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSharedMemory)

public:
    QSharedMemoryPrivate();

    void *memory;
    int size;
    QString key;
    QSharedMemory::SharedMemoryError error;
    QString errorString;
#ifndef QT_NO_SYSTEMSEMAPHORE
    QSystemSemaphore systemSemaphore;
    bool lockedByMe;
#endif

#ifndef Q_OS_ANDROID
    static int createUnixKeyFile(const QString &fileName);
#endif
    static QString makePlatformSafeKey(const QString &key,
            const QString &prefix = QLatin1String("qipc_sharedmemory_"));
#ifndef Q_OS_ANDROID
#ifdef Q_OS_WIN
    HANDLE handle();
#else
    key_t handle();
#endif
#endif
    bool initKey();
    bool cleanHandle();
    bool create(int size);
    bool attach(QSharedMemory::AccessMode mode);
    bool detach();

#ifdef Q_OS_SYMBIAN
    void setErrorString(const QString &function, TInt errorCode);
#else
    void setErrorString(const QString &function);
#endif

#ifndef QT_NO_SYSTEMSEMAPHORE
    bool tryLocker(QSharedMemoryLocker *locker, const QString function) {
        if (!locker->lock()) {
            errorString = QSharedMemory::tr("%1: unable to lock").arg(function);
            error = QSharedMemory::LockError;
            return false;
        }
        return true;
    }
#endif // QT_NO_SYSTEMSEMAPHORE

private:
#ifdef Q_OS_WIN
    HANDLE hand;
#elif defined(Q_OS_SYMBIAN)
    RChunk chunk;
#elif defined(Q_OS_ANDROID)
    int ashmem_fd;
    int old_prot;
#else
    key_t unix_key;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_SHAREDMEMORY

#endif // QSHAREDMEMORY_P_H

