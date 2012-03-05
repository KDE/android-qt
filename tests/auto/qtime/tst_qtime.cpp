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
#include "qdatetime.h"

//TESTED_CLASS=
//TESTED_FILES=corelib/thread/qthreadstorage.h corelib/thread/qthreadstorage.cpp

class tst_QTime : public QObject
{
    Q_OBJECT

public:
    tst_QTime();
    virtual ~tst_QTime();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void toStringLocale();
    void toString();
    void msecsTo_data();
    void msecsTo();
    void secsTo_data();
    void secsTo();
    void setHMS_data();
    void setHMS();
    void msec();
    void second();
    void minute();
    void hour_data();
    void hour();
    void isValid();
    void isNull();
    void addMSecs_data();
    void addMSecs();
    void addSecs_data();
    void addSecs();
    void operator_eq_eq();
    void operator_not_eq();
    void operator_lt();
    void operator_gt();
    void operator_lt_eq();
    void operator_gt_eq();
    void fromString_data();
    void fromString();
    void fromStringFormat_data();
    void fromStringFormat();
    void toString_data();
    void toString_format_data();
    void toString_format();
};

Q_DECLARE_METATYPE(QTime)

tst_QTime::tst_QTime()
{
}

tst_QTime::~tst_QTime()
{

}

// initTestCase will be executed once before the first testfunction is executed.
void tst_QTime::initTestCase()
{
}

// cleanupTestCase will be executed once after the last testfunction is executed.
void tst_QTime::cleanupTestCase()
{
}

// init() will be executed immediately before each testfunction is run.
void tst_QTime::init()
{
// TODO: Add testfunction specific initialization code here.
}

// cleanup() will be executed immediately after each testfunction is run.
void tst_QTime::cleanup()
{
// TODO: Add testfunction specific cleanup code here.
}

void tst_QTime::addSecs_data()
{
    QTest::addColumn<QTime>("t1");
    QTest::addColumn<int>("i");
    QTest::addColumn<QTime>("exp");

    QTest::newRow("Data0") << QTime(0,0,0) << 200 << QTime(0,3,20);
    QTest::newRow("Data1") << QTime(0,0,0) << 20 << QTime(0,0,20);
}

void tst_QTime::addSecs()
{
    QFETCH( QTime, t1 );
    QFETCH( int, i );
    QTime t2;
    t2 = t1.addSecs( i );
    QFETCH( QTime, exp );
    QCOMPARE( t2, exp );
}

