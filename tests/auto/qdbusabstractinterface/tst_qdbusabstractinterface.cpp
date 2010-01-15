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
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qsharedpointer.h>

#include <QtTest/QtTest>

#include <QtDBus>

#include "interface.h"
#include "pinger.h"

typedef QSharedPointer<com::trolltech::QtDBus::Pinger> Pinger;

class tst_QDBusAbstractInterface: public QObject
{
    Q_OBJECT
    Interface targetObj;

    Pinger getPinger(QString service = "", const QString &path = "/")
    {
        QDBusConnection con = QDBusConnection::sessionBus();
        if (!con.isConnected())
            return Pinger();
        if (service.isEmpty() && !service.isNull())
            service = con.baseService();
        return Pinger(new com::trolltech::QtDBus::Pinger(service, path, con));
    }

public:
    tst_QDBusAbstractInterface();

private slots:
    void initTestCase();

    void makeVoidCall();
    void makeStringCall();
    void makeComplexCall();
    void makeMultiOutCall();

    void makeAsyncVoidCall();
    void makeAsyncStringCall();
    void makeAsyncComplexCall();
    void makeAsyncMultiOutCall();

    void stringPropRead();
    void stringPropWrite();
    void variantPropRead();
    void variantPropWrite();
    void complexPropRead();
    void complexPropWrite();

    void stringPropDirectRead();
    void stringPropDirectWrite();
    void variantPropDirectRead();
    void variantPropDirectWrite();
    void complexPropDirectRead();
    void complexPropDirectWrite();

    void getVoidSignal_data();
    void getVoidSignal();
    void getStringSignal_data();
    void getStringSignal();
    void getComplexSignal_data();
    void getComplexSignal();

    void followSignal();

    void createErrors_data();
    void createErrors();

    void callErrors_data();
    void callErrors();
    void asyncCallErrors_data();
    void asyncCallErrors();

    void propertyReadErrors_data();
    void propertyReadErrors();
    void propertyWriteErrors_data();
    void propertyWriteErrors();
    void directPropertyReadErrors_data();
    void directPropertyReadErrors();
    void directPropertyWriteErrors_data();
    void directPropertyWriteErrors();
};

tst_QDBusAbstractInterface::tst_QDBusAbstractInterface()
{
    // register the meta types
    qDBusRegisterMetaType<RegisteredType>();
    qRegisterMetaType<UnregisteredType>();
}

void tst_QDBusAbstractInterface::initTestCase()
{
    // enable debugging temporarily:
    //putenv("QDBUS_DEBUG=1");

    // register the object
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());
    con.registerObject("/", &targetObj, QDBusConnection::ExportScriptableContents);
}

void tst_QDBusAbstractInterface::makeVoidCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusReply<void> r = p->voidMethod();
    QVERIFY(r.isValid());
}

void tst_QDBusAbstractInterface::makeStringCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusReply<QString> r = p->stringMethod();
    QVERIFY(r.isValid());
    QCOMPARE(r.value(), targetObj.stringMethod());
}

void tst_QDBusAbstractInterface::makeComplexCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusReply<RegisteredType> r = p->complexMethod();
    QVERIFY(r.isValid());
    QCOMPARE(r.value(), targetObj.complexMethod());
}

void tst_QDBusAbstractInterface::makeMultiOutCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    int value;
    QDBusReply<QString> r = p->multiOutMethod(value);
    QVERIFY(r.isValid());

    int expectedValue;
    QCOMPARE(r.value(), targetObj.multiOutMethod(expectedValue));
    QCOMPARE(value, expectedValue);
}

void tst_QDBusAbstractInterface::makeAsyncVoidCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusPendingReply<void> r = p->voidMethod();
    r.waitForFinished();
    QVERIFY(r.isValid());
}

void tst_QDBusAbstractInterface::makeAsyncStringCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusPendingReply<QString> r = p->stringMethod();
    r.waitForFinished();
    QVERIFY(r.isValid());
    QCOMPARE(r.value(), targetObj.stringMethod());
}

void tst_QDBusAbstractInterface::makeAsyncComplexCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusPendingReply<RegisteredType> r = p->complexMethod();
    r.waitForFinished();
    QVERIFY(r.isValid());
    QCOMPARE(r.value(), targetObj.complexMethod());
}

