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
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtTest/QtTest>
#include <QtDBus>

Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QVector<int>)

class tst_QDBusLocalCalls: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.tst_QDBusLocalCalls")

    QDBusConnection conn;
public:
    tst_QDBusLocalCalls();

public Q_SLOTS:
    Q_SCRIPTABLE int echo(int value)
    { return value; }

    Q_SCRIPTABLE QString echo(const QString &value)
    { return value; }

    Q_SCRIPTABLE QDBusVariant echo(const QDBusVariant &value)
    { return value; }

    Q_SCRIPTABLE QVector<int> echo(const QVector<int> &value)
    { return value; }

    Q_SCRIPTABLE QString echo2(const QStringList &list, QString &out)
    { out = list[1]; return list[0]; }

    Q_SCRIPTABLE void delayed(const QDBusMessage &msg)
    { msg.setDelayedReply(true); }

protected Q_SLOTS:
    void replyReceived(QDBusPendingCallWatcher *watcher);

private Q_SLOTS:
    void initTestCase();
    void makeInvalidCalls();
    void makeCalls_data();
    void makeCalls();
    void makeCallsVariant_data();
    void makeCallsVariant();
    void makeCallsTwoRets();
    void makeCallsComplex();
    void makeDelayedCalls();
    void asyncReplySignal();

private:
    QVariantList asyncReplyArgs;
    QDBusMessage doCall(const QDBusMessage &call);
};

tst_QDBusLocalCalls::tst_QDBusLocalCalls()
    : conn(QDBusConnection::sessionBus())
{
}

QDBusMessage tst_QDBusLocalCalls::doCall(const QDBusMessage &call)
{
    QFETCH_GLOBAL(bool, useAsync);
    if (useAsync) {
        QDBusPendingCall ac = conn.asyncCall(call);
        ac.waitForFinished();
        return ac.reply();
    } else {
        return conn.call(call);
    }
}

void tst_QDBusLocalCalls::replyReceived(QDBusPendingCallWatcher *watcher)
{
    asyncReplyArgs = watcher->reply().arguments();
    QTestEventLoop::instance().exitLoop();
}

void tst_QDBusLocalCalls::initTestCase()
{
    QVERIFY(conn.isConnected());
    QVERIFY(conn.registerObject("/", this, QDBusConnection::ExportScriptableSlots));

    QTest::addColumn<bool>("useAsync");
    QTest::newRow("sync") << false;
    QTest::newRow("async") << true;
}

void tst_QDBusLocalCalls::makeCalls_data()
{
    QTest::addColumn<QVariant>("value");
    QTest::newRow("int") << QVariant(42);
    QTest::newRow("string") << QVariant("Hello, world");
}

void tst_QDBusLocalCalls::makeCallsVariant_data()
{
    makeCalls_data();
}

void tst_QDBusLocalCalls::makeInvalidCalls()
{
    {
        QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                              "/", QString(), "echo");
        QDBusMessage replyMsg = doCall(callMsg);
        QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ErrorMessage));

        QDBusError error(replyMsg);
        QCOMPARE(int(error.type()), int(QDBusError::UnknownMethod));
    }

    {
        QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                              "/no_object", QString(), "echo");
        QDBusMessage replyMsg = doCall(callMsg);
        QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ErrorMessage));

        QDBusError error(replyMsg);
        QCOMPARE(int(error.type()), int(QDBusError::UnknownObject));
    }
}

void tst_QDBusLocalCalls::makeCalls()
{
    QFETCH(QVariant, value);
    QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                          "/", QString(), "echo");
    callMsg << value;
    QDBusMessage replyMsg = doCall(callMsg);

    QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ReplyMessage));

    QVariantList replyArgs = replyMsg.arguments();
    QCOMPARE(replyArgs.count(), 1);
    QCOMPARE(replyArgs.at(0), value);
}

void tst_QDBusLocalCalls::makeCallsVariant()
{
    QFETCH(QVariant, value);
    QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                          "/", QString(), "echo");
    callMsg << qVariantFromValue(QDBusVariant(value));
    QDBusMessage replyMsg = doCall(callMsg);

    QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ReplyMessage));

    QVariantList replyArgs = replyMsg.arguments();
    QCOMPARE(replyArgs.count(), 1);

    const QVariant &reply = replyArgs.at(0);
    QCOMPARE(reply.userType(), qMetaTypeId<QDBusVariant>());
    QCOMPARE(qvariant_cast<QDBusVariant>(reply).variant(), value);
}

void tst_QDBusLocalCalls::makeCallsTwoRets()
{
    QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                          "/", QString(), "echo2");
    callMsg << (QStringList() << "One" << "Two");
    QDBusMessage replyMsg = doCall(callMsg);

    QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ReplyMessage));

    QVariantList replyArgs = replyMsg.arguments();
    QCOMPARE(replyArgs.count(), 2);
    QCOMPARE(replyArgs.at(0).toString(), QString::fromLatin1("One"));
    QCOMPARE(replyArgs.at(1).toString(), QString::fromLatin1("Two"));
}

void tst_QDBusLocalCalls::makeCallsComplex()
{
    qDBusRegisterMetaType<QList<int> >();
    qDBusRegisterMetaType<QVector<int> >();

    QList<int> value;
    value << 1 << -42 << 47;
    QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                          "/", QString(), "echo");
    callMsg << qVariantFromValue(value);
    QDBusMessage replyMsg = doCall(callMsg);

    QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ReplyMessage));

    QVariantList replyArgs = replyMsg.arguments();
    QCOMPARE(replyArgs.count(), 1);
    const QVariant &reply = replyArgs.at(0);
    QCOMPARE(reply.userType(), qMetaTypeId<QDBusArgument>());
    QCOMPARE(qdbus_cast<QList<int> >(reply), value);
}

void tst_QDBusLocalCalls::makeDelayedCalls()
{
    QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                          "/", QString(), "delayed");
    QTest::ignoreMessage(QtWarningMsg, "QDBusConnection: cannot call local method 'delayed' at object / (with signature '') on blocking mode");
    QDBusMessage replyMsg = doCall(callMsg);
    QCOMPARE(int(replyMsg.type()), int(QDBusMessage::ErrorMessage));

    QDBusError error(replyMsg);
    QCOMPARE(int(error.type()), int(QDBusError::InternalError));
}

void tst_QDBusLocalCalls::asyncReplySignal()
{
    QFETCH_GLOBAL(bool, useAsync);
    if (!useAsync)
        return;                 // this test only works in async mode

    QDBusMessage callMsg = QDBusMessage::createMethodCall(conn.baseService(),
                                                          "/", QString(), "echo");
    callMsg << "Hello World";
    QDBusPendingCall ac = conn.asyncCall(callMsg);
    if (ac.isFinished())
        QSKIP("Test ignored: the local-loop async call is already finished", SkipAll);

    QDBusPendingCallWatcher watch(ac);
    connect(&watch, SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(replyReceived(QDBusPendingCallWatcher*)));

    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(ac.isFinished());
    QVERIFY(!ac.isError());

    QVERIFY(!asyncReplyArgs.isEmpty());
    QCOMPARE(asyncReplyArgs.at(0).toString(), QString("Hello World"));
}

QTEST_MAIN(tst_QDBusLocalCalls)
#include "tst_qdbuslocalcalls.moc"
