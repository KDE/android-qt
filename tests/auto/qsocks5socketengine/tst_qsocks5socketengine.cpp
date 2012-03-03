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

#include <private/qsocks5socketengine_p.h>
#include <qhostinfo.h>
#include <qhostaddress.h>
#include <qtcpsocket.h>
#include <qhttp.h>
#include <qauthenticator.h>
#include <qdebug.h>
#include <qtcpserver.h>
#include <qmetatype.h>
#include <qdebug.h>

#include "../network-settings.h"

Q_DECLARE_METATYPE(QQueue<QByteArray>)

class tst_QSocks5SocketEngine : public QObject, public QAbstractSocketEngineReceiver
{
    Q_OBJECT

public:
    tst_QSocks5SocketEngine();
    virtual ~tst_QSocks5SocketEngine();


public slots:
    void init();
    void cleanup();
private slots:
    void construction();
    void errorTest_data();
    void errorTest();
    void simpleConnectToIMAP();
    void simpleErrorsAndStates();
    void udpTest();
    void serverTest();
    void tcpSocketBlockingTest();
    void tcpSocketNonBlockingTest();
    void downloadBigFile();
   // void tcpLoopbackPerformance();
    void passwordAuth();
    void passwordAuth2();

protected slots:
    void tcpSocketNonBlocking_hostFound();
    void tcpSocketNonBlocking_connected();
    void tcpSocketNonBlocking_closed();
    void tcpSocketNonBlocking_readyRead();
    void tcpSocketNonBlocking_bytesWritten(qint64);
    void exitLoopSlot();
    void downloadBigFileSlot();
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);

private:
    void readNotification() { }
    void writeNotification() { }
    void exceptionNotification() { }
    void connectionNotification() { }
    QTcpSocket *tcpSocketNonBlocking_socket;
    QStringList tcpSocketNonBlocking_data;
    qint64 tcpSocketNonBlocking_totalWritten;
    QTcpSocket *tmpSocket;
    qint64 bytesAvailable;
};

class MiniSocks5Server: public QTcpServer
{
    Q_OBJECT
public:
    QQueue<QByteArray> responses;

    MiniSocks5Server(const QQueue<QByteArray> r)
        : responses(r)
    {
        listen();
        connect(this, SIGNAL(newConnection()), SLOT(handleNewConnection()));
    }

private slots:
    void handleNewConnection()
    {
        QTcpSocket *client = nextPendingConnection();
        connect(client, SIGNAL(readyRead()), SLOT(handleClientCommand()));
        client->setProperty("pendingResponses", qVariantFromValue(responses));
    }

    void handleClientCommand()
    {
        // WARNING
        // this assumes that the client command is received in its entirety
        // should be ok, since SOCKSv5 commands are rather small
        QTcpSocket *client = static_cast<QTcpSocket *>(sender());
        QQueue<QByteArray> pendingResponses =
            qvariant_cast<QQueue<QByteArray> >(client->property("pendingResponses"));
        if (pendingResponses.isEmpty())
            client->disconnectFromHost();
        else
            client->write(pendingResponses.dequeue());
        client->setProperty("pendingResponses", qVariantFromValue(pendingResponses));
    }
};

tst_QSocks5SocketEngine::tst_QSocks5SocketEngine()
{
    Q_SET_DEFAULT_IAP
}

tst_QSocks5SocketEngine::~tst_QSocks5SocketEngine()
{
}

void tst_QSocks5SocketEngine::init()
{
    tmpSocket = 0;
    bytesAvailable = 0;
}

void tst_QSocks5SocketEngine::cleanup()
{
}

