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
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/qtcpserver.h>
#include "../../auto/network-settings.h"


class TimedSender: public QThread
{
    Q_OBJECT
    qint64 totalBytes;
    QSemaphore ready;
    QByteArray dataToSend;
    QTcpSocket *client;
    int timeout;
    int port;
public:
    int transferRate;
    TimedSender(int ms)
        : totalBytes(0), timeout(ms), port(-1), transferRate(-1)
    {
        dataToSend = QByteArray(16*1024, '@');
        start();
        ready.acquire();
    }

    inline int serverPort() const { return port; }

private slots:
    void writeMore()
    {
        while (client->bytesToWrite() < 128 * 1024) {
            writePacket(dataToSend);
        }
    }

protected:
    void run()
    {
        QTcpServer server;
        server.listen();
        port = server.serverPort();
        ready.release();

        server.waitForNewConnection(-1);
        client = server.nextPendingConnection();

        writeMore();
        connect(client, SIGNAL(bytesWritten(qint64)), SLOT(writeMore()), Qt::DirectConnection);

        QEventLoop eventLoop;
        QTimer::singleShot(timeout, &eventLoop, SLOT(quit()));

        QTime timer;
        timer.start();
        eventLoop.exec();
        disconnect(client, SIGNAL(bytesWritten(qint64)), this, 0);

        // wait for the connection to shut down
        client->disconnectFromHost();
        if (!client->waitForDisconnected(10000))
            return;

        transferRate = totalBytes * 1000 / timer.elapsed();
        qDebug() << "TimedSender::run" << "receive rate:" << (transferRate / 1024) << "kB/s in"
                 << timer.elapsed() << "ms";
    }

    void writePacket(const QByteArray &array)
    {
        client->write(array);
        totalBytes += array.size();
    }
};


class QNetworkReplyPtr: public QSharedPointer<QNetworkReply>
{
public:
    inline QNetworkReplyPtr(QNetworkReply *ptr = 0)
        : QSharedPointer<QNetworkReply>(ptr)
    { }

    inline operator QNetworkReply *() const { return data(); }
};


class DataReader: public QObject
{
    Q_OBJECT
public:
    qint64 totalBytes;
    QByteArray data;
    QIODevice *device;
    bool accumulate;
    DataReader(QIODevice *dev, bool acc = true) : totalBytes(0), device(dev), accumulate(acc)
    {
        connect(device, SIGNAL(readyRead()), SLOT(doRead()));
    }

public slots:
    void doRead()
    {
        QByteArray buffer;
        buffer.resize(device->bytesAvailable());
        qint64 bytesRead = device->read(buffer.data(), device->bytesAvailable());
        if (bytesRead == -1) {
            QTestEventLoop::instance().exitLoop();
            return;
        }
        buffer.truncate(bytesRead);
        totalBytes += bytesRead;

        if (accumulate)
            data += buffer;
    }
};

class ThreadedDataReader: public QThread
{
    Q_OBJECT
    // used to make the constructor only return after the tcp server started listening
    QSemaphore ready;
    QTcpSocket *client;
    int timeout;
    int port;
public:
    qint64 transferRate;
    ThreadedDataReader()
        : port(-1), transferRate(-1)
    {
        start();
        ready.acquire();
    }

    inline int serverPort() const { return port; }

protected:
    void run()
    {
        QTcpServer server;
        server.listen();
        port = server.serverPort();
        ready.release();

        server.waitForNewConnection(-1);
        client = server.nextPendingConnection();

        QEventLoop eventLoop;
        DataReader reader(client, false);
        QObject::connect(client, SIGNAL(disconnected()), &eventLoop, SLOT(quit()));

        QTime timer;
        timer.start();
        eventLoop.exec();
        qint64 elapsed = timer.elapsed();

        transferRate = reader.totalBytes * 1000 / elapsed;
        qDebug() << "ThreadedDataReader::run" << "send rate:" << (transferRate / 1024) << "kB/s in" << elapsed << "msec";
    }
};

class DataGenerator: public QIODevice
{
    Q_OBJECT
    enum { Idle, Started, Stopped } state;
public:
    DataGenerator() : state(Idle)
    { open(ReadOnly); }