void tst_QTime::addMSecs_data()
{
    QTest::addColumn<QTime>("t1");
    QTest::addColumn<int>("i");
    QTest::addColumn<QTime>("exp");

    // start with testing positive values
    QTest::newRow( "Data1_0") << QTime(0,0,0,0) << 2000 << QTime(0,0,2,0);
    QTest::newRow( "Data1_1") << QTime(0,0,0,0) << 200 << QTime(0,0,0,200);
    QTest::newRow( "Data1_2") << QTime(0,0,0,0) << 20 << QTime(0,0,0,20);
    QTest::newRow( "Data1_3") << QTime(0,0,0,1) << 1 << QTime(0,0,0,2);
    QTest::newRow( "Data1_4") << QTime(0,0,0,0) << 0 << QTime(0,0,0,0);

    QTest::newRow( "Data2_0") << QTime(0,0,0,98) << 0 << QTime(0,0,0,98);
    QTest::newRow( "Data2_1") << QTime(0,0,0,98) << 1 << QTime(0,0,0,99);
    QTest::newRow( "Data2_2") << QTime(0,0,0,98) << 2 << QTime(0,0,0,100);
    QTest::newRow( "Data2_3") << QTime(0,0,0,98) << 3 << QTime(0,0,0,101);

    QTest::newRow( "Data3_0") << QTime(0,0,0,998) << 0 << QTime(0,0,0,998);
    QTest::newRow( "Data3_1") << QTime(0,0,0,998) << 1 << QTime(0,0,0,999);
    QTest::newRow( "Data3_2") << QTime(0,0,0,998) << 2 << QTime(0,0,1,0);
    QTest::newRow( "Data3_3") << QTime(0,0,0,998) << 3 << QTime(0,0,1,1);

    QTest::newRow( "Data4_0") << QTime(0,0,1,995) << 4 << QTime(0,0,1,999);
    QTest::newRow( "Data4_1") << QTime(0,0,1,995) << 5 << QTime(0,0,2,0);
    QTest::newRow( "Data4_2") << QTime(0,0,1,995) << 6 << QTime(0,0,2,1);
    QTest::newRow( "Data4_3") << QTime(0,0,1,995) << 100 << QTime(0,0,2,95);
    QTest::newRow( "Data4_4") << QTime(0,0,1,995) << 105 << QTime(0,0,2,100);

    QTest::newRow( "Data5_0") << QTime(0,0,59,995) << 4 << QTime(0,0,59,999);
    QTest::newRow( "Data5_1") << QTime(0,0,59,995) << 5 << QTime(0,1,0,0);
    QTest::newRow( "Data5_2") << QTime(0,0,59,995) << 6 << QTime(0,1,0,1);
    QTest::newRow( "Data5_3") << QTime(0,0,59,995) << 1006 << QTime(0,1,1,1);

    QTest::newRow( "Data6_0") << QTime(0,59,59,995) << 4 << QTime(0,59,59,999);
    QTest::newRow( "Data6_1") << QTime(0,59,59,995) << 5 << QTime(1,0,0,0);
    QTest::newRow( "Data6_2") << QTime(0,59,59,995) << 6 << QTime(1,0,0,1);
    QTest::newRow( "Data6_3") << QTime(0,59,59,995) << 106 << QTime(1,0,0,101);
    QTest::newRow( "Data6_4") << QTime(0,59,59,995) << 1004 << QTime(1,0,0,999);
    QTest::newRow( "Data6_5") << QTime(0,59,59,995) << 1005 << QTime(1,0,1,0);
    QTest::newRow( "Data6_6") << QTime(0,59,59,995) << 61006 << QTime(1,1,1,1);

    QTest::newRow( "Data7_0") << QTime(23,59,59,995) << 0 << QTime(23,59,59,995);
    QTest::newRow( "Data7_1") << QTime(23,59,59,995) << 4 << QTime(23,59,59,999);
    QTest::newRow( "Data7_2") << QTime(23,59,59,995) << 5 << QTime(0,0,0,0);
    QTest::newRow( "Data7_3") << QTime(23,59,59,995) << 6 << QTime(0,0,0,1);
    QTest::newRow( "Data7_4") << QTime(23,59,59,995) << 7 << QTime(0,0,0,2);

    // must test negative values too...
    QTest::newRow( "Data11_0") << QTime(0,0,2,0) << -2000 << QTime(0,0,0,0);
    QTest::newRow( "Data11_1") << QTime(0,0,0,200) << -200 << QTime(0,0,0,0);
    QTest::newRow( "Data11_2") << QTime(0,0,0,20) << -20 << QTime(0,0,0,0);
    QTest::newRow( "Data11_3") << QTime(0,0,0,2) << -1 << QTime(0,0,0,1);
    QTest::newRow( "Data11_4") << QTime(0,0,0,0) << -0 << QTime(0,0,0,0);

    QTest::newRow( "Data12_0") << QTime(0,0,0,98) << -0 << QTime(0,0,0,98);
    QTest::newRow( "Data12_1") << QTime(0,0,0,99) << -1 << QTime(0,0,0,98);
    QTest::newRow( "Data12_2") << QTime(0,0,0,100) << -2 << QTime(0,0,0,98);
    QTest::newRow( "Data12_3") << QTime(0,0,0,101) << -3 << QTime(0,0,0,98);

    QTest::newRow( "Data13_0") << QTime(0,0,0,998) << -0 << QTime(0,0,0,998);
    QTest::newRow( "Data13_1") << QTime(0,0,0,999) << -1 << QTime(0,0,0,998);
    QTest::newRow( "Data13_2") << QTime(0,0,1,0) << -2 << QTime(0,0,0,998);
    QTest::newRow( "Data13_3") << QTime(0,0,1,1) << -3 << QTime(0,0,0,998);

    QTest::newRow( "Data14_0") << QTime(0,0,1,999) << -4 << QTime(0,0,1,995);
    QTest::newRow( "Data14_1") << QTime(0,0,2,0) << -5 << QTime(0,0,1,995);
    QTest::newRow( "Data14_2") << QTime(0,0,2,1) << -6 << QTime(0,0,1,995);
    QTest::newRow( "Data14_3") << QTime(0,0,2,95) << -100 << QTime(0,0,1,995);
    QTest::newRow( "Data14_4") << QTime(0,0,2,100) << -105 << QTime(0,0,1,995);

    QTest::newRow( "Data15_0") << QTime(0,0,59,999) << -4 << QTime(0,0,59,995);
    QTest::newRow( "Data15_1") << QTime(0,1,0,0) << -5 << QTime(0,0,59,995);
    QTest::newRow( "Data15_2") << QTime(0,1,0,1) << -6 << QTime(0,0,59,995);
    QTest::newRow( "Data15_3") << QTime(0,1,1,1) << -1006 << QTime(0,0,59,995);

    QTest::newRow( "Data16_0") << QTime(0,59,59,999) << -4 << QTime(0,59,59,995);
    QTest::newRow( "Data16_1") << QTime(1,0,0,0) << -5 << QTime(0,59,59,995);
    QTest::newRow( "Data16_2") << QTime(1,0,0,1) << -6 << QTime(0,59,59,995);
    QTest::newRow( "Data16_3") << QTime(1,0,0,101) << -106 << QTime(0,59,59,995);
    QTest::newRow( "Data16_4") << QTime(1,0,0,999) << -1004 << QTime(0,59,59,995);
    QTest::newRow( "Data16_5") << QTime(1,0,1,0) << -1005 << QTime(0,59,59,995);
    QTest::newRow( "Data16_6") << QTime(1,1,1,1) << -61006 << QTime(0,59,59,995);

    QTest::newRow( "Data17_0") << QTime(23,59,59,995) << -0 << QTime(23,59,59,995);
    QTest::newRow( "Data17_1") << QTime(23,59,59,999) << -4 << QTime(23,59,59,995);
    QTest::newRow( "Data17_2") << QTime(0,0,0,0) << -5 << QTime(23,59,59,995);
    QTest::newRow( "Data17_3") << QTime(0,0,0,1) << -6 << QTime(23,59,59,995);
    QTest::newRow( "Data17_4") << QTime(0,0,0,2) << -7 << QTime(23,59,59,995);
}