//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::construction()
{
    QSocks5SocketEngine socketDevice;

    QVERIFY(!socketDevice.isValid());

    // Initialize device
    QVERIFY(socketDevice.initialize(QAbstractSocket::TcpSocket, QAbstractSocket::IPv4Protocol));
    QVERIFY(socketDevice.isValid());
    QVERIFY(socketDevice.protocol() == QAbstractSocket::IPv4Protocol);
    QVERIFY(socketDevice.socketType() == QAbstractSocket::TcpSocket);
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);
   // QVERIFY(socketDevice.socketDescriptor() != -1);
    QVERIFY(socketDevice.localAddress() == QHostAddress());
    QVERIFY(socketDevice.localPort() == 0);
    QVERIFY(socketDevice.peerAddress() == QHostAddress());
    QVERIFY(socketDevice.peerPort() == 0);
    QVERIFY(socketDevice.error() == QAbstractSocket::UnknownSocketError);

    //QTest::ignoreMessage(QtWarningMsg, "QSocketLayer::bytesAvailable() was called in QAbstractSocket::UnconnectedState");
    QVERIFY(socketDevice.bytesAvailable() == 0);

    //QTest::ignoreMessage(QtWarningMsg, "QSocketLayer::hasPendingDatagrams() was called in QAbstractSocket::UnconnectedState");
    QVERIFY(!socketDevice.hasPendingDatagrams());
}

//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::errorTest_data()
{
    QTest::addColumn<QString>("hostname");
    QTest::addColumn<int>("port");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QQueue<QByteArray> >("responses");
    QTest::addColumn<int>("expectedError");

    QQueue<QByteArray> responses;
    QTest::newRow("proxy-host-not-found") << "this-host-does-not-exist." << 1080 << QString()
                                          << responses
                                          << int(QAbstractSocket::ProxyNotFoundError);
    QTest::newRow("proxy-connection-refused") << "127.0.0.1" << 2 << QString()
                                              << responses
                                              << int(QAbstractSocket::ProxyConnectionRefusedError);

#define REPLY(name, contents)                                           \
    static const char raw_ ## name [] = contents;                       \
    const QByteArray name = QByteArray::fromRawData(raw_ ## name, sizeof raw_ ## name - 1)

    REPLY(garbage, "\4\4\4\4");
    // authentication method replies
    REPLY(noAuthentication, "\5\0");
    REPLY(passwordAuthentication, "\5\2");
    REPLY(garbageAuthentication, "\5\177");
    REPLY(noAcceptableAuthentication, "\5\377");
    // authentication replies
    REPLY(authenticationAccepted, "\5\0");
    REPLY(authenticationNotAccepted, "\5\1");
    // connection replies
    REPLY(connectionAccepted, "\5\0\0\4\177\0\0\1\0\100");
    REPLY(connectionNotAllowed, "\5\2\0");
    REPLY(networkUnreachable, "\5\3\0");
    REPLY(hostUnreachable, "\5\4\0");
    REPLY(connectionRefused, "\5\5\0");

#undef REPLY

    responses << garbage;
    QTest::newRow("garbage1") << QString() << 0 << QString() << responses
                              << int(QAbstractSocket::ProxyProtocolError);

    responses.clear();
    responses << noAuthentication << garbage;
    QTest::newRow("garbage2") << QString() << 0 << QString() << responses
                              << int(QAbstractSocket::ProxyProtocolError);

    responses.clear();
    responses << garbageAuthentication;
    QTest::newRow("unknown-auth-method") << QString() << 0 << QString()
                                         << responses
                                         << int(QAbstractSocket::SocketAccessError);

    responses.clear();
    responses << noAcceptableAuthentication;
    QTest::newRow("no-acceptable-authentication") << QString() << 0 << QString()
                                                  << responses
                                                  << int(QAbstractSocket::ProxyAuthenticationRequiredError);

    responses.clear();
    responses << passwordAuthentication << authenticationNotAccepted;
    QTest::newRow("authentication-required") << QString() << 0 << "foo"
                                             << responses
                                             << int(QAbstractSocket::ProxyAuthenticationRequiredError);

    responses.clear();
    responses << noAuthentication << connectionNotAllowed;
    QTest::newRow("connection-not-allowed") << QString() << 0 << QString()
                                            << responses
                                            << int(QAbstractSocket::SocketAccessError);

    responses.clear();
    responses << noAuthentication << networkUnreachable;
    QTest::newRow("network-unreachable") << QString() << 0 << QString()
                                         << responses
                                         << int(QAbstractSocket::NetworkError);

    responses.clear();
    responses << noAuthentication << hostUnreachable;
    QTest::newRow("host-unreachable") << QString() << 0 << QString()
                                      << responses
                                      << int(QAbstractSocket::HostNotFoundError);

    responses.clear();
    responses << noAuthentication << connectionRefused;
    QTest::newRow("connection-refused") << QString() << 0 << QString()
                                        << responses
                                        << int(QAbstractSocket::ConnectionRefusedError);
}

void tst_QSocks5SocketEngine::errorTest()
{
    QFETCH(QString, hostname);
    QFETCH(int, port);
    QFETCH(QString, username);
    QFETCH(QQueue<QByteArray>, responses);
    QFETCH(int, expectedError);

    MiniSocks5Server server(responses);

    if (hostname.isEmpty()) {
        hostname = "127.0.0.1";
        port = server.serverPort();
    }
    QTcpSocket socket;
    socket.setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, hostname, port, username, username));
    socket.connectToHost("0.1.2.3", 12345);

    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(int(socket.error()), expectedError);
}

