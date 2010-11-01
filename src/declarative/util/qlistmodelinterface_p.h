/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QLISTMODELINTERFACE_H
#define QLISTMODELINTERFACE_H

#include <QtCore/QHash>
#include <QtCore/QVariant>

#include <private/qdeclarativeglobal_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_PRIVATE_EXPORT QListModelInterface : public QObject
{
    Q_OBJECT
 public:
    QListModelInterface(QObject *parent = 0) : QObject(parent) {}
    virtual ~QListModelInterface() {}

    virtual int count() const = 0;
    virtual QVariant data(int index, int role) const = 0;

    virtual QList<int> roles() const = 0;
    virtual QString toString(int role) const = 0;

 Q_SIGNALS:
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void itemsChanged(int index, int count, const QList<int> &roles);

 protected:
    QListModelInterface(QObjectPrivate &dd, QObject *parent) 
        : QObject(dd, parent) {}
};


QT_END_NAMESPACE

QT_END_HEADER
#endif //QTREEMODELINTERFACE_H
