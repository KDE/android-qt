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


#include <QtTest/QTest>
#include <QtTest/QTestEventLoop>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qnetworkproxy.h>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QList>
#include <QThread>

Q_DECLARE_METATYPE(QNetworkConfiguration);
Q_DECLARE_METATYPE(QList<QNetworkProxy>);

class tst_QNetworkProxyFactory : public QObject {
    Q_OBJECT

public:
    tst_QNetworkProxyFactory();

    class QDebugProxyFactory : public QNetworkProxyFactory
    {
    public:
        virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query = QNetworkProxyQuery())
        {
            returnedList = QNetworkProxyFactory::systemProxyForQuery(query);
            requestCounter++;
            return returnedList;
        }
        QList<QNetworkProxy> returnedList;
        int requestCounter;
    };

private slots:
    void systemProxyForQueryCalledFromThread();
    void systemProxyForQuery() const;
#ifndef QT_NO_BEARERMANAGEMENT
    void fromConfigurations();
    void inNetworkAccessManager_data();
    void inNetworkAccessManager();
#endif

private:
    QString formatProxyName(const QNetworkProxy & proxy) const;
    QDebugProxyFactory *factory;
};

tst_QNetworkProxyFactory::tst_QNetworkProxyFactory()
{
    factory = new QDebugProxyFactory;
    QNetworkProxyFactory::setApplicationProxyFactory(factory);
}

QString tst_QNetworkProxyFactory::formatProxyName(const QNetworkProxy & proxy) const
{
    QString proxyName;
    if (!proxy.user().isNull())
        proxyName.append("%1:%2@").arg(proxy.user(), proxy.password());
    proxyName.append(QString("%1:%2").arg(proxy.hostName()).arg(proxy.port()));
    proxyName.append(QString(" (type=%1, capabilities=%2)").arg(proxy.type()).arg(proxy.capabilities()));

    return proxyName;
}

void tst_QNetworkProxyFactory::systemProxyForQuery() const
{
    QNetworkProxyQuery query(QUrl(QString("http://www.abc.com")), QNetworkProxyQuery::UrlRequest);
    QList<QNetworkProxy> systemProxyList = QNetworkProxyFactory::systemProxyForQuery(query);
    bool pass = true;
    QNetworkProxy proxy;

    QList<QNetworkProxy> nativeProxyList;
    nativeProxyList << QNetworkProxy(QNetworkProxy::HttpProxy, QString("test.proxy.com"), 8080) << QNetworkProxy::NoProxy;

    foreach (proxy, systemProxyList) {
        if (!nativeProxyList.contains(proxy)) {
            qWarning() << "System proxy not found in native proxy list: " <<
                  formatProxyName(proxy);
            pass = false;
        }
    }

    foreach (proxy, nativeProxyList) {
        if (!systemProxyList.contains(proxy)) {
            qWarning() << "Native proxy not found in system proxy list: " <<
                  formatProxyName(proxy);
            pass = false;
        }
    }

    if (!pass)
        QFAIL("One or more system proxy lookup failures occurred.");
}

#ifndef QT_NO_BEARERMANAGEMENT