    virtual bool isSequential() const { return true; }
    virtual qint64 bytesAvailable() const { return state == Started ? 1024*1024 : 0; }

public slots:
    void start() { state = Started; emit readyRead(); }
    void stop() { state = Stopped; emit readyRead(); }

protected:
    virtual qint64 readData(char *data, qint64 maxlen)
    {
        if (state == Stopped)
            return -1;          // EOF

        // return as many bytes as are wanted
        memset(data, '@', maxlen);
        return maxlen;
    }
    virtual qint64 writeData(const char *, qint64)
    { return -1; }
};

class ThreadedDataReaderHttpServer: public QThread
{
    Q_OBJECT
    // used to make the constructor only return after the tcp server started listening
    QSemaphore ready;
    QTcpSocket *client;
    int timeout;
    int port;
public:
    qint64 transferRate;
    ThreadedDataReaderHttpServer()
        : port(-1), transferRate(-1)
    {
        start();
        ready.acquire();
    }

    inline int serverPort() const { return port; }

protected:
    void run()
    {
        QTcpServer server;
        server.listen();
        port = server.serverPort();
        ready.release();

        server.waitForNewConnection(-1);
        client = server.nextPendingConnection();
        client->write("HTTP/1.0 200 OK\r\n");
        client->write("Content-length: 0\r\n");
        client->write("\r\n");
        client->flush();

        QCoreApplication::processEvents();

        QEventLoop eventLoop;
        DataReader reader(client, false);
        QObject::connect(client, SIGNAL(disconnected()), &eventLoop, SLOT(quit()));

        QTime timer;
        timer.start();
        eventLoop.exec();
        qint64 elapsed = timer.elapsed();

        transferRate = reader.totalBytes * 1000 / elapsed;
        qDebug() << "ThreadedDataReaderHttpServer::run" << "send rate:" << (transferRate / 1024) << "kB/s in" << elapsed << "msec";
    }
};


class FixedSizeDataGenerator : public QIODevice
{
    Q_OBJECT
    enum { Idle, Started, Stopped } state;
public:
    FixedSizeDataGenerator(qint64 size) : state(Idle)
    { open(ReadOnly | Unbuffered);
      toBeGeneratedTotalCount = toBeGeneratedCount = size;
    }

    virtual qint64 bytesAvailable() const
    {
        return state == Started ? toBeGeneratedCount + QIODevice::bytesAvailable() : 0;
    }

    virtual bool isSequential() const{
        return false;
    }

    virtual bool reset() const{
        return false;
    }

    qint64 size() const {
        return toBeGeneratedTotalCount;
    }

public slots:
    void start() { state = Started; emit readyRead(); }

protected:
    virtual qint64 readData(char *data, qint64 maxlen)
    {
        memset(data, '@', maxlen);

        if (toBeGeneratedCount <= 0) {
            return -1;
        }

        qint64 n = qMin(maxlen, toBeGeneratedCount);
        toBeGeneratedCount -= n;

        if (toBeGeneratedCount <= 0) {
            // make sure this is a queued connection!
            emit readChannelFinished();
        }

        return n;
    }
    virtual qint64 writeData(const char *, qint64)
    { return -1; }

    qint64 toBeGeneratedCount;
    qint64 toBeGeneratedTotalCount;
};

class HttpDownloadPerformanceServer : QObject {
    Q_OBJECT;
    qint64 dataSize;
    qint64 dataSent;
    QTcpServer server;
    QTcpSocket *client;
    bool serverSendsContentLength;
    bool chunkedEncoding;

public:
    HttpDownloadPerformanceServer (qint64 ds, bool sscl, bool ce) : dataSize(ds), dataSent(0),
    client(0), serverSendsContentLength(sscl), chunkedEncoding(ce) {
        server.listen();
        connect(&server, SIGNAL(newConnection()), this, SLOT(newConnectionSlot()));
    }

    int serverPort() {
        return server.serverPort();
    }

public slots:

    void newConnectionSlot() {
        client = server.nextPendingConnection();
        client->setParent(this);
        connect(client, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
        connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWrittenSlot(qint64)));
    }

    void readyReadSlot() {
        client->readAll();
        client->write("HTTP/1.0 200 OK\n");
        if (serverSendsContentLength)
            client->write(QString("Content-Length: " + QString::number(dataSize) + "\n").toAscii());
        if (chunkedEncoding)
            client->write(QString("Transfer-Encoding: chunked\n").toAscii());
        client->write("Connection: close\n\n");
    }

    void bytesWrittenSlot(qint64 amount) {
        Q_UNUSED(amount);
        if (dataSent == dataSize && client) {
            // close eventually

            // chunked encoding: we have to send a last "empty" chunk
            if (chunkedEncoding)
                client->write(QString("0\r\n\r\n").toAscii());

            client->disconnectFromHost();
            server.close();
            client = 0;
            return;
        }

        // send data
        if (client && client->bytesToWrite() < 100*1024 && dataSent < dataSize) {
            qint64 amount = qMin(qint64(16*1024), dataSize - dataSent);
            QByteArray data(amount, '@');

            if (chunkedEncoding) {
                client->write(QString(QString("%1").arg(amount,0,16).toUpper() + "\r\n").toAscii());
                client->write(data.constData(), amount);
                client->write(QString("\r\n").toAscii());
            } else {
                client->write(data.constData(), amount);
            }

            dataSent += amount;
        }
    }
};

class HttpDownloadPerformanceClient : QObject {
    Q_OBJECT;
    QIODevice *device;
    public:
    HttpDownloadPerformanceClient (QIODevice *dev) : device(dev){
        connect(dev, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
    }

    public slots:
    void readyReadSlot() {
        device->readAll();
    }

};




class tst_qnetworkreply : public QObject
{
    Q_OBJECT

    QNetworkAccessManager manager;
private slots:
    void httpLatency();

#ifndef QT_NO_OPENSSL
    void echoPerformance_data();
    void echoPerformance();
#endif

    void downloadPerformance();
    void uploadPerformance();
    void performanceControlRate();
    void httpUploadPerformance();
    void httpDownloadPerformance_data();
    void httpDownloadPerformance();

};

void tst_qnetworkreply::httpLatency()
{
    QNetworkAccessManager manager;
    QBENCHMARK{
        QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/"));
        QNetworkReply* reply = manager.get(request);
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
        QTestEventLoop::instance().enterLoop(5);
        QVERIFY(!QTestEventLoop::instance().timeout());
        delete reply;
    }
}

#ifndef QT_NO_OPENSSL
void tst_qnetworkreply::echoPerformance_data()
{
     QTest::addColumn<bool>("ssl");
     QTest::newRow("no_ssl") << false;
     QTest::newRow("ssl") << true;
}

void tst_qnetworkreply::echoPerformance()
{
    QFETCH(bool, ssl);
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl((ssl ? "https://" : "http://") + QtNetworkSettings::serverName() + "/qtest/cgi-bin/echo.cgi"));

    QByteArray data;
    data.resize(1024*1024*10); // 10 MB
    // init with garbage. needed so ssl cannot compress it in an efficient way.
    for (int i = 0; i < data.size() / sizeof(int); i++) {
        int r = qrand();
        data.data()[i*sizeof(int)] = r;
    }

    QBENCHMARK{
        QNetworkReply* reply = manager.post(request, data);
        connect(reply, SIGNAL(sslErrors( const QList<QSslError> &)), reply, SLOT(ignoreSslErrors()));
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
        QTestEventLoop::instance().enterLoop(5);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(reply->error() == QNetworkReply::NoError);
        delete reply;
    }
}
#endif

void tst_qnetworkreply::downloadPerformance()
{
    // unlike the above function, this one tries to send as fast as possible
    // and measures how fast it was.
    TimedSender sender(5000);
    QNetworkRequest request("debugpipe://127.0.0.1:" + QString::number(sender.serverPort()) + "/?bare=1");
    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply, false);

    QTime loopTime;
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    loopTime.start();
    QTestEventLoop::instance().enterLoop(40);
    int elapsedTime = loopTime.elapsed();
    sender.wait();

    qint64 receivedBytes = reader.totalBytes;
    qDebug() << "tst_QNetworkReply::downloadPerformance" << "receive rate:" << (receivedBytes * 1000 / elapsedTime / 1024) << "kB/s and"
             << elapsedTime << "ms";
}