//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::simpleConnectToIMAP()
{
    QSocks5SocketEngine socketDevice;

    // Initialize device
    QVERIFY(socketDevice.initialize(QAbstractSocket::TcpSocket, QAbstractSocket::IPv4Protocol));
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);

    socketDevice.setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080));

    QVERIFY(!socketDevice.connectToHost(QtNetworkSettings::serverIP(), 143));
    QVERIFY(socketDevice.state() == QAbstractSocket::ConnectingState);
    QVERIFY(socketDevice.waitForWrite());
    QVERIFY(socketDevice.state() == QAbstractSocket::ConnectedState);
    QVERIFY(socketDevice.peerAddress() == QtNetworkSettings::serverIP());

    // Wait for the greeting
    QVERIFY(socketDevice.waitForRead());

    // Read the greeting
    qint64 available = socketDevice.bytesAvailable();
    QVERIFY(available > 0);
    QByteArray array;
    array.resize(available);
    QVERIFY(socketDevice.read(array.data(), array.size()) == available);

    // Check that the greeting is what we expect it to be
    QVERIFY2(QtNetworkSettings::compareReplyIMAP(array), array.constData());

    // Write a logout message
    QByteArray array2 = "XXXX LOGOUT\r\n";
    QVERIFY(socketDevice.write(array2.data(),
                              array2.size()) == array2.size());

    // Wait for the response
    QVERIFY(socketDevice.waitForRead());

    available = socketDevice.bytesAvailable();
    QVERIFY(available > 0);
    array.resize(available);
    QVERIFY(socketDevice.read(array.data(), array.size()) == available);

    // Check that the greeting is what we expect it to be
    QCOMPARE(array.constData(), "* BYE LOGOUT received\r\nXXXX OK Completed\r\n");

    // Wait for the response
    QVERIFY(socketDevice.waitForRead());
    char c;
    QVERIFY(socketDevice.read(&c, sizeof(c)) == -1);
    QVERIFY(socketDevice.error() == QAbstractSocket::RemoteHostClosedError);
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);
}

//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::simpleErrorsAndStates()
{
    {
        QSocks5SocketEngine socketDevice;

        // Initialize device
        QVERIFY(socketDevice.initialize(QAbstractSocket::TcpSocket, QAbstractSocket::IPv4Protocol));

        socketDevice.setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080));

        QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);
        QVERIFY(!socketDevice.connectToHost(QHostInfo::fromName(QtNetworkSettings::serverName()).addresses().first(), 8088));
        QVERIFY(socketDevice.state() == QAbstractSocket::ConnectingState);
        if (socketDevice.waitForWrite(15000)) {
            QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState ||
                    socketDevice.state() == QAbstractSocket::ConnectedState);
        } else {
            QVERIFY(socketDevice.error() == QAbstractSocket::SocketTimeoutError);
        }
    }

}