void tst_QDBusAbstractInterface::makeAsyncMultiOutCall()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusPendingReply<QString, int> r = p->multiOutMethod();
    r.waitForFinished();
    QVERIFY(r.isValid());

    int expectedValue;
    QCOMPARE(r.value(), targetObj.multiOutMethod(expectedValue));
    QCOMPARE(r.argumentAt<1>(), expectedValue);
}

void tst_QDBusAbstractInterface::stringPropRead()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QString expectedValue = targetObj.m_stringProp = "This is a test";
    QVariant v = p->property("stringProp");
    QVERIFY(v.isValid());
    QCOMPARE(v.toString(), expectedValue);
}

void tst_QDBusAbstractInterface::stringPropWrite()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QString expectedValue = "This is a value";
    QVERIFY(p->setProperty("stringProp", expectedValue));
    QCOMPARE(targetObj.m_stringProp, expectedValue);
}

void tst_QDBusAbstractInterface::variantPropRead()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusVariant expectedValue = targetObj.m_variantProp = QDBusVariant(QVariant(42));
    QVariant v = p->property("variantProp");
    QVERIFY(v.isValid());
    QDBusVariant value = v.value<QDBusVariant>();
    QCOMPARE(value.variant().userType(), expectedValue.variant().userType());
    QCOMPARE(value.variant(), expectedValue.variant());
}

void tst_QDBusAbstractInterface::variantPropWrite()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusVariant expectedValue = QDBusVariant(Q_INT64_C(-47));
    QVERIFY(p->setProperty("variantProp", qVariantFromValue(expectedValue)));
    QCOMPARE(targetObj.m_variantProp.variant(), expectedValue.variant());
}

void tst_QDBusAbstractInterface::complexPropRead()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    RegisteredType expectedValue = targetObj.m_complexProp = RegisteredType("This is a test");
    QVariant v = p->property("complexProp");
    QVERIFY(v.userType() == qMetaTypeId<RegisteredType>());
    QCOMPARE(v.value<RegisteredType>(), targetObj.m_complexProp);
}

void tst_QDBusAbstractInterface::complexPropWrite()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    RegisteredType expectedValue = RegisteredType("This is a value");
    QVERIFY(p->setProperty("complexProp", qVariantFromValue(expectedValue)));
    QCOMPARE(targetObj.m_complexProp, expectedValue);
}

void tst_QDBusAbstractInterface::stringPropDirectRead()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QString expectedValue = targetObj.m_stringProp = "This is a test";
    QCOMPARE(p->stringProp(), expectedValue);
}

void tst_QDBusAbstractInterface::stringPropDirectWrite()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QString expectedValue = "This is a value";
    p->setStringProp(expectedValue);
    QCOMPARE(targetObj.m_stringProp, expectedValue);
}

void tst_QDBusAbstractInterface::variantPropDirectRead()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusVariant expectedValue = targetObj.m_variantProp = QDBusVariant(42);
    QCOMPARE(p->variantProp().variant(), expectedValue.variant());
}

void tst_QDBusAbstractInterface::variantPropDirectWrite()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusVariant expectedValue = QDBusVariant(Q_INT64_C(-47));
    p->setVariantProp(expectedValue);
    QCOMPARE(targetObj.m_variantProp.variant().userType(), expectedValue.variant().userType());
    QCOMPARE(targetObj.m_variantProp.variant(), expectedValue.variant());
}

void tst_QDBusAbstractInterface::complexPropDirectRead()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    RegisteredType expectedValue = targetObj.m_complexProp = RegisteredType("This is a test");
    QCOMPARE(p->complexProp(), targetObj.m_complexProp);
}

void tst_QDBusAbstractInterface::complexPropDirectWrite()
{
    Pinger p = getPinger();
    QVERIFY2(p, "Not connected to D-Bus");

    RegisteredType expectedValue = RegisteredType("This is a value");
    p->setComplexProp(expectedValue);
    QCOMPARE(targetObj.m_complexProp, expectedValue);
}

