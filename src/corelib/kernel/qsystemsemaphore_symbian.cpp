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

#include "qsystemsemaphore.h"
#include "qsystemsemaphore_p.h"
#include "qcoreapplication.h"
#include <qdebug.h>

#include "qcore_symbian_p.h"
#include <e32cmn.h>
QT_BEGIN_NAMESPACE

#ifndef QT_NO_SYSTEMSEMAPHORE

QSystemSemaphorePrivate::QSystemSemaphorePrivate() :
        error(QSystemSemaphore::NoError)
{
}

void QSystemSemaphorePrivate::setErrorString(const QString &function, int err)
{
    if (err == KErrNone){
        return;
    }
    switch(err){
    case KErrAlreadyExists:
        errorString = QCoreApplication::tr("%1: already exists", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::AlreadyExists;
    break;
    case KErrNotFound:
        errorString = QCoreApplication::tr("%1: does not exist", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::NotFound;
    break;
    case KErrNoMemory:
    case KErrInUse:
        errorString = QCoreApplication::tr("%1: out of resources", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::OutOfResources;
        break;
    case KErrPermissionDenied:
        errorString = QCoreApplication::tr("%1: permission denied", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::PermissionDenied;
    break;
default:
    errorString = QCoreApplication::tr("%1: unknown error %2", "QSystemSemaphore").arg(function).arg(err);
    error = QSystemSemaphore::UnknownError;
    }

#if defined QSYSTEMSEMAPHORE_DEBUG
        qDebug() << errorString << "key" << key;
#endif
}

int QSystemSemaphorePrivate::handle(QSystemSemaphore::AccessMode)
{
    if (semaphore.Handle()) {
        return semaphore.Handle();
    }

    // don't allow making handles on empty keys
    if (key.isEmpty())
        return 0;

    TPtrC name(qt_QString2TPtrC(fileName));
    int err = KErrAlreadyExists;
    int tryCount = 10;
    // Sort out race conditions by retrying several times until existing handle is acquired.
    // Sometimes opening can fail inexplicably with KErrPermissionDenied many times in a row.
    while (err != KErrNoMemory && err != KErrNone && tryCount-- >= 0) {
        err = semaphore.CreateGlobal(name, initialValue, EOwnerProcess);
        if (err != KErrNoMemory && err != KErrNone)
            err = semaphore.OpenGlobal(name,EOwnerProcess);
    }
    if (err){
        setErrorString(QLatin1String("QSystemSemaphore::handle"),err);
        return 0;
    }
    return semaphore.Handle();
}

void QSystemSemaphorePrivate::cleanHandle()
{
    semaphore.Close();
}

bool QSystemSemaphorePrivate::modifySemaphore(int count)
{
    if (0 == handle())
        return false;

    if (count > 0) {
        semaphore.Signal(count);
    } else {
        semaphore.Wait();
        }
    return true;
}

#endif //QT_NO_SYSTEMSEMAPHORE

QT_END_NAMESPACE