/*
//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::tcpLoopbackPerformance()
{
    QTcpServer server;

    // Bind to any port on all interfaces
    QVERIFY(server.bind(QHostAddress("0.0.0.0"), 0));
    QVERIFY(server.state() == QAbstractSocket::BoundState);
    quint16 port = server.localPort();

    // Listen for incoming connections
    QVERIFY(server.listen());
    QVERIFY(server.state() == QAbstractSocket::ListeningState);

    // Initialize a Tcp socket
    QSocks5SocketEngine client;
    QVERIFY(client.initialize(QAbstractSocket::TcpSocket));

    client.setProxy(QHostAddress("80.232.37.158"), 1081);

    // Connect to our server
    if (!client.connectToHost(QHostAddress("127.0.0.1"), port)) {
        QVERIFY(client.waitForWrite());
        QVERIFY(client.connectToHost(QHostAddress("127.0.0.1"), port));
    }

    // The server accepts the connectio
    int socketDescriptor = server.accept();
    QVERIFY(socketDescriptor > 0);

    // A socket device is initialized on the server side, passing the
    // socket descriptor from accept(). It's pre-connected.
    QSocketLayer serverSocket;
    QVERIFY(serverSocket.initialize(socketDescriptor));
    QVERIFY(serverSocket.state() == QAbstractSocket::ConnectedState);

    const int messageSize = 1024 * 256;
    QByteArray message1(messageSize, '@');
    QByteArray answer(messageSize, '@');

    QTime timer;
    timer.start();
    qlonglong readBytes = 0;
    while (timer.elapsed() < 5000) {
        qlonglong written = serverSocket.write(message1.data(), message1.size());
        while (written > 0) {
            client.waitForRead();
            if (client.bytesAvailable() > 0) {
                qlonglong readNow = client.read(answer.data(), answer.size());
                written -= readNow;
                readBytes += readNow;
            }
        }
    }

    qDebug("\t\t%.1fMB/%.1fs: %.1fMB/s",
           readBytes / (1024.0 * 1024.0),
           timer.elapsed() / 1024.0,
           (readBytes / (timer.elapsed() / 1000.0)) / (1024 * 1024));
}
*/

//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::serverTest()
{
    QSocks5SocketEngine server;

    // Initialize a Tcp socket
    QVERIFY(server.initialize(QAbstractSocket::TcpSocket));

    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080);

    server.setProxy(proxy);

    // Bind to any port on all interfaces
    QVERIFY(server.bind(QHostAddress("0.0.0.0"), 0));
    QVERIFY(server.state() == QAbstractSocket::BoundState);

    // Listen for incoming connections
    QVERIFY(server.listen());
    QVERIFY(server.state() == QAbstractSocket::ListeningState);

    // Initialize a Tcp socket
    QSocks5SocketEngine client;
    QVERIFY(client.initialize(QAbstractSocket::TcpSocket));

    client.setProxy(proxy);

   // QTest::wait(100000); // ### timing problem on win32


    // Connect to our server
    if (!client.connectToHost(server.localAddress(), server.localPort())) {
        QVERIFY(client.waitForWrite());
       // QTest::wait(100); // ### timing problem on win32
        QVERIFY(client.state() == QAbstractSocket::ConnectedState);
        //QTest::wait(100);
    }

    QVERIFY(server.waitForRead());

    // The server accepts the connection
    int socketDescriptor = server.accept();
    QVERIFY(socketDescriptor > 0);

    // A socket device is initialized on the server side, passing the
    // socket descriptor from accept(). It's pre-connected.

    QSocks5SocketEngine serverSocket;
    QVERIFY(serverSocket.initialize(socketDescriptor));
    QVERIFY(serverSocket.state() == QAbstractSocket::ConnectedState);

    QVERIFY(serverSocket.localAddress() == client.peerAddress());
    QVERIFY(serverSocket.localPort() == client.peerPort());
    // this seems depends on the socks server implementation, especially
    // when connecting /to/ the socks server /through/ the same socks server
    //QVERIFY(serverSocket.peerAddress() == client.localAddress());
    //QVERIFY(serverSocket.peerPort() == client.localPort());

    // The server socket sends a greeting to the client
    QByteArray greeting = "Greetings!";
    QVERIFY(serverSocket.write(greeting.data(),
                              greeting.size()) == greeting.size());

    // The client waits for the greeting to arrive
    QVERIFY(client.waitForRead());
    qint64 available = client.bytesAvailable();
    QVERIFY(available > 0);

    // The client reads the greeting and checks that it's correct
    QByteArray response;
    response.resize(available);
    QVERIFY(client.read(response.data(),
                       response.size()) == response.size());
    QCOMPARE(response, greeting);
}