void tst_QTime::addMSecs()
{
    QFETCH( QTime, t1 );
    QFETCH( int, i );
    QTime t2;
    t2 = t1.addMSecs( i );
    QFETCH( QTime, exp );
    QCOMPARE( t2, exp );
}

void tst_QTime::isNull()
{
    QTime t1;
    QVERIFY( t1.isNull() );
    QTime t2(0,0,0);
    QVERIFY( !t2.isNull() );
    QTime t3(0,0,1);
    QVERIFY( !t3.isNull() );
    QTime t4(0,0,0,1);
    QVERIFY( !t4.isNull() );
    QTime t5(23,59,59);
    QVERIFY( !t5.isNull() );
}

void tst_QTime::isValid()
{
    QTime t1;
    QVERIFY( !t1.isValid() );
    QTime t2(24,0,0,0);
    QVERIFY( !t2.isValid() );
    QTime t3(23,60,0,0);
    QVERIFY( !t3.isValid() );
    QTime t4(23,0,-1,0);
    QVERIFY( !t4.isValid() );
    QTime t5(23,0,60,0);
    QVERIFY( !t5.isValid() );
    QTime t6(23,0,0,1000);
    QVERIFY( !t6.isValid() );
}

void tst_QTime::hour_data()
{
    QTest::addColumn<int>("hour");
    QTest::addColumn<int>("minute");
    QTest::addColumn<int>("sec");
    QTest::addColumn<int>("msec");

    QTest::newRow(  "data0" ) << 0 << 0 << 0 << 0;
    QTest::newRow(  "data1" ) << 0 << 0 << 0 << 1;
    QTest::newRow(  "data2" ) << 1 << 2 << 3 << 4;
    QTest::newRow(  "data3" ) << 2 << 12 << 13 << 65;
    QTest::newRow(  "data4" ) << 23 << 59 << 59 << 999;
}

