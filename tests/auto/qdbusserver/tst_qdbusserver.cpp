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

#include <QtTest/QtTest>
#include <QtDBus/QtDBus>

static const QString bus = "unix:path=/tmp/qdbus-test";
static const QString service = "com.trolltech.Qt.Autotests.QDBusServer";
static const QString path = "/com/trolltech/test";

class tst_QDBusServer : public QObject
{
    Q_OBJECT

    void connectToServer();
    void callMethod();
private slots:
};

void tst_QDBusServer::connectToServer()
{
    QDBusConnection connection = QDBusConnection::connectToBus(bus, "test-connection");
    QTest::qWait(100);
    QVERIFY(connection.isConnected());
}

void tst_QDBusServer::callMethod()
{
    QDBusConnection connection = QDBusConnection::connectToBus(bus, "test-connection");
    QTest::qWait(100);
    QVERIFY(connection.isConnected());
    //QDBusMessage msg = QDBusMessage::createMethodCall(bus, path, /*service*/"", "method");
    //QDBusMessage reply = connection.call(msg, QDBus::Block);
}

QTEST_MAIN(tst_QDBusServer)

#include "tst_qdbusserver.moc"