//---------------------------------------------------------------------------
void tst_QSocks5SocketEngine::udpTest()
{
#ifdef SYMBIAN_WINSOCK_CONNECTIVITY
    QSKIP("UDP works bads on non WinPCAP emulator setting", SkipAll);
#endif

    QSocks5SocketEngine udpSocket;

    // Initialize device #1
    QVERIFY(udpSocket.initialize(QAbstractSocket::UdpSocket));
    QVERIFY(udpSocket.isValid());

    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080);

    udpSocket.setProxy(proxy);

    QVERIFY(udpSocket.protocol() == QAbstractSocket::IPv4Protocol);
    QVERIFY(udpSocket.socketType() == QAbstractSocket::UdpSocket);
    QVERIFY(udpSocket.state() == QAbstractSocket::UnconnectedState);

    // Bind #1
    QVERIFY(udpSocket.bind(QHostAddress("0.0.0.0"), 0));
    QVERIFY(udpSocket.state() == QAbstractSocket::BoundState);
    QVERIFY(udpSocket.localPort() != 0);

    // Initialize device #2
    QSocks5SocketEngine udpSocket2;
    QVERIFY(udpSocket2.initialize(QAbstractSocket::UdpSocket));

    udpSocket2.setProxy(proxy);

    // Connect device #2 to #1
    QVERIFY(udpSocket2.connectToHost(udpSocket.localAddress(), udpSocket.localPort()));
    QVERIFY(udpSocket2.state() == QAbstractSocket::ConnectedState);

    // Write a message to #1
    QByteArray message1 = "hei der";
    QVERIFY(udpSocket2.write(message1.data(),
                            message1.size()) == message1.size());

    // Read the message from #2
    QVERIFY(udpSocket.waitForRead());
    QVERIFY(udpSocket.hasPendingDatagrams());
    qint64 available = udpSocket.pendingDatagramSize();
    QVERIFY(available > 0);
    QByteArray answer;
    answer.resize(available);
    QHostAddress senderAddress;
    quint16 senderPort = 0;
    QVERIFY(udpSocket.readDatagram(answer.data(), answer.size(),
                                  &senderAddress,
                                  &senderPort) == message1.size());
    QVERIFY(senderAddress == udpSocket2.localAddress());
    QVERIFY(senderPort == udpSocket2.localPort());
}

void tst_QSocks5SocketEngine::tcpSocketBlockingTest()
{
    QSocks5SocketEngineHandler socks5;

    QTcpSocket socket;

    // Connect
    socket.connectToHost(QtNetworkSettings::serverName(), 143);
    QVERIFY(socket.waitForConnected());
    QCOMPARE(socket.state(), QTcpSocket::ConnectedState);

    // Read greeting
    QVERIFY(socket.waitForReadyRead(5000));
    QString s = socket.readLine();
    QVERIFY2(QtNetworkSettings::compareReplyIMAP(s.toLatin1()), s.toLatin1().constData());

    // Write NOOP
    QCOMPARE((int) socket.write("1 NOOP\r\n", 8), 8);

    if (!socket.canReadLine())
        QVERIFY(socket.waitForReadyRead(5000));

    // Read response
    s = socket.readLine();
    QCOMPARE(s.toLatin1().constData(), "1 OK Completed\r\n");

    // Write LOGOUT
    QCOMPARE((int) socket.write("2 LOGOUT\r\n", 10), 10);

    if (!socket.canReadLine())
        QVERIFY(socket.waitForReadyRead(5000));

    // Read two lines of respose
    s = socket.readLine();
    QCOMPARE(s.toLatin1().constData(), "* BYE LOGOUT received\r\n");

    if (!socket.canReadLine())
        QVERIFY(socket.waitForReadyRead(5000));

    s = socket.readLine();
    QCOMPARE(s.toLatin1().constData(), "2 OK Completed\r\n");

    // Close the socket
    socket.close();

    // Check that it's closed
    QCOMPARE(socket.state(), QTcpSocket::UnconnectedState);
}

