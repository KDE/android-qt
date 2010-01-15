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
// This file contains benchmarks for QNetworkReply functions.

#include <QDebug>
#include <qtest.h>
#include <QtTest/QtTest>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include "../../auto/network-settings.h"

#define BANDWIDTH_LIMIT_BYTES (1024*100)
#define TIME_ESTIMATION_SECONDS (97)

class tst_qnetworkreply : public QObject
{
    Q_OBJECT
    private slots:
    void limiting_data();
    void limiting();

};

QNetworkReply *reply;

class HttpReceiver : public QObject
{
    Q_OBJECT
    public slots:
    void finishedSlot() {
        quint64 bytesPerSec = (reply->header(QNetworkRequest::ContentLengthHeader).toLongLong()) / (stopwatch.elapsed() / 1000.0);
        qDebug() << "Finished HTTP(S) request with" << bytesPerSec << "bytes/sec";
        QVERIFY(bytesPerSec < BANDWIDTH_LIMIT_BYTES*1.05);
        QVERIFY(bytesPerSec > BANDWIDTH_LIMIT_BYTES*0.95);
        timer->stop();
        QTestEventLoop::instance().exitLoop();
    }
    void readyReadSlot() {
    }
    void timeoutSlot() {
        reply->read(BANDWIDTH_LIMIT_BYTES).size();
    }
    void startTimer() {
        stopwatch.start();
        timer = new QTimer(this);
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
        timer->start(1000);
    }
protected:
    QTimer *timer;
    QTime stopwatch;
};

void tst_qnetworkreply::limiting_data()
{
    QTest::addColumn<QUrl>("url");

    QTest::newRow("HTTP") << QUrl("http://" + QtNetworkSettings::serverName() + "/mediumfile");
#ifndef QT_NO_OPENSSL
    QTest::newRow("HTTP+SSL") << QUrl("https://" + QtNetworkSettings::serverName() + "/mediumfile");
#endif

}

void tst_qnetworkreply::limiting()
{
    HttpReceiver receiver;
    QNetworkAccessManager manager;

    QFETCH(QUrl, url);
    QNetworkRequest req (url);

    qDebug() << "Starting. This will take a while (around" << TIME_ESTIMATION_SECONDS << "sec).";
    qDebug() << "Please check the actual bandwidth usage with a network monitor, e.g. the KDE";
    qDebug() << "network plasma widget. It should be around" << BANDWIDTH_LIMIT_BYTES << "bytes/sec.";
    reply = manager.get(req);
    reply->ignoreSslErrors();
    reply->setReadBufferSize(BANDWIDTH_LIMIT_BYTES);
    QObject::connect(reply, SIGNAL(readyRead()), &receiver, SLOT(readyReadSlot()));
    QObject::connect(reply, SIGNAL(finished()), &receiver, SLOT(finishedSlot()));
    receiver.startTimer();

    // event loop
    QTestEventLoop::instance().enterLoop(TIME_ESTIMATION_SECONDS + 20);
    QVERIFY(!QTestEventLoop::instance().timeout());
}


QTEST_MAIN(tst_qnetworkreply)

#include "main.moc"