void tst_QDBusAbstractInterface::getVoidSignal_data()
{
    QTest::addColumn<QString>("service");
    QTest::addColumn<QString>("path");

    QTest::newRow("specific") << QDBusConnection::sessionBus().baseService() << "/";
    QTest::newRow("service-wildcard") << QString() << "/";
    QTest::newRow("path-wildcard") << QDBusConnection::sessionBus().baseService() << QString();
    QTest::newRow("full-wildcard") << QString() << QString();
}

void tst_QDBusAbstractInterface::getVoidSignal()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we need to connect the signal somewhere in order for D-Bus to enable the rules
    QTestEventLoop::instance().connect(p.data(), SIGNAL(voidSignal()), SLOT(exitLoop()));
    QSignalSpy s(p.data(), SIGNAL(voidSignal()));

    emit targetObj.voidSignal();
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(s.size() == 1);
    QVERIFY(s.at(0).size() == 0);
}

void tst_QDBusAbstractInterface::getStringSignal_data()
{
    getVoidSignal_data();
}

void tst_QDBusAbstractInterface::getStringSignal()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we need to connect the signal somewhere in order for D-Bus to enable the rules
    QTestEventLoop::instance().connect(p.data(), SIGNAL(stringSignal(QString)), SLOT(exitLoop()));
    QSignalSpy s(p.data(), SIGNAL(stringSignal(QString)));

    QString expectedValue = "Good morning";
    emit targetObj.stringSignal(expectedValue);
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(s.size() == 1);
    QVERIFY(s[0].size() == 1);
    QCOMPARE(s[0][0].userType(), int(QVariant::String));
    QCOMPARE(s[0][0].toString(), expectedValue);
}

void tst_QDBusAbstractInterface::getComplexSignal_data()
{
    getVoidSignal_data();
}

void tst_QDBusAbstractInterface::getComplexSignal()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we need to connect the signal somewhere in order for D-Bus to enable the rules
    QTestEventLoop::instance().connect(p.data(), SIGNAL(complexSignal(RegisteredType)), SLOT(exitLoop()));
    QSignalSpy s(p.data(), SIGNAL(complexSignal(RegisteredType)));

    RegisteredType expectedValue("Good evening");
    emit targetObj.complexSignal(expectedValue);
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(s.size() == 1);
    QVERIFY(s[0].size() == 1);
    QCOMPARE(s[0][0].userType(), qMetaTypeId<RegisteredType>());
    QCOMPARE(s[0][0].value<RegisteredType>(), expectedValue);
}

void tst_QDBusAbstractInterface::followSignal()
{
    const QString serviceToFollow = "com.trolltech.tst_qdbusabstractinterface.FollowMe";
    Pinger p = getPinger(serviceToFollow);
    QVERIFY2(p, "Not connected to D-Bus");

    QDBusConnection con = p->connection();
    QVERIFY(!con.interface()->isServiceRegistered(serviceToFollow));
    Pinger control = getPinger("");

    // we need to connect the signal somewhere in order for D-Bus to enable the rules
    QTestEventLoop::instance().connect(p.data(), SIGNAL(voidSignal()), SLOT(exitLoop()));
    QTestEventLoop::instance().connect(control.data(), SIGNAL(voidSignal()), SLOT(exitLoop()));
    QSignalSpy s(p.data(), SIGNAL(voidSignal()));

    emit targetObj.voidSignal();
    QTestEventLoop::instance().enterLoop(200);
    QVERIFY(!QTestEventLoop::instance().timeout());

    // signal must not have been received because the service isn't registered
    QVERIFY(s.isEmpty());

    // now register the service
    QDBusReply<QDBusConnectionInterface::RegisterServiceReply> r =
            con.interface()->registerService(serviceToFollow, QDBusConnectionInterface::DontQueueService,
                                             QDBusConnectionInterface::DontAllowReplacement);
    QVERIFY(r.isValid() && r.value() == QDBusConnectionInterface::ServiceRegistered);
    QVERIFY(con.interface()->isServiceRegistered(serviceToFollow));
    QCoreApplication::instance()->processEvents();

    // emit the signal again:
    emit targetObj.voidSignal();
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    // now the signal must have been received:
    QCOMPARE(s.size(), 1);
    QVERIFY(s.at(0).size() == 0);
    s.clear();

    // disconnect the signal
    disconnect(p.data(), SIGNAL(voidSignal()), &QTestEventLoop::instance(), 0);

    // emit the signal again:
    emit targetObj.voidSignal();
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    // and now it mustn't have been received
    QVERIFY(s.isEmpty());

    // cleanup:
    con.interface()->unregisterService(serviceToFollow);
}

