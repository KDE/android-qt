/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSCriptTools module of the Qt Toolkit.
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

#include "qscriptdebuggercommandschedulerjob_p.h"
#include "qscriptdebuggercommandschedulerjob_p_p.h"

QT_BEGIN_NAMESPACE

QScriptDebuggerCommandSchedulerJobPrivate::QScriptDebuggerCommandSchedulerJobPrivate()
{
}

QScriptDebuggerCommandSchedulerJobPrivate::~QScriptDebuggerCommandSchedulerJobPrivate()
{
}

QScriptDebuggerCommandSchedulerJob::QScriptDebuggerCommandSchedulerJob(
    QScriptDebuggerCommandSchedulerInterface *commandScheduler)
    : QScriptDebuggerJob(*new QScriptDebuggerCommandSchedulerJobPrivate)
{
    Q_D(QScriptDebuggerCommandSchedulerJob);
    d->commandScheduler = commandScheduler;
}

QScriptDebuggerCommandSchedulerJob::~QScriptDebuggerCommandSchedulerJob()
{
}

QScriptDebuggerCommandSchedulerJob::QScriptDebuggerCommandSchedulerJob(
    QScriptDebuggerCommandSchedulerJobPrivate &dd,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler)
    : QScriptDebuggerJob(dd)
{
    Q_D(QScriptDebuggerCommandSchedulerJob);
    d->commandScheduler = commandScheduler;
}

QScriptDebuggerCommandSchedulerInterface *QScriptDebuggerCommandSchedulerJob::commandScheduler() const
{
    Q_D(const QScriptDebuggerCommandSchedulerJob);
    return d->commandScheduler;
}

QT_END_NAMESPACE
