/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMUTEXPOOL_P_H
#define QMUTEXPOOL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QSettings. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qatomic.h"
#include "QtCore/qmutex.h"
#include "QtCore/qvarlengtharray.h"

#ifndef QT_NO_THREAD

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QMutexPool
{
public:
    explicit QMutexPool(QMutex::RecursionMode recursionMode = QMutex::NonRecursive, int size = 131);
    ~QMutexPool();

    QMutex *get(const void *address);
    static QMutexPool *instance();
    static QMutex *globalInstanceGet(const void *address);

private:
    QVarLengthArray<QAtomicPointer<QMutex>, 131> mutexes;
    QMutex::RecursionMode recursionMode;
};

extern Q_CORE_EXPORT QMutexPool *qt_global_mutexpool;

QT_END_NAMESPACE

#endif // QT_NO_THREAD

#endif // QMUTEXPOOL_P_H
