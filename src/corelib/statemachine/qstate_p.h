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

#ifndef QSTATE_P_H
#define QSTATE_P_H

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

#include "private/qabstractstate_p.h"

#include <QtCore/qlist.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

struct QPropertyAssignment
{
    QPropertyAssignment()
        : object(0), explicitlySet(true) {}
    QPropertyAssignment(QObject *o, const QByteArray &n,
                        const QVariant &v, bool es = true)
        : object(o), propertyName(n), value(v), explicitlySet(es)
        {}
    QObject *object;
    QByteArray propertyName;
    QVariant value;
    bool explicitlySet;
};

class QAbstractTransition;
class QHistoryState;

class QState;
class Q_AUTOTEST_EXPORT QStatePrivate : public QAbstractStatePrivate
{
    Q_DECLARE_PUBLIC(QState)
public:
    QStatePrivate();
    ~QStatePrivate();

    static QStatePrivate *get(QState *q) { return q ? q->d_func() : 0; }
    static const QStatePrivate *get(const QState *q) { return q? q->d_func() : 0; }

    QList<QAbstractState*> childStates() const;
    QList<QHistoryState*> historyStates() const;
    QList<QAbstractTransition*> transitions() const;

    void emitFinished();
    void emitPropertiesAssigned();

    QAbstractState *errorState;
    QAbstractState *initialState;
    QState::ChildMode childMode;
    mutable bool childStatesListNeedsRefresh;
    mutable QList<QAbstractState*> childStatesList;
    mutable bool transitionsListNeedsRefresh;
    mutable QList<QAbstractTransition*> transitionsList;

    QList<QPropertyAssignment> propertyAssignments;
};

QT_END_NAMESPACE

#endif
