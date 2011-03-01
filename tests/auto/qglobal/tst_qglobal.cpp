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

class tst_QGlobal: public QObject
{
    Q_OBJECT
private slots:
    void qIsNull();
    void qInternalCallbacks();
    void for_each();
    void qassert();
    void qtry();
    void checkptr();
};

void tst_QGlobal::qIsNull()
{
    double d = 0.0;
    float f = 0.0f;

    QVERIFY(::qIsNull(d));
    QVERIFY(::qIsNull(f));

    d += 0.000000001;
    f += 0.0000001f;

    QVERIFY(!::qIsNull(d));
    QVERIFY(!::qIsNull(f));
}

struct ConnectInfo {
    QObject *sender;
    QObject *receiver;
    QString signal, slot;
    int type;
    void reset() {
        sender = receiver = 0;
        signal = slot = QString();
        type = -1;
    }
} connect_info;

bool disconnect_callback(void **data)
{
    connect_info.sender = (QObject *)(data[0]);
    connect_info.receiver = (QObject *)(data[2]);
    connect_info.signal = QString::fromLatin1((const char *) data[1]);
    connect_info.slot = QString::fromLatin1((const char *) data[3]);
    return true;
}

bool connect_callback(void **data)
{
    disconnect_callback(data);
    connect_info.type = *(int *) data[4];
    return true;
}

void tst_QGlobal::qInternalCallbacks()
{
    QInternal::registerCallback(QInternal::ConnectCallback, connect_callback);
    QInternal::registerCallback(QInternal::DisconnectCallback, disconnect_callback);

    QObject a, b;
    QString signal = QLatin1String("2mysignal(x)");
    QString slot = QLatin1String("1myslot(x)");

    // Test that connect works as expected...
    connect_info.reset();
    bool ok = QObject::connect(&a, signal.toLatin1(), &b, slot.toLatin1(), Qt::AutoConnection);
    QVERIFY(ok);
    QCOMPARE(&a, connect_info.sender);
    QCOMPARE(&b, connect_info.receiver);
    QCOMPARE(signal, connect_info.signal);
    QCOMPARE(slot, connect_info.slot);
    QCOMPARE((int) Qt::AutoConnection, connect_info.type);

    // Test that disconnect works as expected
    connect_info.reset();
    ok = QObject::disconnect(&a, signal.toLatin1(), &b, slot.toLatin1());
    QVERIFY(ok);
    QCOMPARE(&a, connect_info.sender);
    QCOMPARE(&b, connect_info.receiver);
    QCOMPARE(signal, connect_info.signal);
    QCOMPARE(slot, connect_info.slot);

    // Unregister callbacks and verify that they are not triggered...
    QInternal::unregisterCallback(QInternal::ConnectCallback, connect_callback);
    QInternal::unregisterCallback(QInternal::DisconnectCallback, disconnect_callback);

    connect_info.reset();
    ok = QObject::connect(&a, signal.toLatin1(), &b, slot.toLatin1(), Qt::AutoConnection);
    QVERIFY(!ok);
    QCOMPARE(connect_info.sender, (QObject *) 0);

    ok = QObject::disconnect(&a, signal.toLatin1(), &b, slot.toLatin1());
    QVERIFY(!ok);
    QCOMPARE(connect_info.sender, (QObject *) 0);
}

void tst_QGlobal::for_each()
{
    QList<int> list;
    list << 0 << 1 << 2 << 3 << 4 << 5;

    int counter = 0;
    foreach(int i, list) {
        QCOMPARE(i, counter++);
    }
    QCOMPARE(counter, list.count());

    // do it again, to make sure we don't have any for-scoping
    // problems with older compilers
    counter = 0;
    foreach(int i, list) {
        QCOMPARE(i, counter++);
    }
    QCOMPARE(counter, list.count());
}

void tst_QGlobal::qassert()
{
    bool passed = false;
    if (false) {
        Q_ASSERT(false);
    } else {
        passed = true;
    }
    QVERIFY(passed);

    passed = false;
    if (false) {
        Q_ASSERT_X(false, "tst_QGlobal", "qassert");
    } else {
        passed = true;
    }
    QVERIFY(passed);

    passed = false;
    if (false)
        Q_ASSERT(false);
    else
        passed = true;
    QVERIFY(passed);

    passed = false;
    if (false)
        Q_ASSERT_X(false, "tst_QGlobal", "qassert");
    else
        passed = true;
    QVERIFY(passed);
}

void tst_QGlobal::qtry()
{
    int i = 0;
    QT_TRY {
        i = 1;
        QT_THROW(42);
        i = 2;
    } QT_CATCH(int) {
        QCOMPARE(i, 1);
        i = 7;
    }
#ifdef QT_NO_EXCEPTIONS
    QCOMPARE(i, 2);
#else
    QCOMPARE(i, 7);
#endif

    // check propper if/else scoping
    i = 0;
    if (true)
        QT_TRY {
            i = 2;
            QT_THROW(42);
            i = 4;
        } QT_CATCH(int) {
            QCOMPARE(i, 2);
            i = 4;
        }
    else
        QCOMPARE(i, 0);
    QCOMPARE(i, 4);

    i = 0;
    if (false)
        QT_TRY {
            i = 2;
            QT_THROW(42);
            i = 4;
        } QT_CATCH(int) {
            QCOMPARE(i, 2);
            i = 2;
        }
    else
        i = 8;
    QCOMPARE(i, 8);

    i = 0;
    if (false)
        i = 42;
    else
        QT_TRY {
            i = 2;
            QT_THROW(42);
            i = 4;
        } QT_CATCH(int) {
            QCOMPARE(i, 2);
            i = 4;
        }
    QCOMPARE(i, 4);
}

void tst_QGlobal::checkptr()
{
    int i;
    QCOMPARE(q_check_ptr(&i), &i);

    const char *c = "hello";
    QCOMPARE(q_check_ptr(c), c);
}

QTEST_MAIN(tst_QGlobal)
#include "tst_qglobal.moc"