void tst_QTime::hour()
{
    QFETCH( int, hour );
    QFETCH( int, minute );
    QFETCH( int, sec );
    QFETCH( int, msec );

    QTime t1( hour, minute, sec, msec );
    QCOMPARE( t1.hour(), hour );
    QCOMPARE( t1.minute(), minute );
    QCOMPARE( t1.second(), sec );
    QCOMPARE( t1.msec(), msec );
}

void tst_QTime::minute()
{
    DEPENDS_ON( "hour" );
}

void tst_QTime::second()
{
    DEPENDS_ON( "hour" );
}

void tst_QTime::msec()
{
    DEPENDS_ON( "hour" );
}

void tst_QTime::setHMS_data()
{
    QTest::addColumn<int>("hour");
    QTest::addColumn<int>("minute");
    QTest::addColumn<int>("sec");

    QTest::newRow(  "data0" ) << 0 << 0 << 0;
    QTest::newRow(  "data1" ) << 1 << 2 << 3;
    QTest::newRow(  "data2" ) << 0 << 59 << 0;
    QTest::newRow(  "data3" ) << 0 << 59 << 59;
    QTest::newRow(  "data4" ) << 23 << 0 << 0;
    QTest::newRow(  "data5" ) << 23 << 59 << 0;
    QTest::newRow(  "data6" ) << 23 << 59 << 59;
}

void tst_QTime::setHMS()
{
    QFETCH( int, hour );
    QFETCH( int, minute );
    QFETCH( int, sec );

    QTime t(3,4,5);
    t.setHMS( hour, minute, sec );
    QCOMPARE( t.hour(), hour );
    QCOMPARE( t.minute(), minute );
    QCOMPARE( t.second(), sec );
}

void tst_QTime::secsTo_data()
{
    QTest::addColumn<QTime>("t1");
    QTest::addColumn<QTime>("t2");
    QTest::addColumn<int>("delta");

    QTest::newRow(  "data0" ) << QTime(0,0,0) << QTime(0,0,59) << 59;
    QTest::newRow(  "data1" ) << QTime(0,0,0) << QTime(0,1,0) << 60;
    QTest::newRow(  "data2" ) << QTime(0,0,0) << QTime(0,10,0) << 600;
    QTest::newRow(  "data3" ) << QTime(0,0,0) << QTime(23,59,59) << 86399;
}

void tst_QTime::secsTo()
{
    QFETCH( QTime, t1 );
    QFETCH( QTime, t2 );
    QFETCH( int, delta );

    QCOMPARE( t1.secsTo( t2 ), delta );
}

void tst_QTime::msecsTo_data()
{
    QTest::addColumn<QTime>("t1");
    QTest::addColumn<QTime>("t2");
    QTest::addColumn<int>("delta");

    QTest::newRow(  "data0" ) << QTime(0,0,0,0) << QTime(0,0,0,0) << 0;
    QTest::newRow(  "data1" ) << QTime(0,0,0,0) << QTime(0,0,1,0) << 1000;
    QTest::newRow(  "data2" ) << QTime(0,0,0,0) << QTime(0,0,10,0) << 10000;
    QTest::newRow(  "data3" ) << QTime(0,0,0,0) << QTime(23,59,59,0) << 86399000;
}

void tst_QTime::msecsTo()
{
    QFETCH( QTime, t1 );
    QFETCH( QTime, t2 );
    QFETCH( int, delta );

    QCOMPARE( t1.msecsTo( t2 ), delta );
}

void tst_QTime::operator_eq_eq()
{
    QTime t1(0,0,0,0);
    QTime t2(0,0,0,0);
    QVERIFY( t1 == t2 );

    t1 = QTime(12,34,56,20);
    t2 = QTime(12,34,56,20);
    QVERIFY( t1 == t2 );

    t1 = QTime(01,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 == t2) );
}

