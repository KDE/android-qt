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


#include <QtTest/QtTest>
#include <QtNetwork/QtNetwork>
#include "../../shared/util.h"
#include "../network-settings.h"

#define TESTFILE QString("http://%1/qtest/cgi-bin/").arg(QtNetworkSettings::serverName())

class tst_QAbstractNetworkCache : public QObject
{
    Q_OBJECT

public:
    tst_QAbstractNetworkCache();
    virtual ~tst_QAbstractNetworkCache();

private slots:
    void expires_data();
    void expires();

    void lastModified_data();
    void lastModified();

    void etag_data();
    void etag();

    void cacheControl_data();
    void cacheControl();

    void deleteCache();

private:
    void check();
};

class NetworkDiskCache : public QNetworkDiskCache
{
    Q_OBJECT
public:
    NetworkDiskCache(QObject *parent = 0)
        : QNetworkDiskCache(parent)
        , gotData(false)
    {
        QString location = QDir::tempPath() + QLatin1String("/tst_qnetworkdiskcache/");
        setCacheDirectory(location);
        clear();
    }

    QIODevice *data(const QUrl &url)
    {
        gotData = true;
        return QNetworkDiskCache::data(url);
    }

    bool gotData;
};


tst_QAbstractNetworkCache::tst_QAbstractNetworkCache()
{
    Q_SET_DEFAULT_IAP

    QCoreApplication::setOrganizationName(QLatin1String("Trolltech"));
    QCoreApplication::setApplicationName(QLatin1String("autotest_qabstractnetworkcache"));
    QCoreApplication::setApplicationVersion(QLatin1String("1.0"));
}

tst_QAbstractNetworkCache::~tst_QAbstractNetworkCache()
{
}

static bool AlwaysTrue = true;
static bool AlwaysFalse = false;

Q_DECLARE_METATYPE(QNetworkRequest::CacheLoadControl)

