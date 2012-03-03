/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qdrag.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QDrag : public QObject
{
Q_OBJECT

public:
    tst_QDrag();
    virtual ~tst_QDrag();

private slots:
    void getSetCheck();
};

tst_QDrag::tst_QDrag()
{
}

tst_QDrag::~tst_QDrag()
{
}

// Testing get/set functions
void tst_QDrag::getSetCheck()
{
    QDrag obj1(0);
    // QMimeData * QDrag::mimeData()
    // void QDrag::setMimeData(QMimeData *)
    QMimeData *var1 = new QMimeData;
    obj1.setMimeData(var1);
    QCOMPARE(var1, obj1.mimeData());
    obj1.setMimeData(var1);
    QCOMPARE(var1, obj1.mimeData());
    obj1.setMimeData((QMimeData *)0);
    QCOMPARE((QMimeData *)0, obj1.mimeData());
    // delete var1; // No delete, since QDrag takes ownership

    Qt::DropAction result = obj1.start();
    QCOMPARE(result, Qt::IgnoreAction);
    result = obj1.start(Qt::MoveAction | Qt::LinkAction);
    QCOMPARE(result, Qt::IgnoreAction);
}

QTEST_MAIN(tst_QDrag)
#include "tst_qdrag.moc"
