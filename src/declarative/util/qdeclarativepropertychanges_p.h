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

#ifndef QDECLARATIVEPROPERTYCHANGES_H
#define QDECLARATIVEPROPERTYCHANGES_H

#include "private/qdeclarativestateoperations_p.h"
#include <private/qdeclarativecustomparser_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
    
class QDeclarativePropertyChangesPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativePropertyChanges : public QDeclarativeStateOperation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyChanges)

    Q_PROPERTY(QObject *target READ object WRITE setObject)
    Q_PROPERTY(bool restoreEntryValues READ restoreEntryValues WRITE setRestoreEntryValues)
    Q_PROPERTY(bool explicit READ isExplicit WRITE setIsExplicit)
public:
    QDeclarativePropertyChanges();
    ~QDeclarativePropertyChanges();

    QObject *object() const;
    void setObject(QObject *);

    bool restoreEntryValues() const;
    void setRestoreEntryValues(bool);

    bool isExplicit() const;
    void setIsExplicit(bool);

    virtual ActionList actions();

    bool containsProperty(const QByteArray &name) const;
    bool containsValue(const QByteArray &name) const;
    bool containsExpression(const QByteArray &name) const;
    void changeValue(const QByteArray &name, const QVariant &value);
    void changeExpression(const QByteArray &name, const QString &expression);
    void removeProperty(const QByteArray &name);
    QVariant value(const QByteArray &name) const;
    QString expression(const QByteArray &name) const;

    void detachFromState();
    void attachToState();

    QVariant property(const QByteArray &name) const;
};

class QDeclarativePropertyChangesParser : public QDeclarativeCustomParser
{
public:
    QDeclarativePropertyChangesParser()
    : QDeclarativeCustomParser(AcceptsAttachedProperties) {}

    void compileList(QList<QPair<QByteArray, QVariant> > &list, const QByteArray &pre, const QDeclarativeCustomParserProperty &prop);

    virtual QByteArray compile(const QList<QDeclarativeCustomParserProperty> &);
    virtual void setCustomData(QObject *, const QByteArray &);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePropertyChanges)

QT_END_HEADER

#endif // QDECLARATIVEPROPERTYCHANGES_H