void tst_QAbstractNetworkCache::expires_data()
{
    QTest::addColumn<QNetworkRequest::CacheLoadControl>("cacheLoadControl");
    QTest::addColumn<QString>("url");
    QTest::addColumn<bool>("fetchFromCache");

    QTest::newRow("304-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_expires304.cgi" << AlwaysFalse;
    QTest::newRow("304-1") << QNetworkRequest::PreferNetwork << "httpcachetest_expires304.cgi" << true;
    QTest::newRow("304-2") << QNetworkRequest::AlwaysCache << "httpcachetest_expires304.cgi" << AlwaysTrue;
    QTest::newRow("304-3") << QNetworkRequest::PreferCache << "httpcachetest_expires304.cgi" << true;

    QTest::newRow("500-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_expires500.cgi" << AlwaysFalse;
    QTest::newRow("500-1") << QNetworkRequest::PreferNetwork << "httpcachetest_expires500.cgi" << true;
    QTest::newRow("500-2") << QNetworkRequest::AlwaysCache << "httpcachetest_expires500.cgi" << AlwaysTrue;
    QTest::newRow("500-3") << QNetworkRequest::PreferCache << "httpcachetest_expires500.cgi" << true;

    QTest::newRow("200-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_expires200.cgi" << AlwaysFalse;
    QTest::newRow("200-1") << QNetworkRequest::PreferNetwork << "httpcachetest_expires200.cgi" << false;
    QTest::newRow("200-2") << QNetworkRequest::AlwaysCache << "httpcachetest_expires200.cgi" << AlwaysTrue;
    QTest::newRow("200-3") << QNetworkRequest::PreferCache << "httpcachetest_expires200.cgi" << false;
}

void tst_QAbstractNetworkCache::expires()
{
    check();
}

void tst_QAbstractNetworkCache::lastModified_data()
{
    QTest::addColumn<QNetworkRequest::CacheLoadControl>("cacheLoadControl");
    QTest::addColumn<QString>("url");
    QTest::addColumn<bool>("fetchFromCache");

    QTest::newRow("304-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_lastModified304.cgi" << AlwaysFalse;
    QTest::newRow("304-1") << QNetworkRequest::PreferNetwork << "httpcachetest_lastModified304.cgi" << true;
    QTest::newRow("304-2") << QNetworkRequest::AlwaysCache << "httpcachetest_lastModified304.cgi" << AlwaysTrue;
    QTest::newRow("304-3") << QNetworkRequest::PreferCache << "httpcachetest_lastModified304.cgi" << true;

    QTest::newRow("200-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_lastModified200.cgi" << AlwaysFalse;
    QTest::newRow("200-1") << QNetworkRequest::PreferNetwork << "httpcachetest_lastModified200.cgi" << false;
    QTest::newRow("200-2") << QNetworkRequest::AlwaysCache << "httpcachetest_lastModified200.cgi" << AlwaysTrue;
    QTest::newRow("200-3") << QNetworkRequest::PreferCache << "httpcachetest_lastModified200.cgi" << false;
}

void tst_QAbstractNetworkCache::lastModified()
{
    check();
}

void tst_QAbstractNetworkCache::etag_data()
{
    QTest::addColumn<QNetworkRequest::CacheLoadControl>("cacheLoadControl");
    QTest::addColumn<QString>("url");
    QTest::addColumn<bool>("fetchFromCache");

    QTest::newRow("304-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_etag304.cgi" << AlwaysFalse;
    QTest::newRow("304-1") << QNetworkRequest::PreferNetwork << "httpcachetest_etag304.cgi" << true;
    QTest::newRow("304-2") << QNetworkRequest::AlwaysCache << "httpcachetest_etag304.cgi" << AlwaysTrue;
    QTest::newRow("304-3") << QNetworkRequest::PreferCache << "httpcachetest_etag304.cgi" << true;

    QTest::newRow("200-0") << QNetworkRequest::AlwaysNetwork << "httpcachetest_etag200.cgi" << AlwaysFalse;
    QTest::newRow("200-1") << QNetworkRequest::PreferNetwork << "httpcachetest_etag200.cgi" << false;
    QTest::newRow("200-2") << QNetworkRequest::AlwaysCache << "httpcachetest_etag200.cgi" << AlwaysTrue;
    QTest::newRow("200-3") << QNetworkRequest::PreferCache << "httpcachetest_etag200.cgi" << false;
}

void tst_QAbstractNetworkCache::etag()
{
    check();
}

void tst_QAbstractNetworkCache::cacheControl_data()
{
    QTest::addColumn<QNetworkRequest::CacheLoadControl>("cacheLoadControl");
    QTest::addColumn<QString>("url");
    QTest::addColumn<bool>("fetchFromCache");
    QTest::newRow("200-0") << QNetworkRequest::PreferNetwork << "httpcachetest_cachecontrol.cgi?max-age=-1" << true;
    QTest::newRow("200-1") << QNetworkRequest::PreferNetwork << "httpcachetest_cachecontrol-expire.cgi" << false;

    QTest::newRow("200-2") << QNetworkRequest::AlwaysNetwork << "httpcachetest_cachecontrol.cgi?no-cache" << AlwaysFalse;
    QTest::newRow("200-3") << QNetworkRequest::PreferNetwork << "httpcachetest_cachecontrol.cgi?no-cache" << false;
    QTest::newRow("200-4") << QNetworkRequest::AlwaysCache << "httpcachetest_cachecontrol.cgi?no-cache" << false;//AlwaysTrue;
    QTest::newRow("200-5") << QNetworkRequest::PreferCache << "httpcachetest_cachecontrol.cgi?no-cache" << false;

    QTest::newRow("304-0") << QNetworkRequest::PreferNetwork << "httpcachetest_cachecontrol.cgi?max-age=1000" << true;

    QTest::newRow("304-1") << QNetworkRequest::AlwaysNetwork << "httpcachetest_cachecontrol.cgi?max-age=1000, must-revalidate" << AlwaysFalse;
    QTest::newRow("304-2") << QNetworkRequest::PreferNetwork << "httpcachetest_cachecontrol.cgi?max-age=1000, must-revalidate" << true;
    QTest::newRow("304-3") << QNetworkRequest::AlwaysCache << "httpcachetest_cachecontrol.cgi?max-age=1000, must-revalidate" << AlwaysTrue;
    QTest::newRow("304-4") << QNetworkRequest::PreferCache << "httpcachetest_cachecontrol.cgi?max-age=1000, must-revalidate" << true;

    // see QTBUG-7060
    //QTest::newRow("nokia-boston") << QNetworkRequest::PreferNetwork << "http://waplabdc.nokia-boston.com/browser/users/venkat/cache/Cache_directives/private_1b.asp" << true;
    QTest::newRow("304-2b") << QNetworkRequest::PreferNetwork << "httpcachetest_cachecontrol200.cgi?private, max-age=1000" << true;
    QTest::newRow("304-4b") << QNetworkRequest::PreferCache << "httpcachetest_cachecontrol200.cgi?private, max-age=1000" << true;
}

void tst_QAbstractNetworkCache::cacheControl()
{
    check();
}

void tst_QAbstractNetworkCache::check()
{
    QFETCH(QNetworkRequest::CacheLoadControl, cacheLoadControl);
    QFETCH(QString, url);
    QFETCH(bool, fetchFromCache);

    QNetworkAccessManager manager;
    NetworkDiskCache *diskCache = new NetworkDiskCache(&manager);
    manager.setCache(diskCache);
    QCOMPARE(diskCache->gotData, false);

    QUrl realUrl = url.contains("://") ? url : TESTFILE + url;
    QNetworkRequest request(realUrl);

    // prime the cache
    QNetworkReply *reply = manager.get(request);
    QSignalSpy downloaded1(reply, SIGNAL(finished()));
    QTRY_COMPARE(downloaded1.count(), 1);
    QCOMPARE(diskCache->gotData, false);
    QByteArray goodData = reply->readAll();

    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, cacheLoadControl);

    // should be in the cache now
    QNetworkReply *reply2 = manager.get(request);
    QSignalSpy downloaded2(reply2, SIGNAL(finished()));
    QTRY_COMPARE(downloaded2.count(), 1);

    QByteArray secondData = reply2->readAll();
    if (!fetchFromCache && cacheLoadControl == QNetworkRequest::AlwaysCache) {
        QCOMPARE(reply2->error(), QNetworkReply::ContentNotFoundError);
        QCOMPARE(secondData, QByteArray());
    } else {
        if (reply2->error() != QNetworkReply::NoError)
            qDebug() << reply2->errorString();
        QCOMPARE(reply2->error(), QNetworkReply::NoError);
        QCOMPARE(QString(secondData), QString(goodData));
        QCOMPARE(secondData, goodData);
        QCOMPARE(reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    }

    if (fetchFromCache) {
        QList<QByteArray> rawHeaderList = reply->rawHeaderList();
        QList<QByteArray> rawHeaderList2 = reply2->rawHeaderList();
        qSort(rawHeaderList);
        qSort(rawHeaderList2);

        // headers can change
        for (int i = 0; i < rawHeaderList.count(); ++i) {
            //qDebug() << i << rawHeaderList.value(i) << reply->rawHeader(rawHeaderList.value(i));
            //qDebug() << i << rawHeaderList2.value(i) << reply2->rawHeader(rawHeaderList2.value(i));
            //QCOMPARE(QString(rawHeaderList.value(i)), QString(rawHeaderList2.value(i)));
            //QCOMPARE(QString(reply->rawHeader(rawHeaderList.value(i))), QString(reply2->rawHeader(rawHeaderList2.value(i))));
        }
        //QCOMPARE(rawHeaderList.count(), rawHeaderList2.count());

    }
    QCOMPARE(diskCache->gotData, fetchFromCache);
}

void tst_QAbstractNetworkCache::deleteCache()
{
    QNetworkAccessManager manager;
    NetworkDiskCache *diskCache = new NetworkDiskCache(&manager);
    manager.setCache(diskCache);

    QString url = "httpcachetest_cachecontrol.cgi?max-age=1000";
    QNetworkRequest request(QUrl(TESTFILE + url));
    QNetworkReply *reply = manager.get(request);
    QSignalSpy downloaded1(reply, SIGNAL(finished()));
    manager.setCache(0);
    QTRY_COMPARE(downloaded1.count(), 1);
}


QTEST_MAIN(tst_QAbstractNetworkCache)
#include "tst_qabstractnetworkcache.moc"

