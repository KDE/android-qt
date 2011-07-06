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

#include <QtCore/QCoreApplication>
#include <QtCore/QSocketNotifier>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#ifdef Q_OS_SYMBIAN
#include <private/qsymbiansocketengine_p.h>
#define NATIVESOCKETENGINE QSymbianSocketEngine
#else
#include <private/qnativesocketengine_p.h>
#define NATIVESOCKETENGINE QNativeSocketEngine
#endif
#ifdef Q_OS_UNIX
#include <private/qnet_unix_p.h>
#include <sys/select.h>
#endif
#include <limits>

class tst_QSocketNotifier : public QObject
{
    Q_OBJECT
public:
    tst_QSocketNotifier();
    ~tst_QSocketNotifier();

private slots:
    void unexpectedDisconnection();
    void mixingWithTimers();
    void posixSockets();
    void bogusFds();
};

tst_QSocketNotifier::tst_QSocketNotifier()
{ }

tst_QSocketNotifier::~tst_QSocketNotifier()
{
}

class UnexpectedDisconnectTester : public QObject
{
    Q_OBJECT
public:
    NATIVESOCKETENGINE *readEnd1, *readEnd2;
    int sequence;

    UnexpectedDisconnectTester(NATIVESOCKETENGINE *s1, NATIVESOCKETENGINE *s2)
        : readEnd1(s1), readEnd2(s2), sequence(0)
    {
        QSocketNotifier *notifier1 =
            new QSocketNotifier(readEnd1->socketDescriptor(), QSocketNotifier::Read, this);
        connect(notifier1, SIGNAL(activated(int)), SLOT(handleActivated()));
        QSocketNotifier *notifier2 =
            new QSocketNotifier(readEnd2->socketDescriptor(), QSocketNotifier::Read, this);
        connect(notifier2, SIGNAL(activated(int)), SLOT(handleActivated()));
    }

public slots:
    void handleActivated()
    {
        char data1[1], data2[1];
        ++sequence;
        if (sequence == 1) {
            // read from both ends
            (void) readEnd1->read(data1, sizeof(data1));
            (void) readEnd2->read(data2, sizeof(data2));
            emit finished();
        } else if (sequence == 2) {
            // we should never get here
            QCOMPARE(readEnd2->read(data2, sizeof(data2)), qint64(-2));
            QVERIFY(readEnd2->isValid());
        }
    }

signals:
    void finished();
};

void tst_QSocketNotifier::unexpectedDisconnection()
{
#ifdef Q_OS_SYMBIAN
    QSKIP("Symbian socket engine pseudo descriptors can't be used for QSocketNotifier", SkipAll);
#else
    /*
      Given two sockets and two QSocketNotifiers registered on each
      their socket. If both sockets receive data, and the first slot
      invoked by one of the socket notifiers empties both sockets, the
      other notifier will also emit activated(). This results in
      unexpected disconnection in QAbstractSocket.

      The use case is that somebody calls one of the
      waitFor... functions in a QSocketNotifier activated slot, and
      the waitFor... functions do local selects that can empty both
      stdin and stderr while waiting for fex bytes to be written.
    */

    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    NATIVESOCKETENGINE readEnd1;
    readEnd1.initialize(QAbstractSocket::TcpSocket);
    bool b = readEnd1.connectToHost(server.serverAddress(), server.serverPort());
    QVERIFY(readEnd1.waitForWrite());
//    while (!b && readEnd1.state() != QAbstractSocket::ConnectedState)
//        b = readEnd1.connectToHost(server.serverAddress(), server.serverPort());
    QVERIFY(readEnd1.state() == QAbstractSocket::ConnectedState);
    QVERIFY(server.waitForNewConnection());
    QTcpSocket *writeEnd1 = server.nextPendingConnection();
    QVERIFY(writeEnd1 != 0);

    NATIVESOCKETENGINE readEnd2;
    readEnd2.initialize(QAbstractSocket::TcpSocket);
    b = readEnd2.connectToHost(server.serverAddress(), server.serverPort());
    QVERIFY(readEnd2.waitForWrite());
//    while (!b)
//        b = readEnd2.connectToHost(server.serverAddress(), server.serverPort());
    QVERIFY(readEnd2.state() == QAbstractSocket::ConnectedState);
    QVERIFY(server.waitForNewConnection());
    QTcpSocket *writeEnd2 = server.nextPendingConnection();
    QVERIFY(writeEnd2 != 0);

    writeEnd1->write("1", 1);
    writeEnd2->write("2", 1);

    writeEnd1->waitForBytesWritten();
    writeEnd2->waitForBytesWritten();

    writeEnd1->flush();
    writeEnd2->flush();

    UnexpectedDisconnectTester tester(&readEnd1, &readEnd2);

    QTimer timer;
    timer.setSingleShot(true);
    timer.start(30000);
    do {
        // we have to wait until sequence value changes
        // as any event can make us jump out processing
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
        QVERIFY(timer.isActive()); //escape if test would hang
    }  while(tester.sequence <= 0);

    QVERIFY(readEnd1.state() == QAbstractSocket::ConnectedState);
    QVERIFY(readEnd2.state() == QAbstractSocket::ConnectedState);

    QCOMPARE(tester.sequence, 2);

    readEnd1.close();
    readEnd2.close();
    writeEnd1->close();
    writeEnd2->close();
    server.close();
#endif
}

class MixingWithTimersHelper : public QObject
{
    Q_OBJECT

public:
    MixingWithTimersHelper(QTimer *timer, QTcpServer *server);