void tst_QTime::operator_not_eq()
{
    QTime t1(0,0,0,0);
    QTime t2(0,0,0,0);
    QVERIFY( !(t1 != t2) );

    t1 = QTime(12,34,56,20);
    t2 = QTime(12,34,56,20);
    QVERIFY( !(t1 != t2) );

    t1 = QTime(01,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 != t2 );
}

void tst_QTime::operator_lt()
{
    QTime t1(0,0,0,0);
    QTime t2(0,0,0,0);
    QVERIFY( !(t1 < t2) );

    t1 = QTime(12,34,56,20);
    t2 = QTime(12,34,56,30);
    QVERIFY( t1 < t2 );

    t1 = QTime(13,34,46,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 < t2 );

    t1 = QTime(13,24,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 < t2 );

    t1 = QTime(12,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 < t2 );

    t1 = QTime(14,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 < t2) );

    t1 = QTime(13,44,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 < t2) );

    t1 = QTime(13,34,56,20);
    t2 = QTime(13,34,46,20);
    QVERIFY( !(t1 < t2) );

    t1 = QTime(13,44,56,30);
    t2 = QTime(13,44,56,20);
    QVERIFY( !(t1 < t2) );
}

void tst_QTime::operator_gt()
{
    QTime t1(0,0,0,0);
    QTime t2(0,0,0,0);
    QVERIFY( !(t1 > t2) );

    t1 = QTime(12,34,56,20);
    t2 = QTime(12,34,56,30);
    QVERIFY( !(t1 > t2) );

    t1 = QTime(13,34,46,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 > t2) );

    t1 = QTime(13,24,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 > t2) );

    t1 = QTime(12,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 > t2) );

    t1 = QTime(14,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 > t2 );

    t1 = QTime(13,44,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 > t2 );

    t1 = QTime(13,34,56,20);
    t2 = QTime(13,34,46,20);
    QVERIFY( t1 > t2 );

    t1 = QTime(13,44,56,30);
    t2 = QTime(13,44,56,20);
    QVERIFY( t1 > t2 );
}

void tst_QTime::operator_lt_eq()
{
    QTime t1(0,0,0,0);
    QTime t2(0,0,0,0);
    QVERIFY( t1 <= t2 );

    t1 = QTime(12,34,56,20);
    t2 = QTime(12,34,56,30);
    QVERIFY( t1 <= t2 );

    t1 = QTime(13,34,46,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 <= t2 );

    t1 = QTime(13,24,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 <= t2 );

    t1 = QTime(12,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 <= t2 );

    t1 = QTime(14,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 <= t2) );

    t1 = QTime(13,44,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 <= t2) );

    t1 = QTime(13,34,56,20);
    t2 = QTime(13,34,46,20);
    QVERIFY( !(t1 <= t2) );

    t1 = QTime(13,44,56,30);
    t2 = QTime(13,44,56,20);
    QVERIFY( !(t1 <= t2) );
}

void tst_QTime::operator_gt_eq()
{
    QTime t1(0,0,0,0);
    QTime t2(0,0,0,0);
    QVERIFY( t1 >= t2 );

    t1 = QTime(12,34,56,20);
    t2 = QTime(12,34,56,30);
    QVERIFY( !(t1 >= t2) );

    t1 = QTime(13,34,46,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 >= t2) );

    t1 = QTime(13,24,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 >= t2) );

    t1 = QTime(12,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( !(t1 >= t2) );

    t1 = QTime(14,34,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 >= t2 );

    t1 = QTime(13,44,56,20);
    t2 = QTime(13,34,56,20);
    QVERIFY( t1 >= t2 );

    t1 = QTime(13,34,56,20);
    t2 = QTime(13,34,46,20);
    QVERIFY( t1 >= t2 );

    t1 = QTime(13,44,56,30);
    t2 = QTime(13,44,56,20);
    QVERIFY( t1 >= t2 );
}