//Purpose of this test is just to check systemProxyForQuery doesn't hang or crash
//with any given configuration including no configuration.
//We can't test it returns the right proxies without implementing the native proxy code
//again here, which would be testing our implementation against itself.
//Therefore it's just testing that something valid is returned (at least a NoProxy entry)
void tst_QNetworkProxyFactory::fromConfigurations()
{
    QNetworkConfigurationManager manager;
    //update is done to get the active / discovered states
    manager.updateConfigurations();
    connect(&manager, SIGNAL(updateCompleted()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QList<QNetworkProxy> proxies;
    QUrl url(QLatin1String("http://qt.nokia.com"));
    //get from known configurations
    foreach (QNetworkConfiguration config, manager.allConfigurations()) {
        QNetworkProxyQuery query(config, url, QNetworkProxyQuery::UrlRequest);
        proxies = QNetworkProxyFactory::systemProxyForQuery(query);
        QVERIFY(!proxies.isEmpty());
        foreach (QNetworkProxy proxy, proxies) {
            qDebug() << config.name() << " - " << config.identifier() << " - " << formatProxyName(proxy);
        }
    }

    //get from default configuration
    QNetworkProxyQuery defaultquery(url, QNetworkProxyQuery::UrlRequest);
    proxies = QNetworkProxyFactory::systemProxyForQuery(defaultquery);
    QVERIFY(!proxies.isEmpty());
    foreach (QNetworkProxy proxy, proxies) {
        qDebug() << "default - " << formatProxyName(proxy);
    }

    //get from active configuration
    QNetworkSession session(manager.defaultConfiguration());
    session.open();
    QVERIFY(session.waitForOpened(30000));
    proxies = QNetworkProxyFactory::systemProxyForQuery(defaultquery);
    QVERIFY(!proxies.isEmpty());
    foreach (QNetworkProxy proxy, proxies) {
        qDebug() << "active - " << formatProxyName(proxy);
    }

    //get from known configurations while there is one active
    foreach (QNetworkConfiguration config, manager.allConfigurations()) {
        QNetworkProxyQuery query(config, url, QNetworkProxyQuery::UrlRequest);
        proxies = QNetworkProxyFactory::systemProxyForQuery(query);
        QVERIFY(!proxies.isEmpty());
        foreach (QNetworkProxy proxy, proxies) {
            qDebug() << config.name() << " - " << config.identifier() << " - " << formatProxyName(proxy);
        }
    }
}

void tst_QNetworkProxyFactory::inNetworkAccessManager_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");
    QTest::addColumn<QList<QNetworkProxy> >("proxies");
    QNetworkConfigurationManager manager;
    //get from known configurations
    foreach (QNetworkConfiguration config, manager.allConfigurations()) {
        QNetworkProxyQuery query(config, QUrl(QString("http://qt.nokia.com")), QNetworkProxyQuery::UrlRequest);
        QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(query);
        QTest::newRow(config.name().toUtf8()) << config << proxies;
    }
}

//Purpose of this test is to check that QNetworkAccessManager uses the proxy from the configuration it
//has been given. Needs two or more working configurations to be a good test.
void tst_QNetworkProxyFactory::inNetworkAccessManager()
{
    QFETCH(QNetworkConfiguration, config);
    QFETCH(QList<QNetworkProxy>, proxies);

    int count = factory->requestCounter;

    QNetworkAccessManager manager;
    manager.setConfiguration(config);

    //using an internet server, because cellular APs won't have a route to the test server.
    QNetworkRequest req(QUrl(QString("http://qt.nokia.com")));
    QNetworkReply *reply = manager.get(req);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(30);
    delete reply;

    if (count == factory->requestCounter) {
        //RND phones are preconfigured with several test access points which won't work without a matching SIM
        //If the network fails to start, QNAM won't ask the factory for proxies so we can't test.
        QSKIP("network configuration didn't start", SkipSingle);
    }

    qDebug() << "testing network configuration for" << config.name();
    foreach (QNetworkProxy proxy, factory->returnedList) {
        qDebug() << formatProxyName(proxy);
    }
    qDebug() << " <vs> ";
    foreach (QNetworkProxy proxy, proxies) {
        qDebug() << formatProxyName(proxy);
    }
    QCOMPARE(factory->returnedList, proxies);
}

#endif //QT_NO_BEARERMANAGEMENT

class QSPFQThread : public QThread
{
protected:
    virtual void run()
    {
        proxies = QNetworkProxyFactory::systemProxyForQuery(query);
    }
public:
    QNetworkProxyQuery query;
    QList<QNetworkProxy> proxies;
};

//regression test for QTBUG-18799
void tst_QNetworkProxyFactory::systemProxyForQueryCalledFromThread()
{
    QUrl url(QLatin1String("http://qt.nokia.com"));
    QNetworkProxyQuery query(url);
    QSPFQThread thread;
    thread.query = query;
    connect(&thread, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    thread.start();
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(thread.isFinished());
    QCOMPARE(thread.proxies, QNetworkProxyFactory::systemProxyForQuery(query));
}

QTEST_MAIN(tst_QNetworkProxyFactory)
#include "tst_qnetworkproxyfactory.moc"
