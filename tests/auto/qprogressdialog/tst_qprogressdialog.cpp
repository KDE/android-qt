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
#include <qprogressdialog.h>
#include <qlabel.h>


//TESTED_CLASS=
//TESTED_FILES=

class tst_QProgressDialog : public QObject
{
Q_OBJECT

public:
    tst_QProgressDialog();
    virtual ~tst_QProgressDialog();

private slots:
    void getSetCheck();
    void task198202();
};

tst_QProgressDialog::tst_QProgressDialog()
{
}

tst_QProgressDialog::~tst_QProgressDialog()
{
}

// Testing get/set functions
void tst_QProgressDialog::getSetCheck()
{
    QProgressDialog obj1;
    // bool QProgressDialog::autoReset()
    // void QProgressDialog::setAutoReset(bool)
    obj1.setAutoReset(false);
    QCOMPARE(false, obj1.autoReset());
    obj1.setAutoReset(true);
    QCOMPARE(true, obj1.autoReset());

    // bool QProgressDialog::autoClose()
    // void QProgressDialog::setAutoClose(bool)
    obj1.setAutoClose(false);
    QCOMPARE(false, obj1.autoClose());
    obj1.setAutoClose(true);
    QCOMPARE(true, obj1.autoClose());

    // int QProgressDialog::maximum()
    // void QProgressDialog::setMaximum(int)
    obj1.setMaximum(0);
    QCOMPARE(0, obj1.maximum());
    obj1.setMaximum(INT_MIN);
    QCOMPARE(INT_MIN, obj1.maximum());
    obj1.setMaximum(INT_MAX);
    QCOMPARE(INT_MAX, obj1.maximum());

    // int QProgressDialog::minimum()
    // void QProgressDialog::setMinimum(int)
    obj1.setMinimum(0);
    QCOMPARE(0, obj1.minimum());
    obj1.setMinimum(INT_MIN);
    QCOMPARE(INT_MIN, obj1.minimum());
    obj1.setMinimum(INT_MAX);
    QCOMPARE(INT_MAX, obj1.minimum());

    // int QProgressDialog::value()
    // void QProgressDialog::setValue(int)
    obj1.setMaximum(INT_MAX);
    obj1.setMinimum(INT_MIN);
    obj1.setValue(0);
    QCOMPARE(0, obj1.value());
    obj1.setValue(INT_MIN+1);
    QCOMPARE(INT_MIN+1, obj1.value());
    obj1.setValue(INT_MIN);
    QCOMPARE(INT_MIN, obj1.value());
    obj1.setValue(INT_MAX-1);
    QCOMPARE(INT_MAX-1, obj1.value());

    obj1.setValue(INT_MAX);
    QCOMPARE(INT_MIN, obj1.value()); // We set autoReset, the thing is reset

    obj1.setAutoReset(false);
    obj1.setValue(INT_MAX);
    QCOMPARE(INT_MAX, obj1.value());
    obj1.setAutoReset(true);

    // int QProgressDialog::minimumDuration()
    // void QProgressDialog::setMinimumDuration(int)
    obj1.setMinimumDuration(0);
    QCOMPARE(0, obj1.minimumDuration());
    obj1.setMinimumDuration(INT_MIN);
    QCOMPARE(INT_MIN, obj1.minimumDuration());
    obj1.setMinimumDuration(INT_MAX);
    QCOMPARE(INT_MAX, obj1.minimumDuration());
}

void tst_QProgressDialog::task198202()
{
    //should not crash
    QProgressDialog dlg(QLatin1String("test"),QLatin1String("test"),1,10);
    dlg.show();
    QTest::qWait(20);
    int futureHeight = dlg.sizeHint().height() - qFindChild<QLabel*>(&dlg)->sizeHint().height();
    dlg.setLabel(0);
    QTest::ignoreMessage(QtWarningMsg, "QProgressDialog::setBar: Cannot set a null progress bar");
    dlg.setBar(0);
    QTest::qWait(20);
    QCOMPARE(dlg.sizeHint().height(), futureHeight);
}

QTEST_MAIN(tst_QProgressDialog)
#include "tst_qprogressdialog.moc"
