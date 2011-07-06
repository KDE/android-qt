/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import QtQuick 1.0

Rectangle {
    width: 200; height: 200
    CustomObject { id: theObject }
    function doSomethingDirect() {
        theObject.prop1 = 0;

        for (var i = 0; i < 1000; ++i)
            theObject.prop1 += theObject.prop2;

        theObject.prop3 = theObject.prop1;
    }

    function doSomethingLocalObj() {
        theObject.prop1 = 0;

        var incrementObj = theObject;
        for (var i = 0; i < 1000; ++i)
            incrementObj.prop1 += incrementObj.prop2;

        incrementObj.prop3 = incrementObj.prop1;
    }

    function doSomethingLocal() {
        theObject.prop1 = 0;

        var increment = theObject.prop2;
        for (var i = 0; i < 1000; ++i)
            theObject.prop1 += increment;

        theObject.prop3 = theObject.prop1;
    }
}