//----------------------------------------------------------------------------------

void tst_QSocks5SocketEngine::tcpSocketNonBlockingTest()
{
    QSocks5SocketEngineHandler socks5;

    QTcpSocket socket;
    connect(&socket, SIGNAL(hostFound()), SLOT(tcpSocketNonBlocking_hostFound()));
    connect(&socket, SIGNAL(connected()), SLOT(tcpSocketNonBlocking_connected()));
    connect(&socket, SIGNAL(disconnected()), SLOT(tcpSocketNonBlocking_closed()));
    connect(&socket, SIGNAL(bytesWritten(qint64)), SLOT(tcpSocketNonBlocking_bytesWritten(qint64)));
    connect(&socket, SIGNAL(readyRead()), SLOT(tcpSocketNonBlocking_readyRead()));
    tcpSocketNonBlocking_socket = &socket;

    // Connect
    socket.connectToHost(QtNetworkSettings::serverName(), 143);
    QVERIFY(socket.state() == QTcpSocket::HostLookupState ||
            socket.state() == QTcpSocket::ConnectingState);

    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout()) {
        QFAIL("Timed out");
    }

    if (socket.state() == QTcpSocket::ConnectingState) {
        QTestEventLoop::instance().enterLoop(30);
        if (QTestEventLoop::instance().timeout()) {
            QFAIL("Timed out");
        }
    }

    QCOMPARE(socket.state(), QTcpSocket::ConnectedState);

    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout()) {
        QFAIL("Timed out");
    }

    // Read greeting
    QVERIFY(!tcpSocketNonBlocking_data.isEmpty());
    QByteArray data = tcpSocketNonBlocking_data.at(0).toLatin1();
    QVERIFY2(QtNetworkSettings::compareReplyIMAP(data), data.constData());

    tcpSocketNonBlocking_data.clear();

    tcpSocketNonBlocking_totalWritten = 0;

    // Write NOOP
    QCOMPARE((int) socket.write("1 NOOP\r\n", 8), 8);


    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout()) {
        QFAIL("Timed out");
    }

    QVERIFY(tcpSocketNonBlocking_totalWritten == 8);


    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout()) {
        QFAIL("Timed out");
    }

    // Read response
    QVERIFY(!tcpSocketNonBlocking_data.isEmpty());
    QCOMPARE(tcpSocketNonBlocking_data.at(0).toLatin1().constData(), "1 OK Completed\r\n");
    tcpSocketNonBlocking_data.clear();


    tcpSocketNonBlocking_totalWritten = 0;

    // Write LOGOUT
    QCOMPARE((int) socket.write("2 LOGOUT\r\n", 10), 10);

    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout()) {
        QFAIL("Timed out");
    }

    QVERIFY(tcpSocketNonBlocking_totalWritten == 10);

    // Wait for greeting
    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout()) {
        QFAIL("Timed out");
    }

    // Read two lines of respose
    QCOMPARE(tcpSocketNonBlocking_data.at(0).toLatin1().constData(), "* BYE LOGOUT received\r\n");
    QCOMPARE(tcpSocketNonBlocking_data.at(1).toLatin1().constData(), "2 OK Completed\r\n");
    tcpSocketNonBlocking_data.clear();

    // Close the socket
    socket.close();

    // Check that it's closed
    QCOMPARE(socket.state(), QTcpSocket::UnconnectedState);
}

void tst_QSocks5SocketEngine::tcpSocketNonBlocking_hostFound()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_QSocks5SocketEngine::tcpSocketNonBlocking_connected()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_QSocks5SocketEngine::tcpSocketNonBlocking_readyRead()
{
    while (tcpSocketNonBlocking_socket->canReadLine())
        tcpSocketNonBlocking_data.append(tcpSocketNonBlocking_socket->readLine());

    QTestEventLoop::instance().exitLoop();
}