void tst_qnetworkreply::uploadPerformance()
{
      ThreadedDataReader reader;
      DataGenerator generator;


      QNetworkRequest request("debugpipe://127.0.0.1:" + QString::number(reader.serverPort()) + "/?bare=1");
      QNetworkReplyPtr reply = manager.put(request, &generator);
      generator.start();
      connect(&reader, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
      QTimer::singleShot(5000, &generator, SLOT(stop()));

      QTestEventLoop::instance().enterLoop(30);
      QCOMPARE(reply->error(), QNetworkReply::NoError);
      QVERIFY(!QTestEventLoop::instance().timeout());
}

void tst_qnetworkreply::httpUploadPerformance()
{
#ifdef Q_OS_SYMBIAN
      // SHow some mercy for non-desktop platform/s
      enum {UploadSize = 4*1024*1024}; // 4 MB
#else
      enum {UploadSize = 128*1024*1024}; // 128 MB
#endif
      ThreadedDataReaderHttpServer reader;
      FixedSizeDataGenerator generator(UploadSize);

      QNetworkRequest request(QUrl("http://127.0.0.1:" + QString::number(reader.serverPort()) + "/?bare=1"));
      request.setHeader(QNetworkRequest::ContentLengthHeader,UploadSize);

      QNetworkReplyPtr reply = manager.put(request, &generator);

      connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));

      QTime time;
      generator.start();
      time.start();
      QTestEventLoop::instance().enterLoop(40);
      qint64 elapsed = time.elapsed();
      reader.exit();
      reader.wait();
      QVERIFY(reply->isFinished());
      QCOMPARE(reply->error(), QNetworkReply::NoError);
      QVERIFY(!QTestEventLoop::instance().timeout());

      qDebug() << "tst_QNetworkReply::httpUploadPerformance" << elapsed << "msec, "
              << ((UploadSize/1024.0)/(elapsed/1000.0)) << " kB/sec";
}


void tst_qnetworkreply::performanceControlRate()
{
    // this is a control comparison for the other two above
    // it does the same thing, but instead bypasses the QNetworkAccess system
    qDebug() << "The following are the maximum transfer rates that we can get in this system"
        " (bypassing QNetworkAccess)";

    TimedSender sender(5000);
    QTcpSocket sink;
    sink.connectToHost("127.0.0.1", sender.serverPort());
    DataReader reader(&sink, false);

    QTime loopTime;
    connect(&sink, SIGNAL(disconnected()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    loopTime.start();
    QTestEventLoop::instance().enterLoop(40);
    int elapsedTime = loopTime.elapsed();
    sender.wait();

    qint64 receivedBytes = reader.totalBytes;
    qDebug() << "tst_QNetworkReply::performanceControlRate" << "receive rate:" << (receivedBytes * 1000 / elapsedTime / 1024) << "kB/s and"
             << elapsedTime << "ms";
}

void tst_qnetworkreply::httpDownloadPerformance_data()
{
    QTest::addColumn<bool>("serverSendsContentLength");
    QTest::addColumn<bool>("chunkedEncoding");

    QTest::newRow("Server sends no Content-Length") << false << false;
    QTest::newRow("Server sends Content-Length")     << true << false;
    QTest::newRow("Server uses chunked encoding")     << false << true;

}

void tst_qnetworkreply::httpDownloadPerformance()
{
    QFETCH(bool, serverSendsContentLength);
    QFETCH(bool, chunkedEncoding);
#ifdef Q_OS_SYMBIAN
    // Show some mercy to non-desktop platform/s
    enum {UploadSize = 4*1024*1024}; // 4 MB
#else
    enum {UploadSize = 128*1024*1024}; // 128 MB
#endif
    HttpDownloadPerformanceServer server(UploadSize, serverSendsContentLength, chunkedEncoding);

    QNetworkRequest request(QUrl("http://127.0.0.1:" + QString::number(server.serverPort()) + "/?bare=1"));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
    HttpDownloadPerformanceClient client(reply);

    QTime time;
    time.start();
    QTestEventLoop::instance().enterLoop(40);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QVERIFY(!QTestEventLoop::instance().timeout());

    qint64 elapsed = time.elapsed();
    qDebug() << "tst_QNetworkReply::httpDownloadPerformance" << elapsed << "msec, "
            << ((UploadSize/1024.0)/(elapsed/1000.0)) << " kB/sec";
}

QTEST_MAIN(tst_qnetworkreply)

#include "tst_qnetworkreply.moc"