void tst_QTime::fromString_data()
{
    // Since we can't define an element of Qt::DateFormat, t1 will be the time
    // expected when we have a TextDate, and t2 will be the time expected when
    // we have an ISODate.

    QTest::addColumn<QString>("str");
    QTest::addColumn<QTime>("t1");
    QTest::addColumn<QTime>("t2");

    QTest::newRow( "data0" ) << QString("00:00:00") << QTime(0,0,0,0) << QTime(0,0,0,0);
    QTest::newRow( "data1" ) << QString("10:12:34") << QTime(10,12,34,0) << QTime(10,12,34,0);
    QTest::newRow( "data2" ) << QString("19:03:54.998601") << QTime(19, 3, 54, 999) << QTime(19, 3, 54, 999);
    QTest::newRow( "data3" ) << QString("19:03:54.999601") << QTime(19, 3, 54, 999) << QTime(19, 3, 54, 999);
}

void tst_QTime::fromString()
{
    QFETCH( QString, str );
    QFETCH( QTime, t1 );
    QFETCH( QTime, t2 );

    QCOMPARE( t1, QTime::fromString( str, Qt::TextDate ) );
    QCOMPARE( t2, QTime::fromString( str, Qt::ISODate ) );
}


void tst_QTime::fromStringFormat_data()
{
    QTest::addColumn<QString>("str");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QTime>("t");

    QTest::newRow( "data0" ) << QString("02:23PM") << QString("hh:mmAP") << QTime(14,23,0,0);
    QTest::newRow( "data1" ) << QString("02:23pm") << QString("hh:mmap") << QTime(14,23,0,0);
}

void tst_QTime::fromStringFormat()
{
    QFETCH(QString, str);
    QFETCH(QString, format);
    QFETCH(QTime, t);

    QCOMPARE(t, QTime::fromString( str, format));

}

void tst_QTime::toString_data()
{
    // Since we can't define an element of Qt::DateFormat, str1 will be the string
    // in TextDate format, and str2 will be the time in ISODate format.

    QTest::addColumn<QTime>("t");
    QTest::addColumn<QString>("str1");
    QTest::addColumn<QString>("str2");

    QTest::newRow( "data0" ) << QTime(0,0,0,0) << QString("00:00:00") << QString("00:00:00");
    QTest::newRow( "data1" ) << QTime(10,12,34,0) << QString("10:12:34") << QString("10:12:34");
}

void tst_QTime::toString()
{
    QFETCH( QTime, t );
    QFETCH( QString, str1 );
    QFETCH( QString, str2 );

    QCOMPARE( str1, t.toString( Qt::TextDate ) );
    QCOMPARE( str2, t.toString( Qt::ISODate ) );
}

void tst_QTime::toString_format_data()
{
    QTest::addColumn<QTime>("t");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("str");

    QTest::newRow( "data0" ) << QTime(0,0,0,0) << QString("h:m:s:z") << QString("0:0:0:0");
    QTest::newRow( "data1" ) << QTime(10,12,34,53) << QString("hh:mm:ss:zzz") << QString("10:12:34:053");
    QTest::newRow( "data2" ) << QTime(10,12,34,45) << QString("hh:m:ss:z") << QString("10:12:34:45");
    QTest::newRow( "data3" ) << QTime(10,12,34,45) << QString("hh:ss ap") << QString("10:34 am");
    QTest::newRow( "data4" ) << QTime(22,12,34,45) << QString("hh:zzz AP") << QString("10:045 PM");
    QTest::newRow( "data5" ) << QTime(230,230,230,230) << QString("hh:mm:ss") << QString();
}

void tst_QTime::toString_format()
{
    QFETCH( QTime, t );
    QFETCH( QString, format );
    QFETCH( QString, str );

    QCOMPARE( t.toString( format ), str );
}

void tst_QTime::toStringLocale()
{
    QTime time(18, 30);
    QCOMPARE(time.toString(Qt::SystemLocaleDate),
                QLocale::system().toString(time, QLocale::ShortFormat));
    QCOMPARE(time.toString(Qt::LocaleDate),
                QLocale().toString(time, QLocale::ShortFormat));
    QLocale::setDefault(QLocale::German);
    QCOMPARE(time.toString(Qt::SystemLocaleDate),
                QLocale::system().toString(time, QLocale::ShortFormat));
    QCOMPARE(time.toString(Qt::LocaleDate),
                QLocale().toString(time, QLocale::ShortFormat));
}

QTEST_APPLESS_MAIN(tst_QTime)
#include "tst_qtime.moc"