void tst_QSocks5SocketEngine::tcpSocketNonBlocking_bytesWritten(qint64 written)
{
    tcpSocketNonBlocking_totalWritten += written;
    QTestEventLoop::instance().exitLoop();
}

void tst_QSocks5SocketEngine::tcpSocketNonBlocking_closed()
{
}

//----------------------------------------------------------------------------------

void tst_QSocks5SocketEngine::downloadBigFile()
{
    QSocks5SocketEngineHandler socks5;

    if (tmpSocket)
        delete tmpSocket;
    tmpSocket = new QTcpSocket;

    connect(tmpSocket, SIGNAL(connected()), SLOT(exitLoopSlot()));
    connect(tmpSocket, SIGNAL(readyRead()), SLOT(downloadBigFileSlot()));

    tmpSocket->connectToHost(QtNetworkSettings::serverName(), 80);

    QTestEventLoop::instance().enterLoop(30);
    if (QTestEventLoop::instance().timeout())
        QFAIL("Network operation timed out");

    QByteArray hostName = QtNetworkSettings::serverName().toLatin1();
    QVERIFY(tmpSocket->state() == QAbstractSocket::ConnectedState);
    QVERIFY(tmpSocket->write("GET /qtest/mediumfile HTTP/1.0\r\n") > 0);
    QVERIFY(tmpSocket->write("HOST: ") > 0);
    QVERIFY(tmpSocket->write(hostName.data()) > 0);
    QVERIFY(tmpSocket->write("\r\n") > 0);
    QVERIFY(tmpSocket->write("\r\n") > 0);

    bytesAvailable = 0;

    QTime stopWatch;
    stopWatch.start();

#if !defined(Q_OS_WINCE) && !defined(Q_OS_SYMBIAN)
    QTestEventLoop::instance().enterLoop(60);
#else
    QTestEventLoop::instance().enterLoop(180);
#endif
    if (QTestEventLoop::instance().timeout())
        QFAIL("Network operation timed out");

    QCOMPARE(bytesAvailable, qint64(10000000));

    QVERIFY(tmpSocket->state() == QAbstractSocket::ConnectedState);

    /*qDebug("\t\t%.1fMB/%.1fs: %.1fMB/s",
           bytesAvailable / (1024.0 * 1024.0),
           stopWatch.elapsed() / 1024.0,
           (bytesAvailable / (stopWatch.elapsed() / 1000.0)) / (1024 * 1024));*/

    delete tmpSocket;
    tmpSocket = 0;
}

void tst_QSocks5SocketEngine::exitLoopSlot()
{
    QTestEventLoop::instance().exitLoop();
}


void tst_QSocks5SocketEngine::downloadBigFileSlot()
{
    QByteArray tmp=tmpSocket->readAll();
    int correction=tmp.indexOf((char)0,0); //skip header
    if (correction==-1) correction=0;
    bytesAvailable += (tmp.size()-correction);
    if (bytesAvailable >= 10000000)
        QTestEventLoop::instance().exitLoop();
}

void tst_QSocks5SocketEngine::passwordAuth()
{
    QSocks5SocketEngine socketDevice;

    // Initialize device
    QVERIFY(socketDevice.initialize(QAbstractSocket::TcpSocket, QAbstractSocket::IPv4Protocol));
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);

    socketDevice.setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080, "qsockstest", "password"));

    // Connect to imap.trolltech.com's IP
    QVERIFY(!socketDevice.connectToHost(QtNetworkSettings::serverIP(), 143));
    QVERIFY(socketDevice.state() == QAbstractSocket::ConnectingState);
    QVERIFY(socketDevice.waitForWrite());
    if (!socketDevice.connectToHost(QtNetworkSettings::serverIP(), 143)) {
        qDebug("%d, %s", socketDevice.error(), socketDevice.errorString().toLatin1().constData());
    }
    QVERIFY(socketDevice.state() == QAbstractSocket::ConnectedState);
    QVERIFY(socketDevice.peerAddress() == QtNetworkSettings::serverIP());

    // Wait for the greeting
    QVERIFY(socketDevice.waitForRead());

    // Read the greeting
    qint64 available = socketDevice.bytesAvailable();
    QVERIFY(available > 0);
    QByteArray array;
    array.resize(available);
    QVERIFY(socketDevice.read(array.data(), array.size()) == available);

    // Check that the greeting is what we expect it to be
    QVERIFY2(QtNetworkSettings::compareReplyIMAP(array), array.constData());

    // Write a logout message
    QByteArray array2 = "XXXX LOGOUT\r\n";
    QVERIFY(socketDevice.write(array2.data(),
                              array2.size()) == array2.size());

    // Wait for the response
    QVERIFY(socketDevice.waitForRead());

    available = socketDevice.bytesAvailable();
    QVERIFY(available > 0);
    array.resize(available);
    QVERIFY(socketDevice.read(array.data(), array.size()) == available);

    // Check that the greeting is what we expect it to be
    QCOMPARE(array.constData(), "* BYE LOGOUT received\r\nXXXX OK Completed\r\n");

    // Wait for the response
    QVERIFY(socketDevice.waitForRead());
    char c;
    QVERIFY(socketDevice.read(&c, sizeof(c)) == -1);
    QVERIFY(socketDevice.error() == QAbstractSocket::RemoteHostClosedError);
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);
}

