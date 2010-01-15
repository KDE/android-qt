/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qslider.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QSlider : public QObject
{
Q_OBJECT

public:
    tst_QSlider();
    virtual ~tst_QSlider();

private slots:
    void getSetCheck();
};

tst_QSlider::tst_QSlider()
{
}

tst_QSlider::~tst_QSlider()
{
}

// Testing get/set functions
void tst_QSlider::getSetCheck()
{
    QSlider obj1;
    // TickPosition QSlider::tickPosition()
    // void QSlider::setTickPosition(TickPosition)
    obj1.setTickPosition(QSlider::TickPosition(QSlider::NoTicks));
    QCOMPARE(QSlider::TickPosition(QSlider::NoTicks), obj1.tickPosition());
    obj1.setTickPosition(QSlider::TickPosition(QSlider::TicksAbove));
    QCOMPARE(QSlider::TickPosition(QSlider::TicksAbove), obj1.tickPosition());
    obj1.setTickPosition(QSlider::TickPosition(QSlider::TicksBelow));
    QCOMPARE(QSlider::TickPosition(QSlider::TicksBelow), obj1.tickPosition());
    obj1.setTickPosition(QSlider::TickPosition(QSlider::TicksBothSides));
    QCOMPARE(QSlider::TickPosition(QSlider::TicksBothSides), obj1.tickPosition());

    // int QSlider::tickInterval()
    // void QSlider::setTickInterval(int)
    obj1.setTickInterval(0);
    QCOMPARE(0, obj1.tickInterval());
    obj1.setTickInterval(INT_MIN);
    QCOMPARE(0, obj1.tickInterval()); // Can't have a negative interval
    obj1.setTickInterval(INT_MAX);
    QCOMPARE(INT_MAX, obj1.tickInterval());
}

QTEST_MAIN(tst_QSlider)
#include "tst_qslider.moc"
