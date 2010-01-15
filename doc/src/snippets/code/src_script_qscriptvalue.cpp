/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
QScriptEngine myEngine;
QScriptValue myObject = myEngine.newObject();
QScriptValue myOtherObject = myEngine.newObject();
myObject.setProperty("myChild", myOtherObject);
myObject.setProperty("name", "John Doe");
//! [0]


//! [1]
QScriptValue val(&myEngine, 123);
myObject.setProperty("myReadOnlyProperty", val, QScriptValue::ReadOnly);
//! [1]


//! [2]
QScriptEngine engine;
engine.evaluate("function fullName() { return this.firstName + ' ' + this.lastName; }");
engine.evaluate("somePerson = { firstName: 'John', lastName: 'Doe' }");

QScriptValue global = engine.globalObject();
QScriptValue fullName = global.property("fullName");
QScriptValue who = global.property("somePerson");
qDebug() << fullName.call(who).toString(); // "John Doe"

engine.evaluate("function cube(x) { return x * x * x; }");
QScriptValue cube = global.property("cube");
QScriptValueList args;
args << 3;
qDebug() << cube.call(QScriptValue(), args).toNumber(); // 27
//! [2]


//! [3]
QScriptValue myNativeFunction(QScriptContext *ctx, QScriptEngine *)
{
    QScriptValue otherFunction = ...;
    return otherFunction.call(ctx->thisObject(), ctx->argumentsObject());
}
//! [3]
