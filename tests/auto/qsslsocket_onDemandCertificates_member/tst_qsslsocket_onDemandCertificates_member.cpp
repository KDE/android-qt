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


#include <QtNetwork>
#include <QtTest/QtTest>

#include <QNetworkProxy>
#include <QAuthenticator>

#include "private/qhostinfo_p.h"

#include "../network-settings.h"

#ifdef Q_OS_SYMBIAN
#define SRCDIR ""
#endif

#ifndef QT_NO_OPENSSL
class QSslSocketPtr: public QSharedPointer<QSslSocket>
{
public:
    inline QSslSocketPtr(QSslSocket *ptr = 0)
        : QSharedPointer<QSslSocket>(ptr)
    { }

    inline operator QSslSocket *() const { return data(); }
};
#endif

class tst_QSslSocket_onDemandCertificates_member : public QObject
{
    Q_OBJECT

    int proxyAuthCalled;

public:
    tst_QSslSocket_onDemandCertificates_member();
    virtual ~tst_QSslSocket_onDemandCertificates_member();

#ifndef QT_NO_OPENSSL
    QSslSocketPtr newSocket();
#endif

public slots:
    void initTestCase_data();
    void init();
    void cleanup();
    void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *auth);

#ifndef QT_NO_OPENSSL
private slots:
    void onDemandRootCertLoadingMemberMethods();

private:
    QSslSocket *socket;
#endif // QT_NO_OPENSSL
};

tst_QSslSocket_onDemandCertificates_member::tst_QSslSocket_onDemandCertificates_member()
{
    Q_SET_DEFAULT_IAP
}

tst_QSslSocket_onDemandCertificates_member::~tst_QSslSocket_onDemandCertificates_member()
{
}

enum ProxyTests {
    NoProxy = 0x00,
    Socks5Proxy = 0x01,
    HttpProxy = 0x02,
    TypeMask = 0x0f,

    NoAuth = 0x00,
    AuthBasic = 0x10,
    AuthNtlm = 0x20,
    AuthMask = 0xf0
};

void tst_QSslSocket_onDemandCertificates_member::initTestCase_data()
{
    QTest::addColumn<bool>("setProxy");
    QTest::addColumn<int>("proxyType");

    QTest::newRow("WithoutProxy") << false << 0;
    QTest::newRow("WithSocks5Proxy") << true << int(Socks5Proxy);
    QTest::newRow("WithSocks5ProxyAuth") << true << int(Socks5Proxy | AuthBasic);

    QTest::newRow("WithHttpProxy") << true << int(HttpProxy);
    QTest::newRow("WithHttpProxyBasicAuth") << true << int(HttpProxy | AuthBasic);
    // uncomment the line below when NTLM works
//    QTest::newRow("WithHttpProxyNtlmAuth") << true << int(HttpProxy | AuthNtlm);
}

void tst_QSslSocket_onDemandCertificates_member::init()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy) {
        QFETCH_GLOBAL(int, proxyType);
        QString testServer = QHostInfo::fromName(QtNetworkSettings::serverName()).addresses().first().toString();
        QNetworkProxy proxy;

        switch (proxyType) {
        case Socks5Proxy:
            proxy = QNetworkProxy(QNetworkProxy::Socks5Proxy, testServer, 1080);
            break;

        case Socks5Proxy | AuthBasic:
            proxy = QNetworkProxy(QNetworkProxy::Socks5Proxy, testServer, 1081);
            break;

        case HttpProxy | NoAuth:
            proxy = QNetworkProxy(QNetworkProxy::HttpProxy, testServer, 3128);
            break;

        case HttpProxy | AuthBasic:
            proxy = QNetworkProxy(QNetworkProxy::HttpProxy, testServer, 3129);
            break;

        case HttpProxy | AuthNtlm:
            proxy = QNetworkProxy(QNetworkProxy::HttpProxy, testServer, 3130);
            break;
        }
        QNetworkProxy::setApplicationProxy(proxy);
    }

    qt_qhostinfo_clear_cache();
}

void tst_QSslSocket_onDemandCertificates_member::cleanup()
{
    QNetworkProxy::setApplicationProxy(QNetworkProxy::DefaultProxy);
}

#ifndef QT_NO_OPENSSL
QSslSocketPtr tst_QSslSocket_onDemandCertificates_member::newSocket()
{
    QSslSocket *socket = new QSslSocket;

    proxyAuthCalled = 0;
    connect(socket, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            Qt::DirectConnection);

    return QSslSocketPtr(socket);
}
#endif

void tst_QSslSocket_onDemandCertificates_member::proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *auth)
{
    ++proxyAuthCalled;
    auth->setUser("qsockstest");
    auth->setPassword("password");
}

#ifndef QT_NO_OPENSSL

void tst_QSslSocket_onDemandCertificates_member::onDemandRootCertLoadingMemberMethods()
{
    QString host("qt.nokia.com");

    // not using any root certs -> should not work
    QSslSocketPtr socket2 = newSocket();
    this->socket = socket2;
    socket2->setCaCertificates(QList<QSslCertificate>());
    socket2->connectToHostEncrypted(host, 443);
    QVERIFY(!socket2->waitForEncrypted());

    // default: using on demand loading -> should work
    QSslSocketPtr socket = newSocket();
    this->socket = socket;
    socket->connectToHostEncrypted(host, 443);
    QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));

    // not using any root certs again -> should not work
    QSslSocketPtr socket3 = newSocket();
    this->socket = socket3;
    socket3->setCaCertificates(QList<QSslCertificate>());
    socket3->connectToHostEncrypted(host, 443);
    QVERIFY(!socket3->waitForEncrypted());

    // setting empty SSL configuration explicitly -> should not work
    QSslSocketPtr socket4 = newSocket();
    this->socket = socket4;
    socket4->setSslConfiguration(QSslConfiguration());
    socket4->connectToHostEncrypted(host, 443);
    QVERIFY(!socket4->waitForEncrypted());
}

#endif // QT_NO_OPENSSL

QTEST_MAIN(tst_QSslSocket_onDemandCertificates_member)
#include "tst_qsslsocket_onDemandCertificates_member.moc"