//----------------------------------------------------------------------------------

void tst_QSocks5SocketEngine::proxyAuthenticationRequired(const QNetworkProxy &,
                                                          QAuthenticator *auth)
{
    auth->setUser("qsockstest");
    auth->setPassword("password");
}

void tst_QSocks5SocketEngine::passwordAuth2()
{
    QSocks5SocketEngine socketDevice;

    // Initialize device
    QVERIFY(socketDevice.initialize(QAbstractSocket::TcpSocket, QAbstractSocket::IPv4Protocol));
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);

    socketDevice.setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1081));
    socketDevice.setReceiver(this);

    QVERIFY(!socketDevice.connectToHost(QtNetworkSettings::serverIP(), 143));
    QVERIFY(socketDevice.state() == QAbstractSocket::ConnectingState);
    while (socketDevice.state() == QAbstractSocket::ConnectingState) {
        QVERIFY(socketDevice.waitForWrite());
        socketDevice.connectToHost(QtNetworkSettings::serverIP(), 143);
    }
    if (socketDevice.state() != QAbstractSocket::ConnectedState)
        qDebug("%d, %s", socketDevice.error(), socketDevice.errorString().toLatin1().constData());
    QVERIFY(socketDevice.state() == QAbstractSocket::ConnectedState);
    QVERIFY(socketDevice.peerAddress() == QtNetworkSettings::serverIP());

    // Wait for the greeting
    QVERIFY(socketDevice.waitForRead());

    // Read the greeting
    qint64 available = socketDevice.bytesAvailable();
    QVERIFY(available > 0);
    QByteArray array;
    array.resize(available);
    QVERIFY(socketDevice.read(array.data(), array.size()) == available);

    // Check that the greeting is what we expect it to be
    QVERIFY2(QtNetworkSettings::compareReplyIMAP(array), array.constData());

    // Write a logout message
    QByteArray array2 = "XXXX LOGOUT\r\n";
    QVERIFY(socketDevice.write(array2.data(),
                              array2.size()) == array2.size());

    // Wait for the response
    QVERIFY(socketDevice.waitForRead());

    available = socketDevice.bytesAvailable();
    QVERIFY(available > 0);
    array.resize(available);
    QVERIFY(socketDevice.read(array.data(), array.size()) == available);

    // Check that the greeting is what we expect it to be
    QCOMPARE(array.constData(), "* BYE LOGOUT received\r\nXXXX OK Completed\r\n");

    // Wait for the response
    QVERIFY(socketDevice.waitForRead());
    char c;
    QVERIFY(socketDevice.read(&c, sizeof(c)) == -1);
    QVERIFY(socketDevice.error() == QAbstractSocket::RemoteHostClosedError);
    QVERIFY(socketDevice.state() == QAbstractSocket::UnconnectedState);
}

//----------------------------------------------------------------------------------

QTEST_MAIN(tst_QSocks5SocketEngine)
#include "tst_qsocks5socketengine.moc"