void tst_QDBusAbstractInterface::createErrors_data()
{
    QTest::addColumn<QString>("service");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("errorName");

    QTest::newRow("invalid-service") << "this isn't valid" << "/" << "com.trolltech.QtDBus.Error.InvalidService";
    QTest::newRow("invalid-path") << QDBusConnection::sessionBus().baseService() << "this isn't valid"
            << "com.trolltech.QtDBus.Error.InvalidObjectPath";
}

void tst_QDBusAbstractInterface::createErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    QVERIFY(!p->isValid());
    QTEST(p->lastError().name(), "errorName");
}

void tst_QDBusAbstractInterface::callErrors_data()
{
    createErrors_data();
    QTest::newRow("service-wildcard") << QString() << "/" << "com.trolltech.QtDBus.Error.InvalidService";
    QTest::newRow("path-wildcard") << QDBusConnection::sessionBus().baseService() << QString()
            << "com.trolltech.QtDBus.Error.InvalidObjectPath";
    QTest::newRow("full-wildcard") << QString() << QString() << "com.trolltech.QtDBus.Error.InvalidService";
}

void tst_QDBusAbstractInterface::callErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we shouldn't be able to make this call:
    QDBusReply<QString> r = p->stringMethod();
    QVERIFY(!r.isValid());
    QTEST(r.error().name(), "errorName");
    QCOMPARE(p->lastError().name(), r.error().name());
}

void tst_QDBusAbstractInterface::asyncCallErrors_data()
{
    callErrors_data();
}

void tst_QDBusAbstractInterface::asyncCallErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we shouldn't be able to make this call:
    QDBusPendingReply<QString> r = p->stringMethod();
    QVERIFY(r.isError());
    QTEST(r.error().name(), "errorName");
    QCOMPARE(p->lastError().name(), r.error().name());
}

void tst_QDBusAbstractInterface::propertyReadErrors_data()
{
    callErrors_data();
}

void tst_QDBusAbstractInterface::propertyReadErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we shouldn't be able to get this value:
    QVariant v = p->property("stringProp");
    QVERIFY(v.isNull());
    QVERIFY(!v.isValid());
    QTEST(p->lastError().name(), "errorName");
}

void tst_QDBusAbstractInterface::propertyWriteErrors_data()
{
    callErrors_data();
}

void tst_QDBusAbstractInterface::propertyWriteErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we shouldn't be able to get this value:
    if (p->isValid())
        QCOMPARE(int(p->lastError().type()), int(QDBusError::NoError));
    QVERIFY(!p->setProperty("stringProp", ""));
    QTEST(p->lastError().name(), "errorName");
}

void tst_QDBusAbstractInterface::directPropertyReadErrors_data()
{
    callErrors_data();
}

void tst_QDBusAbstractInterface::directPropertyReadErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we shouldn't be able to get this value:
    QString v = p->stringProp();
    QVERIFY(v.isNull());
    QTEST(p->lastError().name(), "errorName");
}

void tst_QDBusAbstractInterface::directPropertyWriteErrors_data()
{
    callErrors_data();
}

void tst_QDBusAbstractInterface::directPropertyWriteErrors()
{
    QFETCH(QString, service);
    QFETCH(QString, path);
    Pinger p = getPinger(service, path);
    QVERIFY2(p, "Not connected to D-Bus");

    // we shouldn't be able to get this value:
    // but there's no direct way of verifying that the setting failed
    if (p->isValid())
        QCOMPARE(int(p->lastError().type()), int(QDBusError::NoError));
    p->setStringProp("");
    QTEST(p->lastError().name(), "errorName");
}

QTEST_MAIN(tst_QDBusAbstractInterface)
#include "tst_qdbusabstractinterface.moc"
