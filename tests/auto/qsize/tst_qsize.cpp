/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qsize.h>

Q_DECLARE_METATYPE(QSize)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QSize : public QObject
{
    Q_OBJECT

public:
    tst_QSize();
    virtual ~tst_QSize();


public slots:
    void init();
    void cleanup();
private slots:
    void getSetCheck();
    void scale();

    void expandedTo();
    void expandedTo_data();

    void boundedTo_data();
    void boundedTo();

    void transpose_data();
    void transpose();
};

// Testing get/set functions
void tst_QSize::getSetCheck()
{
    QSize obj1;
    // int QSize::width()
    // void QSize::setWidth(int)
    obj1.setWidth(0);
    QCOMPARE(0, obj1.width());
    obj1.setWidth(INT_MIN);
    QCOMPARE(INT_MIN, obj1.width());
    obj1.setWidth(INT_MAX);
    QCOMPARE(INT_MAX, obj1.width());

    // int QSize::height()
    // void QSize::setHeight(int)
    obj1.setHeight(0);
    QCOMPARE(0, obj1.height());
    obj1.setHeight(INT_MIN);
    QCOMPARE(INT_MIN, obj1.height());
    obj1.setHeight(INT_MAX);
    QCOMPARE(INT_MAX, obj1.height());

    QSizeF obj2;
    // qreal QSizeF::width()
    // void QSizeF::setWidth(qreal)
    obj2.setWidth(0.0);
    QCOMPARE(0.0, obj2.width());
    obj2.setWidth(1.1);
    QCOMPARE(1.1, obj2.width());

    // qreal QSizeF::height()
    // void QSizeF::setHeight(qreal)
    obj2.setHeight(0.0);
    QCOMPARE(0.0, obj2.height());
    obj2.setHeight(1.1);
    QCOMPARE(1.1, obj2.height());
}


tst_QSize::tst_QSize()
{
}

tst_QSize::~tst_QSize()
{
}

void tst_QSize::init()
{
}

void tst_QSize::cleanup()
{
}

void tst_QSize::scale()
{
    QSize t1( 10, 12 );
    t1.scale( 60, 60, Qt::IgnoreAspectRatio );
    QCOMPARE( t1, QSize(60, 60) );

    QSize t2( 10, 12 );
    t2.scale( 60, 60, Qt::KeepAspectRatio );
    QCOMPARE( t2, QSize(50, 60) );

    QSize t3( 10, 12 );
    t3.scale( 60, 60, Qt::KeepAspectRatioByExpanding );
    QCOMPARE( t3, QSize(60, 72) );

    QSize t4( 12, 10 );
    t4.scale( 60, 60, Qt::KeepAspectRatio );
    QCOMPARE( t4, QSize(60, 50) );

    QSize t5( 12, 10 );
    t5.scale( 60, 60, Qt::KeepAspectRatioByExpanding );
    QCOMPARE( t5, QSize(72, 60) );

    // test potential int overflow
    QSize t6(88473, 88473);
    t6.scale(141817, 141817, Qt::KeepAspectRatio);
    QCOMPARE(t6, QSize(141817, 141817));

    QSize t7(800, 600);
    t7.scale(400, INT_MAX, Qt::KeepAspectRatio);
    QCOMPARE(t7, QSize(400, 300));

    QSize t8(800, 600);
    t8.scale(INT_MAX, 150, Qt::KeepAspectRatio);
    QCOMPARE(t8, QSize(200, 150));

    QSize t9(600, 800);
    t9.scale(300, INT_MAX, Qt::KeepAspectRatio);
    QCOMPARE(t9, QSize(300, 400));

    QSize t10(600, 800);
    t10.scale(INT_MAX, 200, Qt::KeepAspectRatio);
    QCOMPARE(t10, QSize(150, 200));

    QSize t11(0, 0);
    t11.scale(240, 200, Qt::IgnoreAspectRatio);
    QCOMPARE(t11, QSize(240, 200));

    QSize t12(0, 0);
    t12.scale(240, 200, Qt::KeepAspectRatio);
    QCOMPARE(t12, QSize(240, 200));

    QSize t13(0, 0);
    t13.scale(240, 200, Qt::KeepAspectRatioByExpanding);
    QCOMPARE(t13, QSize(240, 200));
}


void tst_QSize::expandedTo_data()
{
    QTest::addColumn<QSize>("input1");
    QTest::addColumn<QSize>("input2");
    QTest::addColumn<QSize>("expected");

    QTest::newRow("data0") << QSize(10,12) << QSize(6,4)	<< QSize(10,12);
    QTest::newRow("data1") << QSize(0,0)   << QSize(6,4)	<< QSize(6,4);
    // This should pick the highest of w,h components independently of each other,
    // thus the result dont have to be equal to neither input1 nor input2.
    QTest::newRow("data3") << QSize(6,4)   << QSize(4,6)	<< QSize(6,6);
}

void tst_QSize::expandedTo()
{
    QFETCH( QSize, input1);
    QFETCH( QSize, input2);
    QFETCH( QSize, expected);

    QCOMPARE( input1.expandedTo(input2), expected);
}

void tst_QSize::boundedTo_data()
{
    QTest::addColumn<QSize>("input1");
    QTest::addColumn<QSize>("input2");
    QTest::addColumn<QSize>("expected");

    QTest::newRow("data0") << QSize(10,12) << QSize(6,4)	<< QSize(6,4);
    QTest::newRow("data1") << QSize(0,0)	<< QSize(6,4)	<< QSize(0,0);
    // This should pick the lowest of w,h components independently of each other,
    // thus the result dont have to be equal to neither input1 nor input2.
    QTest::newRow("data3") << QSize(6,4)	<< QSize(4,6)	<< QSize(4,4);
}

void tst_QSize::boundedTo()
{
    QFETCH( QSize, input1);
    QFETCH( QSize, input2);
    QFETCH( QSize, expected);

    QCOMPARE( input1.boundedTo(input2), expected);
}

void tst_QSize::transpose_data()
{
    QTest::addColumn<QSize>("input1");
    QTest::addColumn<QSize>("expected");

    QTest::newRow("data0") << QSize(10,12) << QSize(12,10);
    QTest::newRow("data1") << QSize(0,0)	<< QSize(0,0);
    QTest::newRow("data3") << QSize(6,4)	<< QSize(4,6);
}

void tst_QSize::transpose()
{
    QFETCH( QSize, input1);
    QFETCH( QSize, expected);

    // transpose() works only inplace and does not return anything, so we must do the operation itself before the compare.
    input1.transpose();
    QCOMPARE(input1 , expected);
}

QTEST_APPLESS_MAIN(tst_QSize)
#include "tst_qsize.moc"