    bool timerActivated;
    bool socketActivated;

private slots:
    void timerFired();
    void socketFired();
};

MixingWithTimersHelper::MixingWithTimersHelper(QTimer *timer, QTcpServer *server)
{
    timerActivated = false;
    socketActivated = false;

    connect(timer, SIGNAL(timeout()), SLOT(timerFired()));
    connect(server, SIGNAL(newConnection()), SLOT(socketFired()));
}

void MixingWithTimersHelper::timerFired()
{
    timerActivated = true;
}

void MixingWithTimersHelper::socketFired()
{
    socketActivated = true;
}

void tst_QSocketNotifier::mixingWithTimers()
{
    QTimer timer;
    timer.setInterval(0);
    timer.start();

    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    MixingWithTimersHelper helper(&timer, &server);

    QCoreApplication::processEvents();

    QCOMPARE(helper.timerActivated, true);
    QCOMPARE(helper.socketActivated, false);

    helper.timerActivated = false;
    helper.socketActivated = false;

    QTcpSocket socket;
    socket.connectToHost(server.serverAddress(), server.serverPort());

    QCoreApplication::processEvents();

    QCOMPARE(helper.timerActivated, true);
    QCOMPARE(helper.socketActivated, true);
}

void tst_QSocketNotifier::posixSockets()
{
#ifndef Q_OS_UNIX
    QSKIP("test only for posix", SkipAll);
#else

    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    int posixSocket = qt_safe_socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(0x7f000001);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server.serverPort());
    qt_safe_connect(posixSocket, (const struct sockaddr*)&addr, sizeof(sockaddr_in));
    QVERIFY(server.waitForNewConnection(5000));
    QScopedPointer<QTcpSocket> passive(server.nextPendingConnection());

    ::fcntl(posixSocket, F_SETFL, ::fcntl(posixSocket, F_GETFL) | O_NONBLOCK);

    {
        QSocketNotifier rn(posixSocket, QSocketNotifier::Read);
        connect(&rn, SIGNAL(activated(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QSignalSpy readSpy(&rn, SIGNAL(activated(int)));
        QSocketNotifier wn(posixSocket, QSocketNotifier::Write);
        connect(&wn, SIGNAL(activated(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QSignalSpy writeSpy(&wn, SIGNAL(activated(int)));
        QSocketNotifier en(posixSocket, QSocketNotifier::Exception);
        connect(&en, SIGNAL(activated(int)), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QSignalSpy errorSpy(&en, SIGNAL(activated(int)));

        passive->write("hello",6);
        passive->waitForBytesWritten(5000);

        QTestEventLoop::instance().enterLoop(3);
        QCOMPARE(readSpy.count(), 1);
        writeSpy.clear(); //depending on OS, write notifier triggers on creation or not.
        QCOMPARE(errorSpy.count(), 0);

        char buffer[100];
        qt_safe_read(posixSocket, buffer, 100);
        QCOMPARE(buffer, "hello");

        qt_safe_write(posixSocket, "goodbye", 8);

        QTestEventLoop::instance().enterLoop(3);
        QCOMPARE(readSpy.count(), 1);
        QCOMPARE(writeSpy.count(), 1);
        QCOMPARE(errorSpy.count(), 0);
        QCOMPARE(passive->readAll(), QByteArray("goodbye",8));
    }
    qt_safe_close(posixSocket);
#endif
}

void tst_QSocketNotifier::bogusFds()
{
#ifndef Q_OS_SYMBIAN
    //behaviour of QSocketNotifier with an invalid fd is totally different across OS
    //main point of this test was to check symbian backend doesn't crash
    QSKIP("test only for symbian", SkipAll);
#else
    QTest::ignoreMessage(QtWarningMsg, "QSocketNotifier: Internal error");
    QSocketNotifier max(std::numeric_limits<int>::max(), QSocketNotifier::Read);
    QTest::ignoreMessage(QtWarningMsg, "QSocketNotifier: Invalid socket specified");
    QTest::ignoreMessage(QtWarningMsg, "QSocketNotifier: Internal error");
    QSocketNotifier min(std::numeric_limits<int>::min(), QSocketNotifier::Write);
    QTest::ignoreMessage(QtWarningMsg, "QSocketNotifier: Internal error");
    //bogus magic number is the first pseudo socket descriptor from symbian socket engine.
    QSocketNotifier bogus(0x40000000, QSocketNotifier::Exception);
    QSocketNotifier largestlegal(FD_SETSIZE - 1, QSocketNotifier::Read);

    QSignalSpy maxspy(&max, SIGNAL(activated(int)));
    QSignalSpy minspy(&min, SIGNAL(activated(int)));
    QSignalSpy bogspy(&bogus, SIGNAL(activated(int)));
    QSignalSpy llspy(&largestlegal, SIGNAL(activated(int)));

    //generate some unrelated socket activity
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost));
    connect(&server, SIGNAL(newConnection()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTcpSocket client;
    client.connectToHost(QHostAddress::LocalHost, server.serverPort());
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(server.hasPendingConnections());

    //check no activity on bogus notifiers
    QCOMPARE(maxspy.count(), 0);
    QCOMPARE(minspy.count(), 0);
    QCOMPARE(bogspy.count(), 0);
    QCOMPARE(llspy.count(), 0);
#endif
}

QTEST_MAIN(tst_QSocketNotifier)
#include <tst_qsocketnotifier.moc>
