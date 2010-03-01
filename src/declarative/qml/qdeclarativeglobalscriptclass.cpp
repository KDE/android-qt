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

#include "qdeclarativeglobalscriptclass_p.h"

#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalueiterator.h>

QT_BEGIN_NAMESPACE

/*
    Used to prevent any writes to the global object.
*/
QDeclarativeGlobalScriptClass::QDeclarativeGlobalScriptClass(QScriptEngine *engine)
: QScriptClass(engine)
{
    QScriptValue v = engine->newObject();
    globalObject = engine->globalObject();

    QScriptValueIterator iter(globalObject);
    while (iter.hasNext()) {
        iter.next();
        v.setProperty(iter.scriptName(), iter.value());
    }

    v.setScriptClass(this);
    engine->setGlobalObject(v);
}

QScriptClass::QueryFlags 
QDeclarativeGlobalScriptClass::queryProperty(const QScriptValue &object,
                                    const QScriptString &name,
                                    QueryFlags flags, uint *id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(flags);
    Q_UNUSED(id);
    return HandlesReadAccess | HandlesWriteAccess;
}

QScriptValue 
QDeclarativeGlobalScriptClass::property(const QScriptValue &object,
                               const QScriptString &name, 
                               uint id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    return engine()->undefinedValue();
}

void QDeclarativeGlobalScriptClass::setProperty(QScriptValue &object, 
                                       const QScriptString &name,
                                       uint id, const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_UNUSED(id);
    Q_UNUSED(value);
    QString error = QLatin1String("Invalid write to global property \"") + 
                    name.toString() + QLatin1Char('\"');
    engine()->currentContext()->throwError(error);
}

void QDeclarativeGlobalScriptClass::explicitSetProperty(const QString &name, const QScriptValue &value)
{
    QScriptValue v = engine()->newObject();
    globalObject = engine()->globalObject();

    QScriptValueIterator iter(globalObject);
    while (iter.hasNext()) {
        iter.next();
        v.setProperty(iter.scriptName(), iter.value());
    }

    v.setProperty(name, value);
    v.setScriptClass(this);
    engine()->setGlobalObject(v);
}

QT_END_NAMESPACE

