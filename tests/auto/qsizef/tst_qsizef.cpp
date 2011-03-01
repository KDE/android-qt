/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qsize.h>

Q_DECLARE_METATYPE(QSizeF)

//TESTED_CLASS=
//TESTED_FILES=corelib/tools/qsize.h corelib/tools/qsize.cpp

class tst_QSizeF : public QObject {
    Q_OBJECT

public:
    tst_QSizeF();
    virtual ~tst_QSizeF();

public slots:
    void init();
    void cleanup();

private slots:
    void scale();

    void expandedTo();
    void expandedTo_data();

    void boundedTo_data();
    void boundedTo();

    void transpose_data();
    void transpose();
};

tst_QSizeF::tst_QSizeF() {
}

tst_QSizeF::~tst_QSizeF() {
}

void tst_QSizeF::init() {
}

void tst_QSizeF::cleanup() {
}

void tst_QSizeF::scale() {
    QSizeF t1(10.4, 12.8);
    t1.scale(60.6, 60.6, Qt::IgnoreAspectRatio);
    QCOMPARE(t1, QSizeF(60.6, 60.6));

    QSizeF t2(10.4, 12.8);
    t2.scale(43.52, 43.52, Qt::KeepAspectRatio);
    QCOMPARE(t2, QSizeF(35.36, 43.52));

    QSizeF t3(9.6, 12.48);
    t3.scale(31.68, 31.68, Qt::KeepAspectRatioByExpanding);
    QCOMPARE(t3, QSizeF(31.68, 41.184));

    QSizeF t4(12.8, 10.4);
    t4.scale(43.52, 43.52, Qt::KeepAspectRatio);
    QCOMPARE(t4, QSizeF(43.52, 35.36));

    QSizeF t5(12.48, 9.6);
    t5.scale(31.68, 31.68, Qt::KeepAspectRatioByExpanding);
    QCOMPARE(t5, QSizeF(41.184, 31.68));

    QSizeF t6(0.0, 0.0);
    t6.scale(200, 240, Qt::IgnoreAspectRatio);
    QCOMPARE(t6, QSizeF(200, 240));

    QSizeF t7(0.0, 0.0);
    t7.scale(200, 240, Qt::KeepAspectRatio);
    QCOMPARE(t7, QSizeF(200, 240));

    QSizeF t8(0.0, 0.0);
    t8.scale(200, 240, Qt::KeepAspectRatioByExpanding);
    QCOMPARE(t8, QSizeF(200, 240));
}


void tst_QSizeF::expandedTo_data() {
    QTest::addColumn<QSizeF>("input1");
    QTest::addColumn<QSizeF>("input2");
    QTest::addColumn<QSizeF>("expected");

    QTest::newRow("data0") << QSizeF(10.4, 12.8) << QSizeF(6.6, 4.4) << QSizeF(10.4, 12.8);
    QTest::newRow("data1") << QSizeF(0.0, 0.0) << QSizeF(6.6, 4.4) << QSizeF(6.6, 4.4);
    // This should pick the highest of w,h components independently of each other,
    // thus the result dont have to be equal to neither input1 nor input2.
    QTest::newRow("data3") << QSizeF(6.6, 4.4) << QSizeF(4.4, 6.6) << QSizeF(6.6, 6.6);
}

void tst_QSizeF::expandedTo() {
    QFETCH( QSizeF, input1);
    QFETCH( QSizeF, input2);
    QFETCH( QSizeF, expected);

    QCOMPARE( input1.expandedTo(input2), expected);
}

void tst_QSizeF::boundedTo_data() {
    QTest::addColumn<QSizeF>("input1");
    QTest::addColumn<QSizeF>("input2");
    QTest::addColumn<QSizeF>("expected");

    QTest::newRow("data0") << QSizeF(10.4, 12.8) << QSizeF(6.6, 4.4) << QSizeF(6.6, 4.4);
    QTest::newRow("data1") << QSizeF(0.0, 0.0) << QSizeF(6.6, 4.4) << QSizeF(0.0, 0.0);
    // This should pick the lowest of w,h components independently of each other,
    // thus the result dont have to be equal to neither input1 nor input2.
    QTest::newRow("data3") << QSizeF(6.6, 4.4) << QSizeF(4.4, 6.6) << QSizeF(4.4, 4.4);
}

void tst_QSizeF::boundedTo() {
    QFETCH( QSizeF, input1);
    QFETCH( QSizeF, input2);
    QFETCH( QSizeF, expected);

    QCOMPARE( input1.boundedTo(input2), expected);
}

void tst_QSizeF::transpose_data() {
    QTest::addColumn<QSizeF>("input1");
    QTest::addColumn<QSizeF>("expected");

    QTest::newRow("data0") << QSizeF(10.4, 12.8) << QSizeF(12.8, 10.4);
    QTest::newRow("data1") << QSizeF(0.0, 0.0) << QSizeF(0.0, 0.0);
    QTest::newRow("data3") << QSizeF(6.6, 4.4) << QSizeF(4.4, 6.6);
}

void tst_QSizeF::transpose() {
    QFETCH( QSizeF, input1);
    QFETCH( QSizeF, expected);

    // transpose() works only inplace and does not return anything, so we must do the operation itself before the compare.
    input1.transpose();
    QCOMPARE(input1 , expected);
}

QTEST_APPLESS_MAIN(tst_QSizeF)
#include "tst_qsizef.moc"
