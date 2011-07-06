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
#include <QtCore/QCryptographicHash>
#include <QtCore/QDataStream>
#include <QtCore/QUrl>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QSharedPointer>
#include <QtCore/QScopedPointer>
#include <QtCore/QTemporaryFile>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QFtp>
#include <QtNetwork/QAbstractNetworkCache>
#include <QtNetwork/qauthenticator.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkcookie.h>
#include <QtNetwork/QHttpPart>
#include <QtNetwork/QHttpMultiPart>
#ifndef QT_NO_OPENSSL
#include <QtNetwork/qsslerror.h>
#include <QtNetwork/qsslconfiguration.h>
#endif
#ifndef QT_NO_BEARERMANAGEMENT
#include <QtNetwork/qnetworkconfigmanager.h>
#include <QtNetwork/qnetworkconfiguration.h>
#include <QtNetwork/qnetworksession.h>
#endif

#include <time.h>

#include "private/qnetworkaccessmanager_p.h"

#ifdef Q_OS_SYMBIAN
#define SRCDIR "."
#endif

#include "../network-settings.h"

Q_DECLARE_METATYPE(QSharedPointer<char>)
Q_DECLARE_METATYPE(QNetworkReply*)
Q_DECLARE_METATYPE(QAuthenticator*)
Q_DECLARE_METATYPE(QNetworkProxy)
Q_DECLARE_METATYPE(QNetworkProxyQuery)
Q_DECLARE_METATYPE(QList<QNetworkProxy>)
Q_DECLARE_METATYPE(QNetworkReply::NetworkError)
Q_DECLARE_METATYPE(QBuffer*)
Q_DECLARE_METATYPE(QHttpMultiPart *)
Q_DECLARE_METATYPE(QList<QFile*>) // for multiparts
#ifndef QT_NO_OPENSSL
Q_DECLARE_METATYPE(QSslConfiguration)
#endif

class QNetworkReplyPtr: public QSharedPointer<QNetworkReply>
{
public:
    inline QNetworkReplyPtr(QNetworkReply *ptr = 0)
        : QSharedPointer<QNetworkReply>(ptr)
    { }

    inline operator QNetworkReply *() const { return data(); }
};

class MyCookieJar;
class tst_QNetworkReply: public QObject
{
    Q_OBJECT

    struct ProxyData {
        ProxyData(const QNetworkProxy &p, const QByteArray &t, bool auth)
            : tag(t), proxy(p), requiresAuthentication(auth)
        { }
        QByteArray tag;
        QNetworkProxy proxy;
        bool requiresAuthentication;
    };

    static bool seedCreated;
    static QString createUniqueExtension() {
        if (!seedCreated) {
            qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()) + QCoreApplication::applicationPid());
            seedCreated = true; // not thread-safe, but who cares
        }
        QString s = QString("%1-%2-%3").arg(QTime(0,0,0).msecsTo(QTime::currentTime())).arg(QCoreApplication::applicationPid()).arg(qrand());
        return s;
    };

    QEventLoop *loop;
    enum RunSimpleRequestReturn { Timeout = 0, Success, Failure };
    int returnCode;
    QString testFileName;
#if !defined Q_OS_WIN
    QString wronlyFileName;
#endif
    QString uniqueExtension;
    QList<ProxyData> proxies;
    QNetworkAccessManager manager;
    MyCookieJar *cookieJar;
#ifndef QT_NO_OPENSSL
    QSslConfiguration storedSslConfiguration;
    QList<QSslError> storedExpectedSslErrors;
#endif
#ifndef QT_NO_BEARERMANAGEMENT
    QNetworkConfigurationManager *netConfMan;
    QNetworkConfiguration networkConfiguration;
    QScopedPointer<QNetworkSession> networkSession;
#endif

public:
    tst_QNetworkReply();
    ~tst_QNetworkReply();
    QString runSimpleRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request,
                             QNetworkReplyPtr &reply, const QByteArray &data = QByteArray());
    QString runMultipartRequest(const QNetworkRequest &request, QNetworkReplyPtr &reply,
                                    QHttpMultiPart *multiPart, const QByteArray &verb);

    QString runCustomRequest(const QNetworkRequest &request, QNetworkReplyPtr &reply,
                             const QByteArray &verb, QIODevice *data);

public Q_SLOTS:
    void finished();
    void gotError();
    void authenticationRequired(QNetworkReply*,QAuthenticator*);
    void proxyAuthenticationRequired(const QNetworkProxy &,QAuthenticator*);

#ifndef QT_NO_OPENSSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &);
    void storeSslConfiguration();
    void ignoreSslErrorListSlot(QNetworkReply *reply, const QList<QSslError> &);
#endif

protected Q_SLOTS:
    void nestedEventLoops_slot();

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void stateChecking();
    void invalidProtocol();
    void getFromData_data();
    void getFromData();
    void getFromFile();
    void getFromFileSpecial_data();
    void getFromFileSpecial();
    void getFromFtp_data();
    void getFromFtp();
    void getFromHttp_data();
    void getFromHttp();
    void getErrors_data();
    void getErrors();
    void putToFile_data();
    void putToFile();
    void putToFtp_data();
    void putToFtp();
    void putToHttp_data();
    void putToHttp();
    void putToHttpSynchronous_data();
    void putToHttpSynchronous();
    void putToHttpMultipart_data();
    void putToHttpMultipart();
    void postToHttp_data();
    void postToHttp();
    void postToHttpSynchronous_data();
    void postToHttpSynchronous();
    void postToHttpMultipart_data();
    void postToHttpMultipart();
    void deleteFromHttp_data();
    void deleteFromHttp();
    void putGetDeleteGetFromHttp_data();
    void putGetDeleteGetFromHttp();
    void sendCustomRequestToHttp_data();
    void sendCustomRequestToHttp();
    void connectToIPv6Address_data();
    void connectToIPv6Address();

    void ioGetFromData_data();
    void ioGetFromData();
    void ioGetFromFileSpecial_data();
    void ioGetFromFileSpecial();
    void ioGetFromFile_data();
    void ioGetFromFile();
    void ioGetFromFtp_data();
    void ioGetFromFtp();
    void ioGetFromFtpWithReuse();
    void ioGetFromHttp();

    void ioGetFromBuiltinHttp_data();
    void ioGetFromBuiltinHttp();
    void ioGetFromHttpWithReuseParallel();
    void ioGetFromHttpWithReuseSequential();
    void ioGetFromHttpWithAuth_data();
    void ioGetFromHttpWithAuth();
    void ioGetFromHttpWithAuthSynchronous();
    void ioGetFromHttpWithProxyAuth();
    void ioGetFromHttpWithProxyAuthSynchronous();
    void ioGetFromHttpWithSocksProxy();
#ifndef QT_NO_OPENSSL
    void ioGetFromHttpsWithSslErrors();
    void ioGetFromHttpsWithIgnoreSslErrors();
    void ioGetFromHttpsWithSslHandshakeError();
#endif
    void ioGetFromHttpBrokenServer_data();
    void ioGetFromHttpBrokenServer();
    void ioGetFromHttpStatus100_data();
    void ioGetFromHttpStatus100();
    void ioGetFromHttpNoHeaders_data();
    void ioGetFromHttpNoHeaders();
    void ioGetFromHttpWithCache_data();
    void ioGetFromHttpWithCache();

    void ioGetWithManyProxies_data();
    void ioGetWithManyProxies();

    void ioPutToFileFromFile_data();
    void ioPutToFileFromFile();
    void ioPutToFileFromSocket_data();
    void ioPutToFileFromSocket();
    void ioPutToFileFromLocalSocket_data();
    void ioPutToFileFromLocalSocket();
    void ioPutToFileFromProcess_data();
    void ioPutToFileFromProcess();
    void ioPutToFtpFromFile_data();
    void ioPutToFtpFromFile();
    void ioPutToHttpFromFile_data();
    void ioPutToHttpFromFile();
    void ioPostToHttpFromFile_data();
    void ioPostToHttpFromFile();
    void ioPostToHttpFromSocket_data();
    void ioPostToHttpFromSocket();
    void ioPostToHttpFromSocketSynchronous();
    void ioPostToHttpFromSocketSynchronous_data();
    void ioPostToHttpFromMiddleOfFileToEnd();
    void ioPostToHttpFromMiddleOfFileFiveBytes();
    void ioPostToHttpFromMiddleOfQBufferFiveBytes();
    void ioPostToHttpNoBufferFlag();
    void ioPostToHttpUploadProgress();
    void ioPostToHttpEmptyUploadProgress();

    void lastModifiedHeaderForFile();
    void lastModifiedHeaderForHttp();

    void httpCanReadLine();

    void rateControl_data();
    void rateControl();

    void downloadProgress_data();
    void downloadProgress();
    void uploadProgress_data();
    void uploadProgress();

    void chaining_data();
    void chaining();

    void receiveCookiesFromHttp_data();
    void receiveCookiesFromHttp();
    void receiveCookiesFromHttpSynchronous_data();
    void receiveCookiesFromHttpSynchronous();
    void sendCookies_data();
    void sendCookies();
    void sendCookiesSynchronous_data();
    void sendCookiesSynchronous();

    void nestedEventLoops();

    void httpProxyCommands_data();
    void httpProxyCommands();
    void httpProxyCommandsSynchronous_data();
    void httpProxyCommandsSynchronous();
    void proxyChange();
    void authorizationError_data();
    void authorizationError();

    void httpConnectionCount();

    void httpReUsingConnectionSequential_data();
    void httpReUsingConnectionSequential();
    void httpReUsingConnectionFromFinishedSlot_data();
    void httpReUsingConnectionFromFinishedSlot();

    void httpRecursiveCreation();

#ifndef QT_NO_OPENSSL
    void ioPostToHttpsUploadProgress();
    void ignoreSslErrorsList_data();
    void ignoreSslErrorsList();
    void ignoreSslErrorsListWithSlot_data();
    void ignoreSslErrorsListWithSlot();
    void sslConfiguration_data();
    void sslConfiguration();
#endif

    void getAndThenDeleteObject_data();
    void getAndThenDeleteObject();

    void symbianOpenCDataUrlCrash();

    void getFromHttpIntoBuffer_data();
    void getFromHttpIntoBuffer();
    void getFromHttpIntoBuffer2_data();
    void getFromHttpIntoBuffer2();

    void ioGetFromHttpWithoutContentLength();

    void ioGetFromHttpBrokenChunkedEncoding();
    void qtbug12908compressedHttpReply();
    void compressedHttpReplyBrokenGzip();

    void getFromUnreachableIp();

    void qtbug4121unknownAuthentication();

    void qtbug13431replyThrottling();

    void httpWithNoCredentialUsage();

    void qtbug15311doubleContentLength();

    void qtbug18232gzipContentLengthZero();

    void synchronousRequest_data();
    void synchronousRequest();
#ifndef QT_NO_OPENSSL
    void synchronousRequestSslFailure();
#endif

    void httpAbort();

    void dontInsertPartialContentIntoTheCache();

    // NOTE: This test must be last!
    void parentingRepliesToTheApp();
};

bool tst_QNetworkReply::seedCreated = false;

QT_BEGIN_NAMESPACE

namespace QTest {
    template<>
    char *toString(const QNetworkReply::NetworkError& code)
    {
        const QMetaObject *mo = &QNetworkReply::staticMetaObject;
        int index = mo->indexOfEnumerator("NetworkError");
        if (index == -1)
            return qstrdup("");

        QMetaEnum qme = mo->enumerator(index);
        return qstrdup(qme.valueToKey(code));
    }

    template<>
    char *toString(const QNetworkCookie &cookie)
    {
        return qstrdup(cookie.toRawForm());
    }

    template<>
    char *toString(const QList<QNetworkCookie> &list)
    {
        QString result = "QList(";
        bool first = true;
        foreach (QNetworkCookie cookie, list) {
            if (!first)
                result += ", ";
            first = false;
            result += QString::fromLatin1("QNetworkCookie(%1)").arg(QLatin1String(cookie.toRawForm()));
        }

        return qstrdup(result.append(')').toLocal8Bit());
    }
}

QT_END_NAMESPACE

#define RUN_REQUEST(call)                       \
    do {                                        \
        QString errorMsg = call;                \
        if (!errorMsg.isEmpty())                \
            QFAIL(qPrintable(errorMsg));        \
    } while (0);

#ifndef QT_NO_OPENSSL
static void setupSslServer(QSslSocket* serverSocket)
{
    serverSocket->setProtocol(QSsl::AnyProtocol);
    serverSocket->setLocalCertificate(SRCDIR "/certs/server.pem");
    serverSocket->setPrivateKey(SRCDIR "/certs/server.key");
}
#endif

// Does not work for POST/PUT!
class MiniHttpServer: public QTcpServer
{
    Q_OBJECT
public:
    QTcpSocket *client; // always the last one that was received
    QByteArray dataToTransmit;
    QByteArray receivedData;
    QSemaphore ready;
    bool doClose;
    bool doSsl;
    bool ipv6;
    bool multiple;
    int totalConnections;

    MiniHttpServer(const QByteArray &data, bool ssl = false, QThread *thread = 0, bool useipv6 = false)
        : client(0), dataToTransmit(data), doClose(true), doSsl(ssl), ipv6(useipv6),
          multiple(false), totalConnections(0)
    {
        if(useipv6) {
            listen(QHostAddress::AnyIPv6);
        } else {
            listen();
        }
        if (thread) {
            connect(thread, SIGNAL(started()), this, SLOT(threadStartedSlot()));
            moveToThread(thread);
            thread->start();
            ready.acquire();
        }
    }

protected:
    void incomingConnection(int socketDescriptor)
    {
        //qDebug() << "incomingConnection" << socketDescriptor << "doSsl:" << doSsl << "ipv6:" << ipv6;
        if (!doSsl) {
            client = new QTcpSocket;
            client->setSocketDescriptor(socketDescriptor);
            connectSocketSignals();
        } else {
#ifndef QT_NO_OPENSSL
            QSslSocket *serverSocket = new QSslSocket;
            serverSocket->setParent(this);
            if (serverSocket->setSocketDescriptor(socketDescriptor)) {
                connect(serverSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
                setupSslServer(serverSocket);
                serverSocket->startServerEncryption();
                client = serverSocket;
                connectSocketSignals();
            } else {
                delete serverSocket;
                return;
            }
#endif
        }
        client->setParent(this);
        ++totalConnections;
    }
private:
    void connectSocketSignals()
    {
        //qDebug() << "connectSocketSignals" << client;
        connect(client, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
        connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWrittenSlot()));
        connect(client, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(slotError(QAbstractSocket::SocketError)));
    }

private slots:
#ifndef QT_NO_OPENSSL
    void slotSslErrors(const QList<QSslError>& errors)
    {
        qDebug() << "slotSslErrors" << client->errorString() << errors;
    }
#endif
    void slotError(QAbstractSocket::SocketError err)
    {
        qDebug() << "slotError" << err << client->errorString();
    }

public slots:
    void readyReadSlot()
    {
        receivedData += client->readAll();
        int doubleEndlPos = receivedData.indexOf("\r\n\r\n");

        if (doubleEndlPos != -1) {
            // multiple requests incoming. remove the bytes of the current one
            if (multiple)
                receivedData.remove(0, doubleEndlPos+4);

            // we need to emulate the bytesWrittenSlot call if the data is empty.
            if (dataToTransmit.size() == 0)
                QMetaObject::invokeMethod(this, "bytesWrittenSlot", Qt::QueuedConnection);
            else
                client->write(dataToTransmit);
        }
    }

    void bytesWrittenSlot() {
        if (doClose && client->bytesToWrite() == 0) {
            client->disconnectFromHost();
            disconnect(client, 0, this, 0);
        }
    }

    void threadStartedSlot()
    {
        ready.release();
    }
};

class MyCookieJar: public QNetworkCookieJar
{
public:
    inline QList<QNetworkCookie> allCookies() const
        { return QNetworkCookieJar::allCookies(); }
    inline void setAllCookies(const QList<QNetworkCookie> &cookieList)
        { QNetworkCookieJar::setAllCookies(cookieList); }
};

class MyProxyFactory: public QNetworkProxyFactory
{
public:
    int callCount;
    QList<QNetworkProxy> toReturn;
    QNetworkProxyQuery lastQuery;
    inline MyProxyFactory() { clear(); }

    inline void clear()
    {
        callCount = 0;
        toReturn = QList<QNetworkProxy>() << QNetworkProxy::DefaultProxy;
        lastQuery = QNetworkProxyQuery();
    }

    virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)
    {
        lastQuery = query;
        ++callCount;
        return toReturn;
    }
};

class MyMemoryCache: public QAbstractNetworkCache
{
public:
    typedef QPair<QNetworkCacheMetaData, QByteArray> CachedContent;
    typedef QHash<QByteArray, CachedContent> CacheData;
    CacheData cache;

    MyMemoryCache(QObject *parent) : QAbstractNetworkCache(parent) {}

    QNetworkCacheMetaData metaData(const QUrl &url)
    {
        return cache.value(url.toEncoded()).first;
    }

    void updateMetaData(const QNetworkCacheMetaData &metaData)
    {
        cache[metaData.url().toEncoded()].first = metaData;
    }

    QIODevice *data(const QUrl &url)
    {
        CacheData::ConstIterator it = cache.find(url.toEncoded());
        if (it == cache.constEnd())
            return 0;
        QBuffer *io = new QBuffer(this);
        io->setData(it->second);
        io->open(QIODevice::ReadOnly);
        io->seek(0);
        return io;
    }

    bool remove(const QUrl &url)
    {
        cache.remove(url.toEncoded());
        return true;
    }

    qint64 cacheSize() const
    {
        qint64 total = 0;
        foreach (const CachedContent &entry, cache)
            total += entry.second.size();
        return total;
    }

    QIODevice *prepare(const QNetworkCacheMetaData &)
    {
        qFatal("%s: Should not have tried to add to the cache", Q_FUNC_INFO);
        return 0;
    }
    void insert(QIODevice *)
    {
        qFatal("%s: Should not have tried to add to the cache", Q_FUNC_INFO);
    }

    void clear() { cache.clear(); }
};
Q_DECLARE_METATYPE(MyMemoryCache::CachedContent)
Q_DECLARE_METATYPE(MyMemoryCache::CacheData)

class MySpyMemoryCache: public QAbstractNetworkCache
{
public:
    MySpyMemoryCache(QObject *parent) : QAbstractNetworkCache(parent) {}
    ~MySpyMemoryCache()
    {
        qDeleteAll(m_buffers);
        m_buffers.clear();
    }

    QHash<QUrl, QIODevice*> m_buffers;
    QList<QUrl> m_insertedUrls;

    QNetworkCacheMetaData metaData(const QUrl &)
    {
        return QNetworkCacheMetaData();
    }

    void updateMetaData(const QNetworkCacheMetaData &)
    {
    }

    QIODevice *data(const QUrl &)
    {
        return 0;
    }

    bool remove(const QUrl &url)
    {
        delete m_buffers.take(url);
        return m_insertedUrls.removeAll(url) > 0;
    }

    qint64 cacheSize() const
    {
        return 0;
    }

    QIODevice *prepare(const QNetworkCacheMetaData &metaData)
    {
        QBuffer* buffer = new QBuffer;
        buffer->open(QIODevice::ReadWrite);
        buffer->setProperty("url", metaData.url());
        m_buffers.insert(metaData.url(), buffer);
        return buffer;
    }

    void insert(QIODevice *buffer)
    {
        QUrl url = buffer->property("url").toUrl();
        m_insertedUrls << url;
        delete m_buffers.take(url);
    }

    void clear() { m_insertedUrls.clear(); }
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


class SocketPair: public QObject
{
    Q_OBJECT
public:
    QIODevice *endPoints[2];

    SocketPair(QObject *parent = 0)
        : QObject(parent)
    {
        endPoints[0] = endPoints[1] = 0;
    }

    bool create()
    {
        QTcpServer server;
        server.listen();

        QTcpSocket *active = new QTcpSocket(this);
        active->connectToHost("127.0.0.1", server.serverPort());
#ifndef Q_OS_SYMBIAN
        // need more time as working with embedded
        // device and testing from emualtor
        // things tend to get slower
        if (!active->waitForConnected(1000))
            return false;

        if (!server.waitForNewConnection(1000))
            return false;
#else
        if (!active->waitForConnected(100))
            return false;

        if (!server.waitForNewConnection(100))
            return false;
#endif
        QTcpSocket *passive = server.nextPendingConnection();
        passive->setParent(this);

        endPoints[0] = active;
        endPoints[1] = passive;
        return true;
    }
};

// A blocking tcp server (must be used in a thread) which supports SSL.
class BlockingTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    BlockingTcpServer(bool ssl) : doSsl(ssl), sslSocket(0) {}

    QTcpSocket* waitForNextConnectionSocket() {
        waitForNewConnection(-1);
        if (doSsl) {
            if (!sslSocket)
                qFatal("%s: sslSocket should not be null after calling waitForNewConnection()",
                       Q_FUNC_INFO);
            return sslSocket;
        } else {
            //qDebug() << "returning nextPendingConnection";
            return nextPendingConnection();
        }
    }
    virtual void incomingConnection(int socketDescriptor)
    {
#ifndef QT_NO_OPENSSL
        if (doSsl) {
            QSslSocket *serverSocket = new QSslSocket;
            serverSocket->setParent(this);
            serverSocket->setSocketDescriptor(socketDescriptor);
            connect(serverSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
            setupSslServer(serverSocket);
            serverSocket->startServerEncryption();
            sslSocket = serverSocket;
        } else
#endif
        {
            QTcpServer::incomingConnection(socketDescriptor);
        }
    }
private slots:

#ifndef QT_NO_OPENSSL
    void slotSslErrors(const QList<QSslError>& errors)
    {
        qDebug() << "slotSslErrors" << sslSocket->errorString() << errors;
    }
#endif

private:
    const bool doSsl;
    QTcpSocket* sslSocket;
};

// This server tries to send data as fast as possible (like most servers)
// but it measures how fast it was able to send it, which shows at which
// rate the reader is processing the data.
class FastSender: public QThread
{
    Q_OBJECT
    QSemaphore ready;
    qint64 wantedSize;
    int port;
    enum Protocol { DebugPipe, ProvidedData };
    const Protocol protocol;
    const bool doSsl;
    const bool fillKernelBuffer;
public:
    int transferRate;
    QWaitCondition cond;

    QByteArray dataToTransmit;
    int dataIndex;

    // a server that sends debugpipe data
    FastSender(qint64 size)
        : wantedSize(size), port(-1), protocol(DebugPipe),
          doSsl(false), fillKernelBuffer(true), transferRate(-1),
          dataIndex(0)
    {
        start();
        ready.acquire();
    }

    // a server that sends the data provided at construction time, useful for HTTP
    FastSender(const QByteArray& data, bool https, bool fillBuffer)
        : wantedSize(data.size()), port(-1), protocol(ProvidedData),
          doSsl(https), fillKernelBuffer(fillBuffer), transferRate(-1),
          dataToTransmit(data), dataIndex(0)
    {
        start();
        ready.acquire();
    }

    inline int serverPort() const { return port; }

    int writeNextData(QTcpSocket* socket, qint32 size)
    {
        if (protocol == DebugPipe) {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << QVariantMap() << QByteArray(size, 'a');
            socket->write((char*)&size, sizeof size);
            socket->write(data);
            dataIndex += size;
            return size;
        } else {
            const QByteArray data = dataToTransmit.mid(dataIndex, size);
            socket->write(data);
            dataIndex += data.size();
            //qDebug() << "wrote" << dataIndex << "/" << dataToTransmit.size();
            return data.size();
        }
    }
    void writeLastData(QTcpSocket* socket)
    {
        if (protocol == DebugPipe) {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << QVariantMap() << QByteArray();
            const qint32 size = data.size();
            socket->write((char*)&size, sizeof size);
            socket->write(data);
        }
    }

protected:
    void run()
    {
        BlockingTcpServer server(doSsl);
        server.listen();
        port = server.serverPort();
        ready.release();

        QTcpSocket *client = server.waitForNextConnectionSocket();

        // get the "request" packet
        if (!client->waitForReadyRead(2000)) {
            qDebug() << "FastSender:" << client->error() << "waiting for \"request\" packet";
            return;
        }
        client->readAll();      // we're not interested in the actual contents (e.g. HTTP request)

        enum { BlockSize = 1024 };

        if (fillKernelBuffer) {

            // write a bunch of bytes to fill up the buffers
            bool done = false;
            do {
                if (writeNextData(client, BlockSize) < BlockSize) {
                    qDebug() << "ERROR: FastSender: not enough data to write in order to fill buffers; or client is reading too fast";
                    return;
                }
                while (client->bytesToWrite() > 0) {
                    if (!client->waitForBytesWritten(0)) {
                        done = true;
                        break;
                    }
                }
                //qDebug() << "Filling kernel buffer: wrote" << dataIndex << "bytes";
            } while (!done);

            qDebug() << "FastSender: ok, kernel buffer is full after writing" << dataIndex << "bytes";
        }

        // Tell the client to start reading
        emit dataReady();

        // the kernel buffer is full
        // clean up QAbstractSocket's residue:
        while (client->bytesToWrite() > 0) {
            qDebug() << "Still having" << client->bytesToWrite() << "bytes to write, doing that now";
            if (!client->waitForBytesWritten(2000)) {
                qDebug() << "ERROR: FastSender:" << client->error() << "cleaning up residue";
                return;
            }
        }

        // now write in "blocking mode", this is where the rate measuring starts
        QTime timer;
        timer.start();
        //const qint64 writtenBefore = dataIndex;
        //qint64 measuredTotalBytes = wantedSize - writtenBefore;
        qint64 measuredSentBytes = 0;
        while (dataIndex < wantedSize) {
            const int remainingBytes = wantedSize - measuredSentBytes;
            const int bytesToWrite = qMin(remainingBytes, static_cast<int>(BlockSize));
            if (bytesToWrite <= 0)
                qFatal("%s: attempt to write %d bytes", Q_FUNC_INFO, bytesToWrite);
            measuredSentBytes += writeNextData(client, bytesToWrite);

            while (client->bytesToWrite() > 0) {
                if (!client->waitForBytesWritten(2000)) {
                    qDebug() << "ERROR: FastSender:" << client->error() << "during blocking write";
                    return;
                }
            }
            /*qDebug() << "FastSender:" << bytesToWrite << "bytes written now;"
                     << measuredSentBytes << "measured bytes" << measuredSentBytes + writtenBefore << "total ("
                     << measuredSentBytes*100/measuredTotalBytes << "% complete);"
                     << timer.elapsed() << "ms elapsed";*/
        }

        transferRate = measuredSentBytes * 1000 / timer.elapsed();
        qDebug() << "FastSender: flushed" << measuredSentBytes << "bytes in" << timer.elapsed() << "ms: rate =" << transferRate << "B/s";

        // write a "close connection" packet, if the protocol needs it
        writeLastData(client);
    }
signals:
    void dataReady();
};

class RateControlledReader: public QObject
{
    Q_OBJECT
    QIODevice *device;
    int bytesToRead;
    int interval;
    int readBufferSize;
public:
    QByteArray data;
    qint64 totalBytesRead;
    RateControlledReader(QObject& senderObj, QIODevice *dev, int kbPerSec, int maxBufferSize = 0)
        : device(dev), readBufferSize(maxBufferSize), totalBytesRead(0)
    {
        // determine how often we have to wake up
        int timesPerSecond;
        if (readBufferSize == 0) {
            // The requirement is simply "N KB per seconds"
            timesPerSecond = 20;
            bytesToRead = kbPerSec * 1024 / timesPerSecond;
        } else {
            // The requirement also includes "<readBufferSize> bytes at a time"
            bytesToRead = readBufferSize;
            timesPerSecond = kbPerSec * 1024 / readBufferSize;
        }
        interval = 1000 / timesPerSecond; // in ms

        qDebug() << "RateControlledReader: going to read" << bytesToRead
                 << "bytes every" << interval << "ms";
        qDebug() << "actual read rate will be"
                 << (bytesToRead * 1000 / interval) << "bytes/sec (wanted"
                 << kbPerSec * 1024 << "bytes/sec)";

        // Wait for data to be readyRead
        bool ok = connect(&senderObj, SIGNAL(dataReady()), this, SLOT(slotDataReady()));
        if (!ok)
            qFatal("%s: Cannot connect dataReady signal", Q_FUNC_INFO);
    }

    void wrapUp()
    {
        QByteArray someData = device->read(device->bytesAvailable());
        data += someData;
        totalBytesRead += someData.size();
        qDebug() << "wrapUp: found" << someData.size() << "bytes left. progress" << data.size();
        //qDebug() << "wrapUp: now bytesAvailable=" << device->bytesAvailable();
    }

private slots:
    void slotDataReady()
    {
        //qDebug() << "RateControlledReader: ready to go";
        startTimer(interval);
    }

protected:
    void timerEvent(QTimerEvent *)
    {
        //qDebug() << "RateControlledReader: timerEvent bytesAvailable=" << device->bytesAvailable();
        if (readBufferSize > 0 && device->bytesAvailable() > readBufferSize) {
            // This passes all the time, except in the final flush.
            //qFatal("%s: Too many bytes available", Q_FUNC_INFO);
        }

        qint64 bytesRead = 0;
        QTime stopWatch;
        stopWatch.start();
        do {
            if (device->bytesAvailable() == 0) {
                if (stopWatch.elapsed() > 20) {
                    qDebug() << "RateControlledReader: Not enough data available for reading, waited too much, timing out";
                    break;
                }
                if (!device->waitForReadyRead(5)) {
                    qDebug() << "RateControlledReader: Not enough data available for reading, even after waiting 5ms, bailing out";
                    break;
                }
            }
            QByteArray someData = device->read(bytesToRead - bytesRead);
            data += someData;
            bytesRead += someData.size();
            //qDebug() << "RateControlledReader: successfully read" << someData.size() << "progress:" << data.size();
        } while (bytesRead < bytesToRead);
        totalBytesRead += bytesRead;

        if (bytesRead < bytesToRead)
            qWarning() << "RateControlledReader: WARNING:" << bytesToRead - bytesRead << "bytes not read";
    }
};


tst_QNetworkReply::tst_QNetworkReply()
{
    qRegisterMetaType<QNetworkReply *>(); // for QSignalSpy
    qRegisterMetaType<QAuthenticator *>();
    qRegisterMetaType<QNetworkProxy>();
#ifndef QT_NO_OPENSSL
    qRegisterMetaType<QList<QSslError> >();
#endif
    qRegisterMetaType<QNetworkReply::NetworkError>();

    Q_SET_DEFAULT_IAP

    testFileName = QDir::currentPath() + "/testfile";
    uniqueExtension = createUniqueExtension();
    cookieJar = new MyCookieJar;
    manager.setCookieJar(cookieJar);

    QHostInfo hostInfo = QHostInfo::fromName(QtNetworkSettings::serverName());

    proxies << ProxyData(QNetworkProxy::NoProxy, "", false);

    if (hostInfo.error() == QHostInfo::NoError && !hostInfo.addresses().isEmpty()) {
        QString proxyserver = hostInfo.addresses().first().toString();
        proxies << ProxyData(QNetworkProxy(QNetworkProxy::HttpProxy, proxyserver, 3128), "+proxy", false)
                << ProxyData(QNetworkProxy(QNetworkProxy::HttpProxy, proxyserver, 3129), "+proxyauth", true)
                // currently unsupported
                // << ProxyData(QNetworkProxy(QNetworkProxy::HttpProxy, proxyserver, 3130), "+proxyauth-ntlm", true);
                << ProxyData(QNetworkProxy(QNetworkProxy::Socks5Proxy, proxyserver, 1080), "+socks", false)
                << ProxyData(QNetworkProxy(QNetworkProxy::Socks5Proxy, proxyserver, 1081), "+socksauth", true);
    } else {
        printf("==================================================================\n");
        printf("Proxy could not be looked up. No proxy will be used while testing!\n");
        printf("==================================================================\n");
    }
}

tst_QNetworkReply::~tst_QNetworkReply()
{
}


void tst_QNetworkReply::authenticationRequired(QNetworkReply*, QAuthenticator* auth)
{
    auth->setUser("httptest");
    auth->setPassword("httptest");
}

void tst_QNetworkReply::proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator* auth)
{
    auth->setUser("qsockstest");
    auth->setPassword("password");
}

#ifndef QT_NO_OPENSSL
void tst_QNetworkReply::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
    QVERIFY(!errors.isEmpty());
    QVERIFY(!reply->sslConfiguration().isNull());
}

void tst_QNetworkReply::storeSslConfiguration()
{
    storedSslConfiguration = QSslConfiguration();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply)
        storedSslConfiguration = reply->sslConfiguration();
}
#endif

QString tst_QNetworkReply::runMultipartRequest(const QNetworkRequest &request,
                                                   QNetworkReplyPtr &reply,
                                                   QHttpMultiPart *multiPart,
                                                   const QByteArray &verb)
{
    if (verb == "POST")
        reply = manager.post(request, multiPart);
    else
        reply = manager.put(request, multiPart);

    // the code below is copied from tst_QNetworkReply::runSimpleRequest, see below
    reply->setParent(this);
    connect(reply, SIGNAL(finished()), SLOT(finished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(gotError()));
    multiPart->setParent(reply);

    returnCode = Timeout;
    loop = new QEventLoop;
    QTimer::singleShot(25000, loop, SLOT(quit()));
    int code = returnCode == Timeout ? loop->exec() : returnCode;
    delete loop;
    loop = 0;

    switch (code) {
    case Failure:
        return "Request failed: " + reply->errorString();
    case Timeout:
        return "Network timeout";
    }
    return QString();
}

QString tst_QNetworkReply::runSimpleRequest(QNetworkAccessManager::Operation op,
                                            const QNetworkRequest &request,
                                            QNetworkReplyPtr &reply,
                                            const QByteArray &data)
{
    switch (op) {
    case QNetworkAccessManager::HeadOperation:
        reply = manager.head(request);
        break;

    case QNetworkAccessManager::GetOperation:
        reply = manager.get(request);
        break;

    case QNetworkAccessManager::PutOperation:
        reply = manager.put(request, data);
        break;

    case QNetworkAccessManager::PostOperation:
        reply = manager.post(request, data);
        break;

    case QNetworkAccessManager::DeleteOperation:
        reply = manager.deleteResource(request);
        break;

    default:
        qFatal("%s: Invalid/unknown operation requested", Q_FUNC_INFO);
    }
    reply->setParent(this);

    returnCode = Timeout;
    int code = Success;

    if (request.attribute(QNetworkRequest::SynchronousRequestAttribute).toBool()) {
        if (reply->isFinished())
            code = reply->error() != QNetworkReply::NoError ? Failure : Success;
        else
            code = Failure;
    } else {
        connect(reply, SIGNAL(finished()), SLOT(finished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(gotError()));

        loop = new QEventLoop;
        QTimer::singleShot(20000, loop, SLOT(quit()));
        code = returnCode == Timeout ? loop->exec() : returnCode;
        delete loop;
        loop = 0;
    }

    switch (code) {
    case Failure:
        return "Request failed: " + reply->errorString();
    case Timeout:
        return "Network timeout";
    }
    return QString();
}

QString tst_QNetworkReply::runCustomRequest(const QNetworkRequest &request,
                                            QNetworkReplyPtr &reply,
                                            const QByteArray &verb,
                                            QIODevice *data)
{
    reply = manager.sendCustomRequest(request, verb, data);
    reply->setParent(this);
    connect(reply, SIGNAL(finished()), SLOT(finished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(gotError()));

    returnCode = Timeout;
    loop = new QEventLoop;
    QTimer::singleShot(20000, loop, SLOT(quit()));
    int code = returnCode == Timeout ? loop->exec() : returnCode;
    delete loop;
    loop = 0;

    switch (code) {
    case Failure:
        return "Request failed: " + reply->errorString();
    case Timeout:
        return "Network timeout";
    }
    return QString();
}

void tst_QNetworkReply::finished()
{
    loop->exit(returnCode = Success);
}

void tst_QNetworkReply::gotError()
{
    loop->exit(returnCode = Failure);
    disconnect(QObject::sender(), SIGNAL(finished()), this, 0);
}

void tst_QNetworkReply::initTestCase()
{
#if !defined Q_OS_WIN
    wronlyFileName = QDir::currentPath() + "/write-only";
    QFile wr(wronlyFileName);
    QVERIFY(wr.open(QIODevice::WriteOnly | QIODevice::Truncate));
    wr.setPermissions(QFile::WriteOwner | QFile::WriteUser);
    wr.close();
#endif

    QDir::setSearchPaths("srcdir", QStringList() << SRCDIR);
#ifndef QT_NO_OPENSSL
    QSslSocket::defaultCaCertificates(); //preload certificates
#endif
#ifndef QT_NO_BEARERMANAGEMENT
    netConfMan = new QNetworkConfigurationManager(this);
    netConfMan->updateConfigurations();
    connect(netConfMan, SIGNAL(updateCompleted()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    networkConfiguration = netConfMan->defaultConfiguration();
    if (networkConfiguration.isValid()) {
        networkSession.reset(new QNetworkSession(networkConfiguration));
        if (!networkSession->isOpen()) {
            networkSession->open();
            QVERIFY(networkSession->waitForOpened(30000));
        }
    } else {
        QVERIFY(!(netConfMan->capabilities() & QNetworkConfigurationManager::NetworkSessionRequired));
    }
#endif
}

void tst_QNetworkReply::cleanupTestCase()
{
#if !defined Q_OS_WIN
    QFile::remove(wronlyFileName);
#endif
    if (networkSession && networkSession->isOpen()) {
        networkSession->close();
    }
}

void tst_QNetworkReply::init()
{
    cleanup();
}

void tst_QNetworkReply::cleanup()
{
    QFile file(testFileName);
    QVERIFY(!file.exists() || file.remove());

    // clear the internal cache
    QNetworkAccessManagerPrivate::clearCache(&manager);
    manager.setProxy(QNetworkProxy());
    manager.setCache(0);

    // clear cookies
    cookieJar->setAllCookies(QList<QNetworkCookie>());
}

void tst_QNetworkReply::stateChecking()
{
    QUrl url = QUrl("file:///");
    QNetworkRequest req(url);   // you can't open this file, I know
    QNetworkReplyPtr reply = manager.get(req);

    QVERIFY(reply.data());
    QVERIFY(reply->isOpen());
    QVERIFY(reply->isReadable());
    QVERIFY(!reply->isWritable());

    // both behaviours are OK since we might change underlying behaviour again
    if (!reply->isFinished())
        QCOMPARE(reply->errorString(), QString("Unknown error"));
    else
        QVERIFY(!reply->errorString().isEmpty());


    QCOMPARE(reply->manager(), &manager);
    QCOMPARE(reply->request(), req);
    QCOMPARE(int(reply->operation()), int(QNetworkAccessManager::GetOperation));
    // error and not error are OK since we might change underlying behaviour again
    if (!reply->isFinished())
        QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->url(), url);

    reply->abort();
}

void tst_QNetworkReply::invalidProtocol()
{
    QUrl url = QUrl::fromEncoded("not-a-known-protocol://foo/bar");
    QNetworkRequest req(url);
    QNetworkReplyPtr reply;

    QString errorMsg = "Request failed: Protocol \"not-a-known-protocol\" is unknown";
    QString result = runSimpleRequest(QNetworkAccessManager::GetOperation, req, reply);
    QCOMPARE(result, errorMsg);

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::ProtocolUnknownError);
}

void tst_QNetworkReply::getFromData_data()
{
    QTest::addColumn<QString>("request");
    QTest::addColumn<QByteArray>("expected");
    QTest::addColumn<QString>("mimeType");

    const QString defaultMimeType("text/plain;charset=US-ASCII");

    //QTest::newRow("empty") << "data:" << QByteArray() << defaultMimeType;
    QTest::newRow("empty2") << "data:," << QByteArray() << defaultMimeType;
    QTest::newRow("just-charset_1") << "data:charset=iso-8859-1,"
                                    << QByteArray() << "text/plain;charset=iso-8859-1";
    QTest::newRow("just-charset_2") << "data:charset = iso-8859-1 ,"
                                    << QByteArray() << "text/plain;charset = iso-8859-1";
    //QTest::newRow("just-media") << "data:text/xml" << QByteArray() << "text/xml";
    QTest::newRow("just-media2") << "data:text/xml," << QByteArray() << "text/xml";

    QTest::newRow("plain_1") << "data:,foo" << QByteArray("foo") << defaultMimeType;
    QTest::newRow("plain_2") << "data:text/html,Hello World" << QByteArray("Hello World")
                             << "text/html";
    QTest::newRow("plain_3") << "data:text/html;charset=utf-8,Hello World"
                             << QByteArray("Hello World") << "text/html;charset=utf-8";

    QTest::newRow("pct_1") << "data:,%3Cbody%20contentEditable%3Dtrue%3E%0D%0A"
                           << QByteArray("<body contentEditable=true>\r\n") << defaultMimeType;
    QTest::newRow("pct_2") << "data:text/html;charset=utf-8,%3Cbody%20contentEditable%3Dtrue%3E%0D%0A"
                           << QByteArray("<body contentEditable=true>\r\n")
                           << "text/html;charset=utf-8";

    QTest::newRow("base64-empty_1") << "data:;base64," << QByteArray() << defaultMimeType;
    QTest::newRow("base64-empty_2") << "data:charset=utf-8;base64," << QByteArray()
                                    << "text/plain;charset=utf-8";
    QTest::newRow("base64-empty_3") << "data:text/html;charset=utf-8;base64,"
                                    << QByteArray() << "text/html;charset=utf-8";

    QTest::newRow("base64_1") << "data:;base64,UXQgaXMgZ3JlYXQh" << QByteArray("Qt is great!")
                              << defaultMimeType;
    QTest::newRow("base64_2") << "data:charset=utf-8;base64,UXQgaXMgZ3JlYXQh"
                              << QByteArray("Qt is great!") << "text/plain;charset=utf-8";
    QTest::newRow("base64_3") << "data:text/html;charset=utf-8;base64,UXQgaXMgZ3JlYXQh"
                              << QByteArray("Qt is great!") << "text/html;charset=utf-8";

    QTest::newRow("pct-nul") << "data:,a%00g" << QByteArray("a\0g", 3) << defaultMimeType;
    QTest::newRow("base64-nul") << "data:;base64,YQBn" << QByteArray("a\0g", 3) << defaultMimeType;
    QTest::newRow("pct-nonutf8") << "data:,a%E1g" << QByteArray("a\xE1g", 3) << defaultMimeType;

    QTest::newRow("base64")
        << QString::fromLatin1("data:application/xml;base64,PGUvPg==")
        << QByteArray("<e/>")
        << "application/xml";

    QTest::newRow("base64, no media type")
        << QString::fromLatin1("data:;base64,PGUvPg==")
        << QByteArray("<e/>")
        << defaultMimeType;

    QTest::newRow("Percent encoding")
        << QString::fromLatin1("data:application/xml,%3Ce%2F%3E")
        << QByteArray("<e/>")
        << "application/xml";

    QTest::newRow("Percent encoding, no media type")
        << QString::fromLatin1("data:,%3Ce%2F%3E")
        << QByteArray("<e/>")
        << defaultMimeType;

    QTest::newRow("querychars")
        << QString::fromLatin1("data:,foo?x=0&y=0")
        << QByteArray("foo?x=0&y=0")
        << defaultMimeType;

    QTest::newRow("css") << "data:text/css,div%20{%20border-right:%20solid;%20}"
                         << QByteArray("div { border-right: solid; }")
                         << "text/css";
}

void tst_QNetworkReply::getFromData()
{
    QFETCH(QString, request);
    QFETCH(QByteArray, expected);
    QFETCH(QString, mimeType);

    QUrl url = QUrl::fromEncoded(request.toLatin1());
    QNetworkRequest req(url);
    QNetworkReplyPtr reply;

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, req, reply));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentTypeHeader).toString(), mimeType);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), qint64(expected.size()));
    QCOMPARE(reply->readAll(), expected);
}

void tst_QNetworkReply::getFromFile()
{
    // create the file:
    QTemporaryFile file(QDir::currentPath() + "/temp-XXXXXX");
    file.setAutoRemove(true);
    QVERIFY(file.open());

    QNetworkRequest request(QUrl::fromLocalFile(file.fileName()));
    QNetworkReplyPtr reply;

    static const char fileData[] = "This is some data that is in the file.\r\n";
    QByteArray data = QByteArray::fromRawData(fileData, sizeof fileData - 1);
    QVERIFY(file.write(data) == data.size());
    file.flush();
    QCOMPARE(file.size(), qint64(data.size()));

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), file.size());
    QCOMPARE(reply->readAll(), data);

    // make the file bigger
    file.resize(0);
    const int multiply = (128 * 1024) / (sizeof fileData - 1);
    for (int i = 0; i < multiply; ++i)
        file.write(fileData, sizeof fileData - 1);
    file.flush();

    // run again
    reply = 0;

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));
    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), file.size());
    QCOMPARE(qint64(reply->readAll().size()), file.size());
}

void tst_QNetworkReply::getFromFileSpecial_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("url");

    QTest::newRow("resource") << ":/resource" <<  "qrc:/resource";
    QTest::newRow("search-path") << "srcdir:/rfc3252.txt" << "srcdir:/rfc3252.txt";
    QTest::newRow("bigfile-path") << "srcdir:/bigfile" << "srcdir:/bigfile";
#ifdef Q_OS_WIN
    QTest::newRow("smb-path") << "srcdir:/smb-file.txt" << "file://" + QtNetworkSettings::winServerName() + "/testshare/test.pri";
#endif
}

void tst_QNetworkReply::getFromFileSpecial()
{
    QFETCH(QString, fileName);
    QFETCH(QString, url);

    // open the resource so we can find out its size
    QFile resource(fileName);
    QVERIFY(resource.open(QIODevice::ReadOnly));

    QNetworkRequest request;
    QNetworkReplyPtr reply;
    request.setUrl(url);
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), resource.size());
    QCOMPARE(reply->readAll(), resource.readAll());
}

void tst_QNetworkReply::getFromFtp_data()
{
    QTest::addColumn<QString>("referenceName");
    QTest::addColumn<QString>("url");

    QTest::newRow("rfc3252.txt") << SRCDIR "/rfc3252.txt" << "ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt";
    QTest::newRow("bigfile") << SRCDIR "/bigfile" << "ftp://" + QtNetworkSettings::serverName() + "/qtest/bigfile";
}

void tst_QNetworkReply::getFromFtp()
{
    QFETCH(QString, referenceName);
    QFETCH(QString, url);

    QFile reference(referenceName);
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
    QCOMPARE(reply->readAll(), reference.readAll());
}

void tst_QNetworkReply::getFromHttp_data()
{
    QTest::addColumn<QString>("referenceName");
    QTest::addColumn<QString>("url");

    QTest::newRow("success-internal") << SRCDIR "/rfc3252.txt" << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt";
    QTest::newRow("success-external") << SRCDIR "/rfc3252.txt" << "http://www.ietf.org/rfc/rfc3252.txt";
    QTest::newRow("bigfile-internal") << SRCDIR "/bigfile" << "http://" + QtNetworkSettings::serverName() + "/qtest/bigfile";
}

void tst_QNetworkReply::getFromHttp()
{
    QFETCH(QString, referenceName);
    QFETCH(QString, url);

    QFile reference(referenceName);
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QCOMPARE(reply->size(), reference.size());
    // only compare when the header is set.
    if (reply->header(QNetworkRequest::ContentLengthHeader).isValid())
        QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
    QCOMPARE(reply->readAll(), reference.readAll());
}

void tst_QNetworkReply::getErrors_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<int>("error");
    QTest::addColumn<int>("httpStatusCode");
    QTest::addColumn<bool>("dataIsEmpty");

    // empties
    QTest::newRow("empty-url") << QString() << int(QNetworkReply::ProtocolUnknownError) << 0 << true;
    QTest::newRow("empty-scheme-host") << SRCDIR "/rfc3252.txt" << int(QNetworkReply::ProtocolUnknownError) << 0 << true;
    QTest::newRow("empty-scheme") << "//" + QtNetworkSettings::winServerName() + "/testshare/test.pri"
            << int(QNetworkReply::ProtocolUnknownError) << 0 << true;

    // file: errors
    QTest::newRow("file-host") << "file://this-host-doesnt-exist.troll.no/foo.txt"
#if !defined Q_OS_WIN
                               << int(QNetworkReply::ProtocolInvalidOperationError) << 0 << true;
#else
                               << int(QNetworkReply::ContentNotFoundError) << 0 << true;
#endif
    QTest::newRow("file-no-path") << "file://localhost"
                                  << int(QNetworkReply::ContentOperationNotPermittedError) << 0 << true;
    QTest::newRow("file-is-dir") << QUrl::fromLocalFile(QDir::currentPath()).toString()
                                 << int(QNetworkReply::ContentOperationNotPermittedError) << 0 << true;
    QTest::newRow("file-exist") << QUrl::fromLocalFile(QDir::currentPath() + "/this-file-doesnt-exist.txt").toString()
                                << int(QNetworkReply::ContentNotFoundError) << 0 << true;
#if !defined Q_OS_WIN && !defined(Q_OS_SYMBIAN)
    QTest::newRow("file-is-wronly") << QUrl::fromLocalFile(wronlyFileName).toString()
                                    << int(QNetworkReply::ContentAccessDenied) << 0 << true;
#endif
    if (QFile::exists("/etc/shadow"))
        QTest::newRow("file-permissions") << "file:/etc/shadow"
                                          << int(QNetworkReply::ContentAccessDenied) << 0 << true;

    // ftp: errors
    QTest::newRow("ftp-host") << "ftp://this-host-doesnt-exist.troll.no/foo.txt"
                              << int(QNetworkReply::HostNotFoundError) << 0 << true;
    QTest::newRow("ftp-no-path") << "ftp://" + QtNetworkSettings::serverName()
                                 << int(QNetworkReply::ContentOperationNotPermittedError) << 0 << true;
    QTest::newRow("ftp-is-dir") << "ftp://" + QtNetworkSettings::serverName() + "/qtest"
                                << int(QNetworkReply::ContentOperationNotPermittedError) << 0 << true;
    QTest::newRow("ftp-dir-not-readable") << "ftp://" + QtNetworkSettings::serverName() + "/pub/dir-not-readable/foo.txt"
                                          << int(QNetworkReply::ContentAccessDenied) << 0 << true;
    QTest::newRow("ftp-file-not-readable") << "ftp://" + QtNetworkSettings::serverName() + "/pub/file-not-readable.txt"
                                           << int(QNetworkReply::ContentAccessDenied) << 0 << true;
    QTest::newRow("ftp-exist") << "ftp://" + QtNetworkSettings::serverName() + "/pub/this-file-doesnt-exist.txt"
                               << int(QNetworkReply::ContentNotFoundError) << 0 << true;

    // http: errors
    QTest::newRow("http-host") << "http://this-host-will-never-exist.troll.no/"
                               << int(QNetworkReply::HostNotFoundError) << 0 << true;
    QTest::newRow("http-exist") << "http://" + QtNetworkSettings::serverName() + "/this-file-doesnt-exist.txt"
                                << int(QNetworkReply::ContentNotFoundError) << 404 << false;
    QTest::newRow("http-authentication") << "http://" + QtNetworkSettings::serverName() + "/qtest/rfcs-auth"
                                         << int(QNetworkReply::AuthenticationRequiredError) << 401 << false;
}

void tst_QNetworkReply::getErrors()
{
    QFETCH(QString, url);
    QNetworkRequest request(url);

#if defined(Q_OS_WIN) || defined (Q_OS_SYMBIAN)
    if (qstrcmp(QTest::currentDataTag(), "empty-scheme-host") == 0 && QFileInfo(url).isAbsolute())
        QTest::ignoreMessage(QtWarningMsg, "QNetworkAccessFileBackendFactory: URL has no schema set, use file:// for files");
#endif

    QNetworkReplyPtr reply = manager.get(request);
    reply->setParent(this);     // we have expect-fails

    if (!reply->isFinished())
        QCOMPARE(reply->error(), QNetworkReply::NoError);

    // now run the request:
    connect(reply, SIGNAL(finished()),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    //qDebug() << reply->errorString();

    QFETCH(int, error);
#if defined(Q_OS_WIN) || defined (Q_OS_SYMBIAN)
    if (QFileInfo(url).isAbsolute())
        QEXPECT_FAIL("empty-scheme-host", "this is expected to fail on Windows and Symbian, QTBUG-17731", Abort);
#endif
    QEXPECT_FAIL("ftp-is-dir", "QFtp cannot provide enough detail", Abort);
    // the line below is not necessary
    QEXPECT_FAIL("ftp-dir-not-readable", "QFtp cannot provide enough detail", Abort);
    QCOMPARE(reply->error(), QNetworkReply::NetworkError(error));

    QTEST(reply->readAll().isEmpty(), "dataIsEmpty");

    QVERIFY(reply->isFinished());
    QVERIFY(!reply->isRunning());

    QFETCH(int, httpStatusCode);
    if (httpStatusCode != 0) {
        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), httpStatusCode);
    }
}

static inline QByteArray md5sum(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5);
}

void tst_QNetworkReply::putToFile_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("md5sum");

    QByteArray data;
    data = "";
    QTest::newRow("empty") << data << md5sum(data);

    data = "This is a normal message.";
    QTest::newRow("generic") << data << md5sum(data);

    data = "This is a message to show that Qt rocks!\r\n\n";
    QTest::newRow("small") << data << md5sum(data);

    data = QByteArray("abcd\0\1\2\abcd",12);
    QTest::newRow("with-nul") << data << md5sum(data);

    data = QByteArray(4097, '\4');
    QTest::newRow("4k+1") << data << md5sum(data);

    data = QByteArray(128*1024+1, '\177');
    QTest::newRow("128k+1") << data << md5sum(data);

    data = QByteArray(2*1024*1024+1, '\177');
    QTest::newRow("2MB+1") << data << md5sum(data);
}

void tst_QNetworkReply::putToFile()
{
    QFile file(testFileName);

    QUrl url = QUrl::fromLocalFile(file.fileName());
    QNetworkRequest request(url);
    QNetworkReplyPtr reply;

    QFETCH(QByteArray, data);

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PutOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.size(), qint64(data.size()));
    QByteArray contents = file.readAll();
    QCOMPARE(contents, data);
}

void tst_QNetworkReply::putToFtp_data()
{
    putToFile_data();
}

void tst_QNetworkReply::putToFtp()
{
    QUrl url("ftp://" + QtNetworkSettings::serverName());
    url.setPath(QString("/qtest/upload/qnetworkaccess-putToFtp-%1-%2")
                .arg(QTest::currentDataTag())
                .arg(uniqueExtension));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;

    QFETCH(QByteArray, data);

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PutOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    // download the file again from FTP to make sure it was uploaded
    // correctly
    QFtp ftp;
    ftp.connectToHost(url.host());
    ftp.login();
    ftp.get(url.path());

    QObject::connect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QObject::disconnect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));

    QByteArray uploaded = ftp.readAll();
    QCOMPARE(uploaded.size(), data.size());
    QCOMPARE(uploaded, data);

    ftp.close();
    QObject::connect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QObject::disconnect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
}

void tst_QNetworkReply::putToHttp_data()
{
    putToFile_data();
}

void tst_QNetworkReply::putToHttp()
{
    QUrl url("http://" + QtNetworkSettings::serverName());
    url.setPath(QString("/dav/qnetworkaccess-putToHttp-%1-%2")
                .arg(QTest::currentDataTag())
                .arg(uniqueExtension));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;

    QFETCH(QByteArray, data);

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PutOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 201); // 201 Created

    // download the file again from HTTP to make sure it was uploaded
    // correctly. HTTP/0.9 is enough
    QTcpSocket socket;
    socket.connectToHost(QtNetworkSettings::serverName(), 80);
    socket.write("GET " + url.toEncoded(QUrl::RemoveScheme | QUrl::RemoveAuthority) + "\r\n");
    if (!socket.waitForDisconnected(10000))
        QFAIL("Network timeout");

    QByteArray uploadedData = socket.readAll();
    QCOMPARE(uploadedData, data);
}

void tst_QNetworkReply::putToHttpSynchronous_data()
{
    uniqueExtension = createUniqueExtension();
    putToFile_data();
}

void tst_QNetworkReply::putToHttpSynchronous()
{
    QUrl url("http://" + QtNetworkSettings::serverName());
    url.setPath(QString("/dav/qnetworkaccess-putToHttp-%1-%2")
                .arg(QTest::currentDataTag())
                .arg(uniqueExtension));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;

    QFETCH(QByteArray, data);

    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PutOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 201); // 201 Created

    // download the file again from HTTP to make sure it was uploaded
    // correctly. HTTP/0.9 is enough
    QTcpSocket socket;
    socket.connectToHost(QtNetworkSettings::serverName(), 80);
    socket.write("GET " + url.toEncoded(QUrl::RemoveScheme | QUrl::RemoveAuthority) + "\r\n");
    if (!socket.waitForDisconnected(10000))
        QFAIL("Network timeout");

    QByteArray uploadedData = socket.readAll();
    QCOMPARE(uploadedData, data);
}

void tst_QNetworkReply::postToHttp_data()
{
    putToFile_data();
}

void tst_QNetworkReply::postToHttp()
{
    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi");

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply;

    QFETCH(QByteArray, data);

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PostOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QFETCH(QByteArray, md5sum);
    QByteArray uploadedData = reply->readAll().trimmed();
    QCOMPARE(uploadedData, md5sum.toHex());
}

void tst_QNetworkReply::postToHttpSynchronous_data()
{
    putToFile_data();
}

void tst_QNetworkReply::postToHttpSynchronous()
{
    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi");

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");

    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QNetworkReplyPtr reply;

    QFETCH(QByteArray, data);

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PostOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QFETCH(QByteArray, md5sum);
    QByteArray uploadedData = reply->readAll().trimmed();
    QCOMPARE(uploadedData, md5sum.toHex());
}

void tst_QNetworkReply::postToHttpMultipart_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QHttpMultiPart *>("multiPart");
    QTest::addColumn<QByteArray>("expectedReplyData");
    QTest::addColumn<QByteArray>("contentType");

    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/multipart.cgi");
    QByteArray expectedData;


    // empty parts

    QHttpMultiPart *emptyMultiPart = new QHttpMultiPart;
    QTest::newRow("empty") << url << emptyMultiPart << expectedData << QByteArray("mixed");

    QHttpMultiPart *emptyRelatedMultiPart = new QHttpMultiPart;
    emptyRelatedMultiPart->setContentType(QHttpMultiPart::RelatedType);
    QTest::newRow("empty-related") << url << emptyRelatedMultiPart << expectedData << QByteArray("related");

    QHttpMultiPart *emptyAlternativeMultiPart = new QHttpMultiPart;
    emptyAlternativeMultiPart->setContentType(QHttpMultiPart::AlternativeType);
    QTest::newRow("empty-alternative") << url << emptyAlternativeMultiPart << expectedData << QByteArray("alternative");


    // text-only parts

    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text\""));
    textPart.setBody("7 bytes");
    QHttpMultiPart *multiPart1 = new QHttpMultiPart;
    multiPart1->setContentType(QHttpMultiPart::FormDataType);
    multiPart1->append(textPart);
    expectedData = "key: text, value: 7 bytes\n";
    QTest::newRow("text") << url << multiPart1 << expectedData << QByteArray("form-data");

    QHttpMultiPart *customMultiPart = new QHttpMultiPart;
    customMultiPart->append(textPart);
    expectedData = "header: Content-Type, value: 'text/plain'\n"
                   "header: Content-Disposition, value: 'form-data; name=\"text\"'\n"
                   "content: 7 bytes\n"
                   "\n";
    QTest::newRow("text-custom") << url << customMultiPart << expectedData << QByteArray("custom");

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    textPart2.setRawHeader("myRawHeader", "myValue");
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text2\""));
    textPart2.setBody("some more bytes");
    textPart2.setBodyDevice((QIODevice *) 1); // test whether setting and unsetting of the device works
    textPart2.setBodyDevice(0);
    QHttpMultiPart *multiPart2 = new QHttpMultiPart;
    multiPart2->setContentType(QHttpMultiPart::FormDataType);
    multiPart2->append(textPart);
    multiPart2->append(textPart2);
    expectedData = "key: text2, value: some more bytes\n"
                   "key: text, value: 7 bytes\n";
    QTest::newRow("text-text") << url << multiPart2 << expectedData << QByteArray("form-data");


    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text3\""));
    textPart3.setRawHeader("Content-Location", "http://my.test.location.tld");
    textPart3.setBody("even more bytes");
    QHttpMultiPart *multiPart3 = new QHttpMultiPart;
    multiPart3->setContentType(QHttpMultiPart::AlternativeType);
    multiPart3->append(textPart);
    multiPart3->append(textPart2);
    multiPart3->append(textPart3);
    expectedData = "header: Content-Type, value: 'text/plain'\n"
                   "header: Content-Disposition, value: 'form-data; name=\"text\"'\n"
                   "content: 7 bytes\n"
                   "\n"
                   "header: Content-Type, value: 'text/plain'\n"
                   "header: myRawHeader, value: 'myValue'\n"
                   "header: Content-Disposition, value: 'form-data; name=\"text2\"'\n"
                   "content: some more bytes\n"
                   "\n"
                   "header: Content-Type, value: 'text/plain'\n"
                   "header: Content-Disposition, value: 'form-data; name=\"text3\"'\n"
                   "header: Content-Location, value: 'http://my.test.location.tld'\n"
                   "content: even more bytes\n\n";
    QTest::newRow("text-text-text") << url << multiPart3 << expectedData << QByteArray("alternative");



    // text and image parts

    QHttpPart imagePart11;
    imagePart11.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart11.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage\""));
    imagePart11.setRawHeader("Content-Location", "http://my.test.location.tld");
    imagePart11.setRawHeader("Content-ID", "my@id.tld");
    QFile *file11 = new QFile(SRCDIR "/image1.jpg");
    file11->open(QIODevice::ReadOnly);
    imagePart11.setBodyDevice(file11);
    QHttpMultiPart *imageMultiPart1 = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    imageMultiPart1->append(imagePart11);
    file11->setParent(imageMultiPart1);
    expectedData = "key: testImage, value: 87ef3bb319b004ba9e5e9c9fa713776e\n"; // md5 sum of file
    QTest::newRow("image") << url << imageMultiPart1 << expectedData << QByteArray("form-data");

    QHttpPart imagePart21;
    imagePart21.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart21.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage1\""));
    imagePart21.setRawHeader("Content-Location", "http://my.test.location.tld");
    imagePart21.setRawHeader("Content-ID", "my@id.tld");
    QFile *file21 = new QFile(SRCDIR "/image1.jpg");
    file21->open(QIODevice::ReadOnly);
    imagePart21.setBodyDevice(file21);
    QHttpMultiPart *imageMultiPart2 = new QHttpMultiPart();
    imageMultiPart2->setContentType(QHttpMultiPart::FormDataType);
    imageMultiPart2->append(textPart);
    imageMultiPart2->append(imagePart21);
    file21->setParent(imageMultiPart2);
    QHttpPart imagePart22;
    imagePart22.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart22.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage2\""));
    QFile *file22 = new QFile(SRCDIR "/image2.jpg");
    file22->open(QIODevice::ReadOnly);
    imagePart22.setBodyDevice(file22);
    imageMultiPart2->append(imagePart22);
    file22->setParent(imageMultiPart2);
    expectedData = "key: testImage1, value: 87ef3bb319b004ba9e5e9c9fa713776e\n"
                   "key: text, value: 7 bytes\n"
                   "key: testImage2, value: 483761b893f7fb1bd2414344cd1f3dfb\n";
    QTest::newRow("text-image-image") << url << imageMultiPart2 << expectedData << QByteArray("form-data");


    QHttpPart imagePart31;
    imagePart31.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart31.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage1\""));
    imagePart31.setRawHeader("Content-Location", "http://my.test.location.tld");
    imagePart31.setRawHeader("Content-ID", "my@id.tld");
    QFile *file31 = new QFile(SRCDIR "/image1.jpg");
    file31->open(QIODevice::ReadOnly);
    imagePart31.setBodyDevice(file31);
    QHttpMultiPart *imageMultiPart3 = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    imageMultiPart3->append(imagePart31);
    file31->setParent(imageMultiPart3);
    QHttpPart imagePart32;
    imagePart32.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart32.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage2\""));
    QFile *file32 = new QFile(SRCDIR "/image2.jpg");
    file32->open(QIODevice::ReadOnly);
    imagePart32.setBodyDevice(file31); // check that resetting works
    imagePart32.setBodyDevice(file32);
    imageMultiPart3->append(imagePart32);
    file32->setParent(imageMultiPart3);
    QHttpPart imagePart33;
    imagePart33.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart33.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage3\""));
    QFile *file33 = new QFile(SRCDIR "/image3.jpg");
    file33->open(QIODevice::ReadOnly);
    imagePart33.setBodyDevice(file33);
    imageMultiPart3->append(imagePart33);
    file33->setParent(imageMultiPart3);
    expectedData = "key: testImage1, value: 87ef3bb319b004ba9e5e9c9fa713776e\n"
                   "key: testImage2, value: 483761b893f7fb1bd2414344cd1f3dfb\n"
                   "key: testImage3, value: ab0eb6fd4fcf8b4436254870b4513033\n";
    QTest::newRow("3-images") << url << imageMultiPart3 << expectedData << QByteArray("form-data");


    // note: nesting multiparts is not working currently; for that, the outputDevice would need to be public

//    QHttpPart imagePart41;
//    imagePart41.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
//    QFile *file41 = new QFile(SRCDIR "/image1.jpg");
//    file41->open(QIODevice::ReadOnly);
//    imagePart41.setBodyDevice(file41);
//
//    QHttpMultiPart *innerMultiPart = new QHttpMultiPart();
//    innerMultiPart->setContentType(QHttpMultiPart::FormDataType);
//    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant());
//    innerMultiPart->append(textPart);
//    innerMultiPart->append(imagePart41);
//    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant());
//    innerMultiPart->append(textPart2);
//
//    QHttpPart nestedPart;
//    nestedPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"nestedMessage"));
//    nestedPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/alternative; boundary=\"" + innerMultiPart->boundary() + "\""));
//    innerMultiPart->outputDevice()->open(QIODevice::ReadOnly);
//    nestedPart.setBodyDevice(innerMultiPart->outputDevice());
//
//    QHttpMultiPart *outerMultiPart = new QHttpMultiPart;
//    outerMultiPart->setContentType(QHttpMultiPart::FormDataType);
//    outerMultiPart->append(textPart);
//    outerMultiPart->append(nestedPart);
//    outerMultiPart->append(textPart2);
//    expectedData = "nothing"; // the CGI.pm module running on the test server does not understand nested multiparts
//    openFiles.clear();
//    openFiles << file41;
//    QTest::newRow("nested") << url << outerMultiPart << expectedData << openFiles;


    // test setting large chunks of content with a byte array instead of a device (DISCOURAGED because of high memory consumption,
    // but we need to test that the behavior is correct)
    QHttpPart imagePart51;
    imagePart51.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart51.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"testImage\""));
    QFile *file51 = new QFile(SRCDIR "/image1.jpg");
    file51->open(QIODevice::ReadOnly);
    QByteArray imageData = file51->readAll();
    file51->close();
    delete file51;
    imagePart51.setBody("7 bytes"); // check that resetting works
    imagePart51.setBody(imageData);
    QHttpMultiPart *imageMultiPart5 = new QHttpMultiPart;
    imageMultiPart5->setContentType(QHttpMultiPart::FormDataType);
    imageMultiPart5->append(imagePart51);
    expectedData = "key: testImage, value: 87ef3bb319b004ba9e5e9c9fa713776e\n"; // md5 sum of file
    QTest::newRow("image-as-content") << url << imageMultiPart5 << expectedData << QByteArray("form-data");
}

void tst_QNetworkReply::postToHttpMultipart()
{
    QFETCH(QUrl, url);

    static QSet<QByteArray> boundaries;

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;

    QFETCH(QHttpMultiPart *, multiPart);
    QFETCH(QByteArray, expectedReplyData);
    QFETCH(QByteArray, contentType);

    // hack for testing the setting of the content-type header by hand:
    if (contentType == "custom") {
        QByteArray contentType("multipart/custom; boundary=\"" + multiPart->boundary() + "\"");
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    }

    QVERIFY2(! boundaries.contains(multiPart->boundary()), "boundary '" + multiPart->boundary() + "' has been created twice");
    boundaries.insert(multiPart->boundary());

    RUN_REQUEST(runMultipartRequest(request, reply, multiPart, "POST"));
    multiPart->deleteLater();

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QVERIFY(multiPart->boundary().count() > 20); // check that there is randomness after the "boundary_.oOo._" string
    QVERIFY(multiPart->boundary().count() < 70);
    QByteArray replyData = reply->readAll();

    expectedReplyData.prepend("content type: multipart/" + contentType + "; boundary=\"" + multiPart->boundary() + "\"\n");
//    QEXPECT_FAIL("nested", "the server does not understand nested multipart messages", Continue); // see above
    QCOMPARE(replyData, expectedReplyData);
}

void tst_QNetworkReply::putToHttpMultipart_data()
{
    postToHttpMultipart_data();
}

void tst_QNetworkReply::putToHttpMultipart()
{
    QSKIP("test server script cannot handle PUT data yet", SkipAll);
    QFETCH(QUrl, url);

    static QSet<QByteArray> boundaries;

    QNetworkRequest request(url);
    QNetworkReplyPtr reply;

    QFETCH(QHttpMultiPart *, multiPart);
    QFETCH(QByteArray, expectedReplyData);
    QFETCH(QByteArray, contentType);

    // hack for testing the setting of the content-type header by hand:
    if (contentType == "custom") {
        QByteArray contentType("multipart/custom; boundary=\"" + multiPart->boundary() + "\"");
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    }

    QVERIFY2(! boundaries.contains(multiPart->boundary()), "boundary '" + multiPart->boundary() + "' has been created twice");
    boundaries.insert(multiPart->boundary());

    RUN_REQUEST(runMultipartRequest(request, reply, multiPart, "PUT"));
    multiPart->deleteLater();

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QVERIFY(multiPart->boundary().count() > 20); // check that there is randomness after the "boundary_.oOo._" string
    QVERIFY(multiPart->boundary().count() < 70);
    QByteArray replyData = reply->readAll();

    expectedReplyData.prepend("content type: multipart/" + contentType + "; boundary=\"" + multiPart->boundary() + "\"\n");
//    QEXPECT_FAIL("nested", "the server does not understand nested multipart messages", Continue); // see above
    QCOMPARE(replyData, expectedReplyData);
}

void tst_QNetworkReply::deleteFromHttp_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<int>("resultCode");
    QTest::addColumn<QNetworkReply::NetworkError>("error");

    // for status codes to expect, see http://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html

    QTest::newRow("405-method-not-allowed") << QUrl("http://" + QtNetworkSettings::serverName() + "/index.html") << 405 << QNetworkReply::ContentOperationNotPermittedError;
    QTest::newRow("200-ok") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/http-delete.cgi?200-ok") << 200 << QNetworkReply::NoError;
    QTest::newRow("202-accepted") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/http-delete.cgi?202-accepted") << 202 << QNetworkReply::NoError;
    QTest::newRow("204-no-content") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/http-delete.cgi?204-no-content") << 204 << QNetworkReply::NoError;
    QTest::newRow("404-not-found") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/http-delete.cgi?404-not-found") << 404 << QNetworkReply::ContentNotFoundError;
}

void tst_QNetworkReply::deleteFromHttp()
{
    QFETCH(QUrl, url);
    QFETCH(int, resultCode);
    QFETCH(QNetworkReply::NetworkError, error);
    QNetworkRequest request(url);
    QNetworkReplyPtr reply;
    runSimpleRequest(QNetworkAccessManager::DeleteOperation, request, reply, 0);
    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), error);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), resultCode);
}

void tst_QNetworkReply::putGetDeleteGetFromHttp_data()
{
    QTest::addColumn<QUrl>("putUrl");
    QTest::addColumn<int>("putResultCode");
    QTest::addColumn<QNetworkReply::NetworkError>("putError");
    QTest::addColumn<QUrl>("deleteUrl");
    QTest::addColumn<int>("deleteResultCode");
    QTest::addColumn<QNetworkReply::NetworkError>("deleteError");
    QTest::addColumn<QUrl>("get2Url");
    QTest::addColumn<int>("get2ResultCode");
    QTest::addColumn<QNetworkReply::NetworkError>("get2Error");

    QUrl url("http://" + QtNetworkSettings::serverName());
    url.setPath(QString("/dav/qnetworkaccess-putToHttp-%1-%2")
                .arg(QTest::currentDataTag())
                .arg(uniqueExtension));

    // first use case: put, get (to check it is there), delete, get (to check it is not there anymore)
    QTest::newRow("success") << url << 201 << QNetworkReply::NoError << url << 204 << QNetworkReply::NoError << url << 404 << QNetworkReply::ContentNotFoundError;

    QUrl wrongUrl("http://" + QtNetworkSettings::serverName());
    wrongUrl.setPath(QString("/dav/qnetworkaccess-thisURLisNotAvailable"));

    // second use case: put, get (to check it is there), delete wrong URL, get (to check it is still there)
    QTest::newRow("delete-error") << url << 201 << QNetworkReply::NoError << wrongUrl << 404 << QNetworkReply::ContentNotFoundError << url << 200 << QNetworkReply::NoError;

}

void tst_QNetworkReply::putGetDeleteGetFromHttp()
{
    QFETCH(QUrl, putUrl);
    QFETCH(int, putResultCode);
    QFETCH(QNetworkReply::NetworkError, putError);
    QFETCH(QUrl, deleteUrl);
    QFETCH(int, deleteResultCode);
    QFETCH(QNetworkReply::NetworkError, deleteError);
    QFETCH(QUrl, get2Url);
    QFETCH(int, get2ResultCode);
    QFETCH(QNetworkReply::NetworkError, get2Error);

    QNetworkRequest putRequest(putUrl);
    QNetworkRequest deleteRequest(deleteUrl);
    QNetworkRequest get2Request(get2Url);
    QNetworkReplyPtr reply;

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PutOperation, putRequest, reply, 0));
    QCOMPARE(reply->error(), putError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), putResultCode);

    runSimpleRequest(QNetworkAccessManager::GetOperation, putRequest, reply, 0);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    runSimpleRequest(QNetworkAccessManager::DeleteOperation, deleteRequest, reply, 0);
    QCOMPARE(reply->error(), deleteError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), deleteResultCode);

    runSimpleRequest(QNetworkAccessManager::GetOperation, get2Request, reply, 0);
    QCOMPARE(reply->error(), get2Error);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), get2ResultCode);

}

void tst_QNetworkReply::connectToIPv6Address_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QNetworkReply::NetworkError>("error");
    QTest::addColumn<QByteArray>("dataToSend");
    QTest::addColumn<QByteArray>("hostfield");
    QTest::newRow("localhost") << QUrl(QByteArray("http://[::1]")) << QNetworkReply::NoError<< QByteArray("localhost") << QByteArray("[::1]");
    //QTest::newRow("ipv4localhost") << QUrl(QByteArray("http://127.0.0.1")) << QNetworkReply::NoError<< QByteArray("ipv4localhost") << QByteArray("127.0.0.1");
    //to add more test data here
}

void tst_QNetworkReply::connectToIPv6Address()
{
    QFETCH(QUrl, url);
    QFETCH(QNetworkReply::NetworkError, error);
    QFETCH(QByteArray, dataToSend);
    QFETCH(QByteArray, hostfield);

    QByteArray httpResponse = QByteArray("HTTP/1.0 200 OK\r\nContent-Length: ");
    httpResponse += QByteArray::number(dataToSend.size());
    httpResponse += "\r\n\r\n";
    httpResponse += dataToSend;

    MiniHttpServer server(httpResponse, false, NULL/*thread*/, true/*useipv6*/);
    server.doClose = true;

    url.setPort(server.serverPort());
    QNetworkRequest request(url);

    QNetworkReplyPtr reply = manager.get(request);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QByteArray content = reply->readAll();
    //qDebug() << server.receivedData;
    QByteArray hostinfo = "\r\nHost: " + hostfield + ":" + QByteArray::number(server.serverPort()) + "\r\n";
    QSKIP("Fix this -- Host Info verification failed on Windows XP", SkipAll);
    QVERIFY(server.receivedData.contains(hostinfo));
    QVERIFY(content == dataToSend);
    QCOMPARE(reply->url(), request.url());
    QVERIFY(reply->error() == error);
}

void tst_QNetworkReply::sendCustomRequestToHttp_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QByteArray>("verb");
    QTest::addColumn<QBuffer *>("device");
    QTest::addColumn<int>("resultCode");
    QTest::addColumn<QNetworkReply::NetworkError>("error");
    QTest::addColumn<QByteArray>("expectedContent");

    QTest::newRow("options") << QUrl("http://" + QtNetworkSettings::serverName()) <<
            QByteArray("OPTIONS") << (QBuffer *) 0 << 200 << QNetworkReply::NoError << QByteArray();
    QTest::newRow("trace") << QUrl("http://" + QtNetworkSettings::serverName()) <<
            QByteArray("TRACE") << (QBuffer *) 0 << 200 << QNetworkReply::NoError << QByteArray();
    QTest::newRow("connect") << QUrl("http://" + QtNetworkSettings::serverName()) <<
            QByteArray("CONNECT") << (QBuffer *) 0 << 400 << QNetworkReply::UnknownContentError << QByteArray(); // 400 = Bad Request
    QTest::newRow("nonsense") << QUrl("http://" + QtNetworkSettings::serverName()) <<
            QByteArray("NONSENSE") << (QBuffer *) 0 << 501 << QNetworkReply::ProtocolUnknownError << QByteArray(); // 501 = Method Not Implemented

    QByteArray ba("test");
    QBuffer *buffer = new QBuffer;
    buffer->setData(ba);
    buffer->open(QIODevice::ReadOnly);
    QTest::newRow("post") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi") << QByteArray("POST")
            << buffer << 200 << QNetworkReply::NoError << QByteArray("098f6bcd4621d373cade4e832627b4f6\n");

    QByteArray ba2("test");
    QBuffer *buffer2 = new QBuffer;
    buffer2->setData(ba2);
    buffer2->open(QIODevice::ReadOnly);
    QTest::newRow("put") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi") << QByteArray("PUT")
            << buffer2 << 200 << QNetworkReply::NoError << QByteArray("098f6bcd4621d373cade4e832627b4f6\n");
}

void tst_QNetworkReply::sendCustomRequestToHttp()
{
    QFETCH(QUrl, url);
    QNetworkRequest request(url);
    QNetworkReplyPtr reply;
    QFETCH(QByteArray, verb);
    QFETCH(QBuffer *, device);
    runCustomRequest(request, reply, verb, device);
    QCOMPARE(reply->url(), url);
    QFETCH(QNetworkReply::NetworkError, error);
    QCOMPARE(reply->error(), error);
    QFETCH(int, resultCode);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), resultCode);
    QFETCH(QByteArray, expectedContent);
    if (! expectedContent.isEmpty())
        QCOMPARE(reply->readAll(), expectedContent);
}

void tst_QNetworkReply::ioGetFromData_data()
{
    QTest::addColumn<QString>("urlStr");
    QTest::addColumn<QByteArray>("data");

    QTest::newRow("data-empty") << "data:," << QByteArray();
    QTest::newRow("data-literal") << "data:,foo" << QByteArray("foo");
    QTest::newRow("data-pct") << "data:,%3Cbody%20contentEditable%3Dtrue%3E%0D%0A"
                           << QByteArray("<body contentEditable=true>\r\n");
    QTest::newRow("data-base64") << "data:;base64,UXQgaXMgZ3JlYXQh" << QByteArray("Qt is great!");
}

void tst_QNetworkReply::ioGetFromData()
{
    QFETCH(QString, urlStr);

    QUrl url = QUrl::fromEncoded(urlStr.toLatin1());
    QNetworkRequest request(url);

    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply);

    connect(reply, SIGNAL(finished()),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QFETCH(QByteArray, data);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toInt(), data.size());
    QCOMPARE(reader.data.size(), data.size());
    QCOMPARE(reader.data, data);
}

void tst_QNetworkReply::ioGetFromFileSpecial_data()
{
    getFromFileSpecial_data();
}

void tst_QNetworkReply::ioGetFromFileSpecial()
{
    QFETCH(QString, fileName);
    QFETCH(QString, url);

    QFile resource(fileName);
    QVERIFY(resource.open(QIODevice::ReadOnly));

    QNetworkRequest request;
    request.setUrl(url);
    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), resource.size());
    QCOMPARE(qint64(reader.data.size()), resource.size());
    QCOMPARE(reader.data, resource.readAll());
}

void tst_QNetworkReply::ioGetFromFile_data()
{
    putToFile_data();
}

void tst_QNetworkReply::ioGetFromFile()
{
    QTemporaryFile file(QDir::currentPath() + "/temp-XXXXXX");
    file.setAutoRemove(true);
    QVERIFY(file.open());

    QFETCH(QByteArray, data);
    QVERIFY(file.write(data) == data.size());
    file.flush();
    QCOMPARE(file.size(), qint64(data.size()));

    QNetworkRequest request(QUrl::fromLocalFile(file.fileName()));
    QNetworkReplyPtr reply = manager.get(request);
    QVERIFY(reply->isFinished()); // a file should immediately be done
    DataReader reader(reply);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), file.size());
    QCOMPARE(qint64(reader.data.size()), file.size());
    QCOMPARE(reader.data, data);
}

void tst_QNetworkReply::ioGetFromFtp_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<qint64>("expectedSize");

    QTest::newRow("bigfile") << "bigfile" << Q_INT64_C(519240);

    QFile file(SRCDIR "/rfc3252.txt");
    QTest::newRow("rfc3252.txt") << "rfc3252.txt" << file.size();
}

void tst_QNetworkReply::ioGetFromFtp()
{
    QFETCH(QString, fileName);
    QFile reference(fileName);
    reference.open(QIODevice::ReadOnly); // will fail for bigfile

    QNetworkRequest request("ftp://" + QtNetworkSettings::serverName() + "/qtest/" + fileName);
    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QFETCH(qint64, expectedSize);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), expectedSize);
    QCOMPARE(qint64(reader.data.size()), expectedSize);

    if (reference.isOpen())
        QCOMPARE(reader.data, reference.readAll());
}

void tst_QNetworkReply::ioGetFromFtpWithReuse()
{
    QString fileName = SRCDIR "/rfc3252.txt";
    QFile reference(fileName);
    reference.open(QIODevice::ReadOnly);

    QNetworkRequest request(QUrl("ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));

    // two concurrent (actually, consecutive) gets:
    QNetworkReplyPtr reply1 = manager.get(request);
    DataReader reader1(reply1);
    QNetworkReplyPtr reply2 = manager.get(request);
    DataReader reader2(reply2);
    QSignalSpy spy(reply1, SIGNAL(finished()));

    connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    if (spy.count() == 0) {
        connect(reply1, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }

    QCOMPARE(reply1->url(), request.url());
    QCOMPARE(reply2->url(), request.url());
    QCOMPARE(reply1->error(), QNetworkReply::NoError);
    QCOMPARE(reply2->error(), QNetworkReply::NoError);

    QCOMPARE(qint64(reader1.data.size()), reference.size());
    QCOMPARE(qint64(reader2.data.size()), reference.size());
    QCOMPARE(reply1->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
    QCOMPARE(reply2->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());

    QByteArray referenceData = reference.readAll();
    QCOMPARE(reader1.data, referenceData);
    QCOMPARE(reader2.data, referenceData);
}

void tst_QNetworkReply::ioGetFromHttp()
{
    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
    QCOMPARE(qint64(reader.data.size()), reference.size());

    QCOMPARE(reader.data, reference.readAll());
}

void tst_QNetworkReply::ioGetFromHttpWithReuseParallel()
{
    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    QNetworkReplyPtr reply1 = manager.get(request);
    QNetworkReplyPtr reply2 = manager.get(request);
    DataReader reader1(reply1);
    DataReader reader2(reply2);
    QSignalSpy spy(reply1, SIGNAL(finished()));

    connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    if (spy.count() == 0) {
        connect(reply1, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }

    QCOMPARE(reply1->url(), request.url());
    QCOMPARE(reply2->url(), request.url());
    QCOMPARE(reply1->error(), QNetworkReply::NoError);
    QCOMPARE(reply2->error(), QNetworkReply::NoError);
    QCOMPARE(reply1->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QCOMPARE(reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    QCOMPARE(reply1->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
    QCOMPARE(reply2->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
    QCOMPARE(qint64(reader1.data.size()), reference.size());
    QCOMPARE(qint64(reader2.data.size()), reference.size());

    QByteArray referenceData = reference.readAll();
    QCOMPARE(reader1.data, referenceData);
    QCOMPARE(reader2.data, referenceData);
}

void tst_QNetworkReply::ioGetFromHttpWithReuseSequential()
{
    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    {
        QNetworkReplyPtr reply = manager.get(request);
        DataReader reader(reply);

        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());

        QCOMPARE(reply->url(), request.url());
        QCOMPARE(reply->error(), QNetworkReply::NoError);
        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

        QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
        QCOMPARE(qint64(reader.data.size()), reference.size());

        QCOMPARE(reader.data, reference.readAll());
    }

    reference.seek(0);
    // rinse and repeat:
    {
        QNetworkReplyPtr reply = manager.get(request);
        DataReader reader(reply);

        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());

        QCOMPARE(reply->url(), request.url());
        QCOMPARE(reply->error(), QNetworkReply::NoError);
        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

        QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), reference.size());
        QCOMPARE(qint64(reader.data.size()), reference.size());

        QCOMPARE(reader.data, reference.readAll());
    }
}

void tst_QNetworkReply::ioGetFromHttpWithAuth_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QByteArray>("expectedData");

    QFile reference(SRCDIR "/rfc3252.txt");
    reference.open(QIODevice::ReadOnly);
    QByteArray referenceData = reference.readAll();
    QTest::newRow("basic") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfcs-auth/rfc3252.txt") << referenceData;
    QTest::newRow("digest") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/auth-digest/") << QByteArray("digest authentication successful\n");
}

void tst_QNetworkReply::ioGetFromHttpWithAuth()
{
    // This test sends three requests
    // The first two in parallel
    // The third after the first two finished

    QFETCH(QUrl, url);
    QFETCH(QByteArray, expectedData);
    QNetworkRequest request(url);
    {
        QNetworkReplyPtr reply1 = manager.get(request);
        QNetworkReplyPtr reply2 = manager.get(request);
        DataReader reader1(reply1);
        DataReader reader2(reply2);
        QSignalSpy finishedspy(reply1, SIGNAL(finished()));

        QSignalSpy authspy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

        connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        if (finishedspy.count() == 0) {
            connect(reply1, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
            QTestEventLoop::instance().enterLoop(10);
            QVERIFY(!QTestEventLoop::instance().timeout());
        }
        manager.disconnect(SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                           this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

        QCOMPARE(reply1->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(reader1.data, expectedData);
        QCOMPARE(reader2.data, expectedData);

        QCOMPARE(authspy.count(), 1);
    }

    // rinse and repeat:
    {
        QNetworkReplyPtr reply = manager.get(request);
        DataReader reader(reply);

        QSignalSpy authspy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        manager.disconnect(SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                           this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(reader.data, expectedData);

        QCOMPARE(authspy.count(), 0);
    }

    // now check with synchronous calls:
    {
        request.setAttribute(
                QNetworkRequest::SynchronousRequestAttribute,
                true);

        QSignalSpy authspy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        QNetworkReplyPtr replySync = manager.get(request);
        QVERIFY(replySync->isFinished()); // synchronous
        QCOMPARE(authspy.count(), 0);

        // we cannot use a data reader here, since that connects to the readyRead signal,
        // just use readAll()

        // the only thing we check here is that the auth cache was used when using synchronous requests
        QCOMPARE(replySync->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(replySync->readAll(), expectedData);
    }
}

void tst_QNetworkReply::ioGetFromHttpWithAuthSynchronous()
{
    // verify that we do not enter an endless loop with synchronous calls and wrong credentials
    // the case when we succeed with the login is tested in ioGetFromHttpWithAuth()

    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfcs-auth/rfc3252.txt"));
    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QSignalSpy authspy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QNetworkReplyPtr replySync = manager.get(request);
    QVERIFY(replySync->isFinished()); // synchronous
    QCOMPARE(replySync->error(), QNetworkReply::AuthenticationRequiredError);
    QCOMPARE(authspy.count(), 0);
    QCOMPARE(replySync->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 401);
}

void tst_QNetworkReply::ioGetFromHttpWithProxyAuth()
{
    qRegisterMetaType<QNetworkProxy>(); // for QSignalSpy
    qRegisterMetaType<QAuthenticator *>();

    // This test sends three requests
    // The first two in parallel
    // The third after the first two finished
    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129);
    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    {
        manager.setProxy(proxy);
        QNetworkReplyPtr reply1 = manager.get(request);
        QNetworkReplyPtr reply2 = manager.get(request);
        manager.setProxy(QNetworkProxy());

        DataReader reader1(reply1);
        DataReader reader2(reply2);
        QSignalSpy finishedspy(reply1, SIGNAL(finished()));

        QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        if (finishedspy.count() == 0) {
            connect(reply1, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
            QTestEventLoop::instance().enterLoop(10);
            QVERIFY(!QTestEventLoop::instance().timeout());
        }
        manager.disconnect(SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                           this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        QCOMPARE(reply1->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QByteArray referenceData = reference.readAll();
        QCOMPARE(reader1.data, referenceData);
        QCOMPARE(reader2.data, referenceData);

        QCOMPARE(authspy.count(), 1);
    }

    reference.seek(0);
    // rinse and repeat:
    {
        manager.setProxy(proxy);
        QNetworkReplyPtr reply = manager.get(request);
        DataReader reader(reply);
        manager.setProxy(QNetworkProxy());

        QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        manager.disconnect(SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                           this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(reader.data, reference.readAll());

        QCOMPARE(authspy.count(), 0);
    }

    // now check with synchronous calls:
    reference.seek(0);
    {
        request.setAttribute(
                QNetworkRequest::SynchronousRequestAttribute,
                true);

        QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        QNetworkReplyPtr replySync = manager.get(request);
        QVERIFY(replySync->isFinished()); // synchronous
        QCOMPARE(authspy.count(), 0);

        // we cannot use a data reader here, since that connects to the readyRead signal,
        // just use readAll()

        // the only thing we check here is that the proxy auth cache was used when using synchronous requests
        QCOMPARE(replySync->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(replySync->readAll(), reference.readAll());
    }
}

void tst_QNetworkReply::ioGetFromHttpWithProxyAuthSynchronous()
{
    // verify that we do not enter an endless loop with synchronous calls and wrong credentials
    // the case when we succeed with the login is tested in ioGetFromHttpWithAuth()

    QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129);
    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    manager.setProxy(proxy);
    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    QNetworkReplyPtr replySync = manager.get(request);
    manager.setProxy(QNetworkProxy()); // reset
    QVERIFY(replySync->isFinished()); // synchronous
    QCOMPARE(replySync->error(), QNetworkReply::ProxyAuthenticationRequiredError);
    QCOMPARE(authspy.count(), 0);
    QCOMPARE(replySync->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 407);
}

void tst_QNetworkReply::ioGetFromHttpWithSocksProxy()
{
    // HTTP caching proxies are tested by the above function
    // test SOCKSv5 proxies too

    qRegisterMetaType<QNetworkProxy>(); // for QSignalSpy
    qRegisterMetaType<QAuthenticator *>();

    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080);
    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    {
        manager.setProxy(proxy);
        QNetworkReplyPtr reply = manager.get(request);
        DataReader reader(reply);
        manager.setProxy(QNetworkProxy());

        QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        manager.disconnect(SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                           this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
        QCOMPARE(reader.data, reference.readAll());

        QCOMPARE(authspy.count(), 0);
    }

    // set an invalid proxy just to make sure that we can't load
    proxy = QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1079);
    {
        manager.setProxy(proxy);
        QNetworkReplyPtr reply = manager.get(request);
        DataReader reader(reply);
        manager.setProxy(QNetworkProxy());

        QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
        manager.disconnect(SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                           this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        QVERIFY(!reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid());
        QVERIFY(reader.data.isEmpty());

        QVERIFY(int(reply->error()) > 0);
        QEXPECT_FAIL("", "QTcpSocket doesn't return enough information yet", Continue);
        QCOMPARE(int(reply->error()), int(QNetworkReply::ProxyConnectionRefusedError));

        QCOMPARE(authspy.count(), 0);
    }
}

#ifndef QT_NO_OPENSSL
void tst_QNetworkReply::ioGetFromHttpsWithSslErrors()
{
    qRegisterMetaType<QNetworkReply*>(); // for QSignalSpy
    qRegisterMetaType<QList<QSslError> >();

    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(QUrl("https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply);

    QSignalSpy sslspy(&manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(&manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(reply, SIGNAL(metaDataChanged()), SLOT(storeSslConfiguration()));

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    manager.disconnect(SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                       this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QCOMPARE(reader.data, reference.readAll());

    QCOMPARE(sslspy.count(), 1);

    QVERIFY(!storedSslConfiguration.isNull());
    QVERIFY(!reply->sslConfiguration().isNull());
}

void tst_QNetworkReply::ioGetFromHttpsWithIgnoreSslErrors()
{
    // same as above, except that we call ignoreSslErrors and don't connect
    // to the sslErrors() signal (which is *still* emitted)

    qRegisterMetaType<QNetworkReply*>(); // for QSignalSpy
    qRegisterMetaType<QList<QSslError> >();

    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(QUrl("https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));

    QNetworkReplyPtr reply = manager.get(request);
    reply->ignoreSslErrors();
    DataReader reader(reply);

    QSignalSpy sslspy(&manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(reply, SIGNAL(metaDataChanged()), SLOT(storeSslConfiguration()));
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QCOMPARE(reader.data, reference.readAll());

    QCOMPARE(sslspy.count(), 1);

    QVERIFY(!storedSslConfiguration.isNull());
    QVERIFY(!reply->sslConfiguration().isNull());
}

void tst_QNetworkReply::ioGetFromHttpsWithSslHandshakeError()
{
    qRegisterMetaType<QNetworkReply*>(); // for QSignalSpy
    qRegisterMetaType<QList<QSslError> >();

    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QNetworkRequest request(QUrl("https://" + QtNetworkSettings::serverName() + ":80"));

    QNetworkReplyPtr reply = manager.get(request);
    reply->ignoreSslErrors();
    DataReader reader(reply);

    QSignalSpy sslspy(&manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(reply, SIGNAL(metaDataChanged()), SLOT(storeSslConfiguration()));
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->error(), QNetworkReply::SslHandshakeFailedError);
    QCOMPARE(sslspy.count(), 0);
}
#endif

void tst_QNetworkReply::ioGetFromHttpBrokenServer_data()
{
    QTest::addColumn<QByteArray>("dataToSend");
    QTest::addColumn<bool>("doDisconnect");

    QTest::newRow("no-newline") << QByteArray("Hello World") << false;

    // these are OK now, we just eat the lonely newlines
    //QTest::newRow("just-newline") << QByteArray("\r\n") << false;
    //QTest::newRow("just-2newline") << QByteArray("\r\n\r\n") << false;

    QTest::newRow("with-newlines") << QByteArray("Long first line\r\nLong second line") << false;
    QTest::newRow("with-newlines2") << QByteArray("\r\nSecond line") << false;
    QTest::newRow("with-newlines3") << QByteArray("ICY\r\nSecond line") << false;
    QTest::newRow("invalid-version") << QByteArray("HTTP/123 200 \r\n") << false;
    QTest::newRow("invalid-version2") << QByteArray("HTTP/a.\033 200 \r\n") << false;
    QTest::newRow("invalid-reply-code") << QByteArray("HTTP/1.0 fuu \r\n") << false;

    QTest::newRow("empty+disconnect") << QByteArray() << true;

    QTest::newRow("no-newline+disconnect") << QByteArray("Hello World") << true;
    QTest::newRow("just-newline+disconnect") << QByteArray("\r\n") << true;
    QTest::newRow("just-2newline+disconnect") << QByteArray("\r\n\r\n") << true;
    QTest::newRow("with-newlines+disconnect") << QByteArray("Long first line\r\nLong second line") << true;
    QTest::newRow("with-newlines2+disconnect") << QByteArray("\r\nSecond line") << true;
    QTest::newRow("with-newlines3+disconnect") << QByteArray("ICY\r\nSecond line") << true;

    QTest::newRow("invalid-version+disconnect") << QByteArray("HTTP/123 200 ") << true;
    QTest::newRow("invalid-version2+disconnect") << QByteArray("HTTP/a.\033 200 ") << true;
    QTest::newRow("invalid-reply-code+disconnect") << QByteArray("HTTP/1.0 fuu ") << true;

    QTest::newRow("immediate disconnect") << QByteArray("") << true;
    QTest::newRow("justHalfStatus+disconnect") << QByteArray("HTTP/1.1") << true;
    QTest::newRow("justStatus+disconnect") << QByteArray("HTTP/1.1 200 OK\r\n") << true;
    QTest::newRow("justStatusAndHalfHeaders+disconnect") << QByteArray("HTTP/1.1 200 OK\r\nContent-L") << true;

    QTest::newRow("halfContent+disconnect") << QByteArray("HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\nAB") << true;

}

void tst_QNetworkReply::ioGetFromHttpBrokenServer()
{
    QFETCH(QByteArray, dataToSend);
    QFETCH(bool, doDisconnect);
    MiniHttpServer server(dataToSend);
    server.doClose = doDisconnect;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);
    QSignalSpy spy(reply, SIGNAL(error(QNetworkReply::NetworkError)));


    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(spy.count(), 1);
    QVERIFY(reply->error() != QNetworkReply::NoError);
}

void tst_QNetworkReply::ioGetFromHttpStatus100_data()
{
    QTest::addColumn<QByteArray>("dataToSend");
    QTest::addColumn<int>("statusCode");
    QTest::newRow("normal") << QByteArray("HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n") << 200;
    QTest::newRow("minimal") << QByteArray("HTTP/1.1 100 Continue\n\nHTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n") << 200;
    QTest::newRow("minimal2") << QByteArray("HTTP/1.1 100 Continue\n\nHTTP/1.0 200 OK\r\n\r\n") << 200;
    QTest::newRow("minimal3") << QByteArray("HTTP/1.1 100 Continue\n\nHTTP/1.0 200 OK\n\n") << 200;
    QTest::newRow("minimal+404") << QByteArray("HTTP/1.1 100 Continue\n\nHTTP/1.0 204 No Content\r\n\r\n") << 204;
    QTest::newRow("with_headers") << QByteArray("HTTP/1.1 100 Continue\r\nBla: x\r\n\r\nHTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n") << 200;
    QTest::newRow("with_headers2") << QByteArray("HTTP/1.1 100 Continue\nBla: x\n\nHTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n") << 200;
}

void tst_QNetworkReply::ioGetFromHttpStatus100()
{
    QFETCH(QByteArray, dataToSend);
    QFETCH(int, statusCode);
    MiniHttpServer server(dataToSend);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), statusCode);
    QVERIFY(reply->rawHeader("bla").isNull());
}

void tst_QNetworkReply::ioGetFromHttpNoHeaders_data()
{
    QTest::addColumn<QByteArray>("dataToSend");
    QTest::newRow("justStatus+noheaders+disconnect") << QByteArray("HTTP/1.0 200 OK\r\n\r\n");
}

void tst_QNetworkReply::ioGetFromHttpNoHeaders()
{
    QFETCH(QByteArray, dataToSend);
    MiniHttpServer server(dataToSend);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
}

void tst_QNetworkReply::ioGetFromHttpWithCache_data()
{
    qRegisterMetaType<MyMemoryCache::CachedContent>();
    QTest::addColumn<QByteArray>("dataToSend");
    QTest::addColumn<QString>("body");
    QTest::addColumn<MyMemoryCache::CachedContent>("cachedReply");
    QTest::addColumn<int>("cacheMode");
    QTest::addColumn<QStringList>("extraHttpHeaders");
    QTest::addColumn<bool>("loadedFromCache");
    QTest::addColumn<bool>("networkUsed");

    QByteArray reply200 =
            "HTTP/1.0 200\r\n"
            "Connection: keep-alive\r\n"
            "Content-Type: text/plain\r\n"
            "Cache-control: no-cache\r\n"
            "Content-length: 8\r\n"
            "\r\n"
            "Reloaded";
    QByteArray reply304 =
            "HTTP/1.0 304 Use Cache\r\n"
            "Connection: keep-alive\r\n"
            "\r\n";

    QTest::newRow("not-cached,always-network")
            << reply200 << "Reloaded" << MyMemoryCache::CachedContent() << int(QNetworkRequest::AlwaysNetwork) << QStringList() << false << true;
    QTest::newRow("not-cached,prefer-network")
            << reply200 << "Reloaded" << MyMemoryCache::CachedContent() << int(QNetworkRequest::PreferNetwork) << QStringList() << false << true;
    QTest::newRow("not-cached,prefer-cache")
            << reply200 << "Reloaded" << MyMemoryCache::CachedContent() << int(QNetworkRequest::PreferCache) << QStringList() << false << true;

    QDateTime present = QDateTime::currentDateTime().toUTC();
    QDateTime past = present.addSecs(-3600);
    QDateTime future = present.addSecs(3600);
    static const char dateFormat[] = "ddd, dd MMM yyyy hh:mm:ss 'GMT'";

    QNetworkCacheMetaData::RawHeaderList rawHeaders;
    MyMemoryCache::CachedContent content;
    content.second = "Not-reloaded";
    content.first.setLastModified(past);

    //
    // Set to expired
    //
    rawHeaders.clear();
    rawHeaders << QNetworkCacheMetaData::RawHeader("Date", QLocale::c().toString(past, dateFormat).toLatin1())
            << QNetworkCacheMetaData::RawHeader("Cache-control", "max-age=0"); // isn't used in cache loading
    content.first.setRawHeaders(rawHeaders);
    content.first.setLastModified(past);

    QTest::newRow("expired,200,prefer-network")
            << reply200 << "Reloaded" << content << int(QNetworkRequest::PreferNetwork) << QStringList() << false << true;
    QTest::newRow("expired,200,prefer-cache")
            << reply200 << "Reloaded" << content << int(QNetworkRequest::PreferCache) << QStringList() << false << true;

    QTest::newRow("expired,304,prefer-network")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::PreferNetwork) << QStringList() << true << true;
    QTest::newRow("expired,304,prefer-cache")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::PreferCache) << QStringList() << true << true;

    //
    // Set to not-expired
    //
    rawHeaders.clear();
    rawHeaders << QNetworkCacheMetaData::RawHeader("Date", QLocale::c().toString(past, dateFormat).toLatin1())
            << QNetworkCacheMetaData::RawHeader("Cache-control", "max-age=7200"); // isn't used in cache loading
    content.first.setRawHeaders(rawHeaders);
    content.first.setExpirationDate(future);

    QTest::newRow("not-expired,200,always-network")
            << reply200 << "Reloaded" << content << int(QNetworkRequest::AlwaysNetwork) << QStringList() << false << true;
    QTest::newRow("not-expired,200,prefer-network")
            << reply200 << "Not-reloaded" << content << int(QNetworkRequest::PreferNetwork) << QStringList() << true << false;
    QTest::newRow("not-expired,200,prefer-cache")
            << reply200 << "Not-reloaded" << content << int(QNetworkRequest::PreferCache) << QStringList() << true << false;
    QTest::newRow("not-expired,200,always-cache")
            << reply200 << "Not-reloaded" << content << int(QNetworkRequest::AlwaysCache) << QStringList() << true << false;

    QTest::newRow("not-expired,304,prefer-network")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::PreferNetwork) << QStringList() << true << false;
    QTest::newRow("not-expired,304,prefer-cache")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::PreferCache) << QStringList() << true << false;
    QTest::newRow("not-expired,304,always-cache")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::AlwaysCache) << QStringList() << true << false;

    //
    // Set must-revalidate now
    //
    rawHeaders.clear();
    rawHeaders << QNetworkCacheMetaData::RawHeader("Date", QLocale::c().toString(past, dateFormat).toLatin1())
            << QNetworkCacheMetaData::RawHeader("Cache-control", "max-age=7200, must-revalidate"); // must-revalidate is used
    content.first.setRawHeaders(rawHeaders);

    QTest::newRow("must-revalidate,200,always-network")
            << reply200 << "Reloaded" << content << int(QNetworkRequest::AlwaysNetwork) << QStringList() << false << true;
    QTest::newRow("must-revalidate,200,prefer-network")
            << reply200 << "Reloaded" << content << int(QNetworkRequest::PreferNetwork) << QStringList() << false << true;
    QTest::newRow("must-revalidate,200,prefer-cache")
            << reply200 << "Reloaded" << content << int(QNetworkRequest::PreferCache) << QStringList() << false << true;
    QTest::newRow("must-revalidate,200,always-cache")
            << reply200 << "" << content << int(QNetworkRequest::AlwaysCache) << QStringList() << false << false;

    QTest::newRow("must-revalidate,304,prefer-network")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::PreferNetwork) << QStringList() << true << true;
    QTest::newRow("must-revalidate,304,prefer-cache")
            << reply304 << "Not-reloaded" << content << int(QNetworkRequest::PreferCache) << QStringList() << true << true;
    QTest::newRow("must-revalidate,304,always-cache")
            << reply304 << "" << content << int(QNetworkRequest::AlwaysCache) << QStringList() << false << false;

    //
    // Partial content
    //
    rawHeaders.clear();
    rawHeaders << QNetworkCacheMetaData::RawHeader("Date", QLocale::c().toString(past, dateFormat).toLatin1())
            << QNetworkCacheMetaData::RawHeader("Cache-control", "max-age=7200"); // isn't used in cache loading
    content.first.setRawHeaders(rawHeaders);
    content.first.setExpirationDate(future);

    QByteArray reply206 =
            "HTTP/1.0 206\r\n"
            "Connection: keep-alive\r\n"
            "Content-Type: text/plain\r\n"
            "Cache-control: no-cache\r\n"
            "Content-Range: bytes 2-6/8\r\n"
            "Content-length: 4\r\n"
            "\r\n"
            "load";

    QTest::newRow("partial,dontuse-cache")
            << reply206 << "load" << content << int(QNetworkRequest::PreferCache) << (QStringList() << "Range" << "bytes=2-6") << false << true;
}

void tst_QNetworkReply::ioGetFromHttpWithCache()
{
    QFETCH(QByteArray, dataToSend);
    MiniHttpServer server(dataToSend);
    server.doClose = false;

    MyMemoryCache *memoryCache = new MyMemoryCache(&manager);
    manager.setCache(memoryCache);

    QFETCH(MyMemoryCache::CachedContent, cachedReply);
    QUrl url = "http://localhost:" + QString::number(server.serverPort());
    cachedReply.first.setUrl(url);
    if (!cachedReply.second.isNull())
        memoryCache->cache.insert(url.toEncoded(), cachedReply);

    QFETCH(int, cacheMode);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, cacheMode);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    QFETCH(QStringList, extraHttpHeaders);
    QStringListIterator it(extraHttpHeaders);
    while (it.hasNext()) {
        QString header = it.next();
        QString value = it.next();
        request.setRawHeader(header.toLatin1(), value.toLatin1()); // To latin1? Deal with it!
    }

    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QTEST(reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool(), "loadedFromCache");
    QTEST(server.totalConnections > 0, "networkUsed");
    QFETCH(QString, body);
    QCOMPARE(reply->readAll().constData(), qPrintable(body));
}

void tst_QNetworkReply::ioGetWithManyProxies_data()
{
    QTest::addColumn<QList<QNetworkProxy> >("proxyList");
    QTest::addColumn<QNetworkProxy>("proxyUsed");
    QTest::addColumn<QString>("url");
    QTest::addColumn<QNetworkReply::NetworkError>("expectedError");

    QList<QNetworkProxy> proxyList;

    // All of the other functions test DefaultProxy
    // So let's test something else

    // Simple tests that work:

    // HTTP request with HTTP caching proxy
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("http-on-http")
        << proxyList << proxyList.at(0)
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTP request with HTTP transparent proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("http-on-http2")
        << proxyList << proxyList.at(0)
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTP request with SOCKS transparent proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1081);
    QTest::newRow("http-on-socks")
        << proxyList << proxyList.at(0)
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // FTP request with FTP caching proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121);
    QTest::newRow("ftp-on-ftp")
        << proxyList << proxyList.at(0)
        << "ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // The following test doesn't work because QFtp is too limited
    // It can only talk to its own kind of proxies

    // FTP request with SOCKSv5 transparent proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1081);
    QTest::newRow("ftp-on-socks")
        << proxyList << proxyList.at(0)
        << "ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

#ifndef QT_NO_OPENSSL
    // HTTPS with HTTP transparent proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("https-on-http")
        << proxyList << proxyList.at(0)
        << "https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTPS request with SOCKS transparent proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1081);
    QTest::newRow("https-on-socks")
        << proxyList << proxyList.at(0)
        << "https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;
#endif

    // Tests that fail:

    // HTTP request with FTP caching proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121);
    QTest::newRow("http-on-ftp")
        << proxyList << QNetworkProxy()
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::ProxyNotFoundError;

    // FTP request with HTTP caching proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("ftp-on-http")
        << proxyList << QNetworkProxy()
        << "ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::ProxyNotFoundError;

    // FTP request with HTTP caching proxies
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3130);
    QTest::newRow("ftp-on-multiple-http")
        << proxyList << QNetworkProxy()
        << "ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::ProxyNotFoundError;

#ifndef QT_NO_OPENSSL
    // HTTPS with HTTP caching proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("https-on-httptransparent")
        << proxyList << QNetworkProxy()
        << "https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::ProxyNotFoundError;

    // HTTPS with FTP caching proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121);
    QTest::newRow("https-on-ftp")
        << proxyList << QNetworkProxy()
        << "https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::ProxyNotFoundError;
#endif

    // Complex requests:

    // HTTP request with more than one HTTP proxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3130);
    QTest::newRow("http-on-multiple-http")
        << proxyList << proxyList.at(0)
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTP request with HTTP + SOCKS
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1081);
    QTest::newRow("http-on-http+socks")
        << proxyList << proxyList.at(0)
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTP request with FTP + HTTP + SOCKS
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121)
              << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1081);
    QTest::newRow("http-on-ftp+http+socks")
        << proxyList << proxyList.at(1) // second proxy should be used
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTP request with NoProxy + HTTP
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::NoProxy)
              << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("http-on-noproxy+http")
        << proxyList << proxyList.at(0)
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTP request with FTP + NoProxy
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121)
              << QNetworkProxy(QNetworkProxy::NoProxy);
    QTest::newRow("http-on-ftp+noproxy")
        << proxyList << proxyList.at(1) // second proxy should be used
        << "http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // FTP request with HTTP Caching + FTP
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121);
    QTest::newRow("ftp-on-http+ftp")
        << proxyList << proxyList.at(1) // second proxy should be used
        << "ftp://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

#ifndef QT_NO_OPENSSL
    // HTTPS request with HTTP Caching + HTTP transparent
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::HttpProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("https-on-httpcaching+http")
        << proxyList << proxyList.at(1) // second proxy should be used
        << "https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;

    // HTTPS request with FTP + HTTP C + HTTP T
    proxyList.clear();
    proxyList << QNetworkProxy(QNetworkProxy::FtpCachingProxy, QtNetworkSettings::serverName(), 2121)
              << QNetworkProxy(QNetworkProxy::HttpCachingProxy, QtNetworkSettings::serverName(), 3129)
              << QNetworkProxy(QNetworkProxy::HttpProxy, QtNetworkSettings::serverName(), 3129);
    QTest::newRow("https-on-ftp+httpcaching+http")
        << proxyList << proxyList.at(2) // skip the first two
        << "https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"
        << QNetworkReply::NoError;
#endif
}

void tst_QNetworkReply::ioGetWithManyProxies()
{
    // Test proxy factories

    qRegisterMetaType<QNetworkProxy>(); // for QSignalSpy
    qRegisterMetaType<QAuthenticator *>();

    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    // set the proxy factory:
    QFETCH(QList<QNetworkProxy>, proxyList);
    MyProxyFactory *proxyFactory = new MyProxyFactory;
    proxyFactory->toReturn = proxyList;
    manager.setProxyFactory(proxyFactory);

    QFETCH(QString, url);
    QUrl theUrl(url);
    QNetworkRequest request(theUrl);
    QNetworkReplyPtr reply = manager.get(request);
    DataReader reader(reply);

    QSignalSpy authspy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
#ifndef QT_NO_OPENSSL
    connect(&manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif
    QTestEventLoop::instance().enterLoop(15);
    QVERIFY(!QTestEventLoop::instance().timeout());

    manager.disconnect(SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                       this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
#ifndef QT_NO_OPENSSL
    manager.disconnect(SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                       this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif

    QFETCH(QNetworkReply::NetworkError, expectedError);
    QEXPECT_FAIL("ftp-on-socks", "QFtp is too limited and won't accept non-FTP proxies", Abort);
    QCOMPARE(reply->error(), expectedError);

    // Verify that the factory was called properly
    QCOMPARE(proxyFactory->callCount, 1);
    QCOMPARE(proxyFactory->lastQuery, QNetworkProxyQuery(theUrl));

    if (expectedError == QNetworkReply::NoError) {
        // request succeeded
        QCOMPARE(reader.data, reference.readAll());

        // now verify that the proxies worked:
        QFETCH(QNetworkProxy, proxyUsed);
        if (proxyUsed.type() == QNetworkProxy::NoProxy) {
            QCOMPARE(authspy.count(), 0);
        } else {
            if (QByteArray(QTest::currentDataTag()).startsWith("ftp-"))
                return;         // No authentication with current FTP or with FTP proxies
            QCOMPARE(authspy.count(), 1);
            QCOMPARE(qvariant_cast<QNetworkProxy>(authspy.at(0).at(0)), proxyUsed);
        }
    } else {
        // request failed
        QCOMPARE(authspy.count(), 0);
    }
}

void tst_QNetworkReply::ioPutToFileFromFile_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("empty") << SRCDIR "/empty";
    QTest::newRow("real-file") << SRCDIR "/rfc3252.txt";
    QTest::newRow("resource") << ":/resource";
    QTest::newRow("search-path") << "srcdir:/rfc3252.txt";
}

void tst_QNetworkReply::ioPutToFileFromFile()
{
    QFETCH(QString, fileName);
    QFile sourceFile(fileName);
    QFile targetFile(testFileName);

    QVERIFY(sourceFile.open(QIODevice::ReadOnly));

    QUrl url = QUrl::fromLocalFile(targetFile.fileName());
    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.put(request, &sourceFile);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    QVERIFY(sourceFile.atEnd());
    sourceFile.seek(0);         // reset it to the beginning

    QVERIFY(targetFile.open(QIODevice::ReadOnly));
    QCOMPARE(targetFile.size(), sourceFile.size());
    QCOMPARE(targetFile.readAll(), sourceFile.readAll());
}

void tst_QNetworkReply::ioPutToFileFromSocket_data()
{
    putToFile_data();
}

void tst_QNetworkReply::ioPutToFileFromSocket()
{
    QFile file(testFileName);

    QUrl url = QUrl::fromLocalFile(file.fileName());
    QNetworkRequest request(url);

    QFETCH(QByteArray, data);
    SocketPair socketpair;
    socketpair.create();
    QVERIFY(socketpair.endPoints[0] && socketpair.endPoints[1]);

    socketpair.endPoints[0]->write(data);
    QNetworkReplyPtr reply = manager.put(QNetworkRequest(url), socketpair.endPoints[1]);
    socketpair.endPoints[0]->close();

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.size(), qint64(data.size()));
    QByteArray contents = file.readAll();
    QCOMPARE(contents, data);
}

void tst_QNetworkReply::ioPutToFileFromLocalSocket_data()
{
    putToFile_data();
}

void tst_QNetworkReply::ioPutToFileFromLocalSocket()
{
    QString socketname = "networkreplytest";
    QLocalServer server;
    if (!server.listen(socketname)) {
        QLocalServer::removeServer(socketname);
        QVERIFY(server.listen(socketname));
    }
    QLocalSocket active;
    active.connectToServer(socketname);
    QVERIFY2(server.waitForNewConnection(10), server.errorString().toLatin1().constData());
    QVERIFY2(active.waitForConnected(10), active.errorString().toLatin1().constData());
    QVERIFY2(server.hasPendingConnections(), server.errorString().toLatin1().constData());
    QLocalSocket *passive = server.nextPendingConnection();

    QFile file(testFileName);
    QUrl url = QUrl::fromLocalFile(file.fileName());
    QNetworkRequest request(url);

    QFETCH(QByteArray, data);
    active.write(data);
    active.close();
    QNetworkReplyPtr reply = manager.put(QNetworkRequest(url), passive);
    passive->setParent(reply);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.size(), qint64(data.size()));
    QByteArray contents = file.readAll();
    QCOMPARE(contents, data);
}

void tst_QNetworkReply::ioPutToFileFromProcess_data()
{
    putToFile_data();
}

void tst_QNetworkReply::ioPutToFileFromProcess()
{
#if defined(Q_OS_WINCE) || defined (Q_OS_SYMBIAN)
    QSKIP("Currently no stdin/out supported for Windows CE / Symbian OS", SkipAll);
#else

#ifdef Q_OS_WIN
    if (qstrcmp(QTest::currentDataTag(), "small") == 0)
        QSKIP("When passing a CR-LF-LF sequence through Windows stdio, it gets converted, "
              "so this test fails. Disabled on Windows", SkipSingle);
#endif

#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFile file(testFileName);

    QUrl url = QUrl::fromLocalFile(file.fileName());
    QNetworkRequest request(url);

    QFETCH(QByteArray, data);
    QProcess process;
    process.start("echo/echo all");
    process.write(data);
    process.closeWriteChannel();

    QNetworkReplyPtr reply = manager.put(QNetworkRequest(url), &process);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.size(), qint64(data.size()));
    QByteArray contents = file.readAll();
    QCOMPARE(contents, data);
#endif
#endif
}

void tst_QNetworkReply::ioPutToFtpFromFile_data()
{
    ioPutToFileFromFile_data();
}

void tst_QNetworkReply::ioPutToFtpFromFile()
{
    QFETCH(QString, fileName);
    QFile sourceFile(fileName);
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));

    QUrl url("ftp://" + QtNetworkSettings::serverName());
    url.setPath(QString("/qtest/upload/qnetworkaccess-ioPutToFtpFromFile-%1-%2")
                .arg(QTest::currentDataTag())
                .arg(uniqueExtension));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.put(request, &sourceFile);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(reply->readAll().isEmpty());

    QVERIFY(sourceFile.atEnd());
    sourceFile.seek(0);         // reset it to the beginning

    // download the file again from FTP to make sure it was uploaded
    // correctly
    QFtp ftp;
    ftp.connectToHost(url.host());
    ftp.login();
    ftp.get(url.path());

    QObject::connect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(3);
    QObject::disconnect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));

    QByteArray uploaded = ftp.readAll();
    QCOMPARE(qint64(uploaded.size()), sourceFile.size());
    QCOMPARE(uploaded, sourceFile.readAll());

    ftp.close();
    QObject::connect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QObject::disconnect(&ftp, SIGNAL(done(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
}

void tst_QNetworkReply::ioPutToHttpFromFile_data()
{
    ioPutToFileFromFile_data();
}

void tst_QNetworkReply::ioPutToHttpFromFile()
{
    QFETCH(QString, fileName);
    QFile sourceFile(fileName);
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));

    QUrl url("http://" + QtNetworkSettings::serverName());
    url.setPath(QString("/dav/qnetworkaccess-ioPutToHttpFromFile-%1-%2")
                .arg(QTest::currentDataTag())
                .arg(uniqueExtension));

    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.put(request, &sourceFile);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    // verify that the HTTP status code is 201 Created
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 201);

    QVERIFY(sourceFile.atEnd());
    sourceFile.seek(0);         // reset it to the beginning

    // download the file again from HTTP to make sure it was uploaded
    // correctly
    reply = manager.get(request);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QCOMPARE(reply->readAll(), sourceFile.readAll());
}

void tst_QNetworkReply::ioPostToHttpFromFile_data()
{
    ioPutToFileFromFile_data();
}

void tst_QNetworkReply::ioPostToHttpFromFile()
{
    QFETCH(QString, fileName);
    QFile sourceFile(fileName);
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));

    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi");
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");

    QNetworkReplyPtr reply = manager.post(request, &sourceFile);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    // verify that the HTTP status code is 200 Ok
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    QVERIFY(sourceFile.atEnd());
    sourceFile.seek(0);         // reset it to the beginning

    QCOMPARE(reply->readAll().trimmed(), md5sum(sourceFile.readAll()).toHex());
}

void tst_QNetworkReply::ioPostToHttpFromSocket_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("md5sum");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QNetworkProxy>("proxy");
    QTest::addColumn<int>("authenticationRequiredCount");
    QTest::addColumn<int>("proxyAuthenticationRequiredCount");

    for (int i = 0; i < proxies.count(); ++i)
        for (int auth = 0; auth < 2; ++auth) {
            QUrl url;
            if (auth)
                url = "http://" + QtNetworkSettings::serverName() + "/qtest/protected/cgi-bin/md5sum.cgi";
            else
                url = "http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi";

            QNetworkProxy proxy = proxies.at(i).proxy;
            QByteArray testsuffix = QByteArray(auth ? "+auth" : "") + proxies.at(i).tag;
            int proxyauthcount = proxies.at(i).requiresAuthentication;

            QByteArray data;
            data = "";
            QTest::newRow("empty" + testsuffix) << data << md5sum(data) << url << proxy << auth << proxyauthcount;

            data = "This is a normal message.";
            QTest::newRow("generic" + testsuffix) << data << md5sum(data) << url << proxy << auth << proxyauthcount;

            data = "This is a message to show that Qt rocks!\r\n\n";
            QTest::newRow("small" + testsuffix) << data << md5sum(data) << url << proxy << auth << proxyauthcount;

            data = QByteArray("abcd\0\1\2\abcd",12);
            QTest::newRow("with-nul" + testsuffix) << data << md5sum(data) << url << proxy << auth << proxyauthcount;

            data = QByteArray(4097, '\4');
            QTest::newRow("4k+1" + testsuffix) << data << md5sum(data) << url << proxy << auth << proxyauthcount;

            data = QByteArray(128*1024+1, '\177');
            QTest::newRow("128k+1" + testsuffix) << data << md5sum(data) << url << proxy << auth << proxyauthcount;
        }
}

void tst_QNetworkReply::ioPostToHttpFromSocket()
{
    qRegisterMetaType<QNetworkProxy>(); // for QSignalSpy
    qRegisterMetaType<QAuthenticator *>();
    qRegisterMetaType<QNetworkReply *>();

    QFETCH(QByteArray, data);
    QFETCH(QUrl, url);
    QFETCH(QNetworkProxy, proxy);
    SocketPair socketpair;
    socketpair.create();
    QVERIFY(socketpair.endPoints[0] && socketpair.endPoints[1]);

    socketpair.endPoints[0]->write(data);

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");

    manager.setProxy(proxy);
    QNetworkReplyPtr reply = manager.post(request, socketpair.endPoints[1]);
    socketpair.endPoints[0]->close();

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

    QSignalSpy authenticationRequiredSpy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QSignalSpy proxyAuthenticationRequiredSpy(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

    QTestEventLoop::instance().enterLoop(12);
    disconnect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
               this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    disconnect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
               this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    // verify that the HTTP status code is 200 Ok
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    QCOMPARE(reply->readAll().trimmed(), md5sum(data).toHex());

    QTEST(authenticationRequiredSpy.count(), "authenticationRequiredCount");
    QTEST(proxyAuthenticationRequiredSpy.count(), "proxyAuthenticationRequiredCount");
}

void tst_QNetworkReply::ioPostToHttpFromSocketSynchronous_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("md5sum");

    QByteArray data;
    data = "";
    QTest::newRow("empty") << data << md5sum(data);

    data = "This is a normal message.";
    QTest::newRow("generic") << data << md5sum(data);

    data = "This is a message to show that Qt rocks!\r\n\n";
    QTest::newRow("small") << data << md5sum(data);

    data = QByteArray("abcd\0\1\2\abcd",12);
    QTest::newRow("with-nul") << data << md5sum(data);

    data = QByteArray(4097, '\4');
    QTest::newRow("4k+1") << data << md5sum(data);

    data = QByteArray(128*1024+1, '\177');
    QTest::newRow("128k+1") << data << md5sum(data);

    data = QByteArray(2*1024*1024+1, '\177');
    QTest::newRow("2MB+1") << data << md5sum(data);
}

void tst_QNetworkReply::ioPostToHttpFromSocketSynchronous()
{
    QFETCH(QByteArray, data);

    SocketPair socketpair;
    QVERIFY(socketpair.create());
    QVERIFY(socketpair.endPoints[0] && socketpair.endPoints[1]);
    socketpair.endPoints[0]->write(data);
    socketpair.endPoints[0]->waitForBytesWritten(5000);
    // ### for 4.8: make the socket pair unbuffered, to not read everything in one go in QNetworkReplyImplPrivate::setup()
    QTestEventLoop::instance().enterLoop(3);

    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/md5sum.cgi");
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QNetworkReplyPtr reply = manager.post(request, socketpair.endPoints[1]);
    QVERIFY(reply->isFinished());
    socketpair.endPoints[0]->close();

    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    // verify that the HTTP status code is 200 Ok
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    QCOMPARE(reply->readAll().trimmed(), md5sum(data).toHex());
}

// this tests checks if rewinding the POST-data to some place in the middle
// worked.
void tst_QNetworkReply::ioPostToHttpFromMiddleOfFileToEnd()
{
    QFile sourceFile(SRCDIR "/rfc3252.txt");
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));
    // seeking to the middle
    sourceFile.seek(sourceFile.size() / 2);

    QUrl url = "http://" + QtNetworkSettings::serverName() + "/qtest/protected/cgi-bin/md5sum.cgi";
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply = manager.post(request, &sourceFile);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

    QTestEventLoop::instance().enterLoop(2);
    disconnect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
               this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QVERIFY(!QTestEventLoop::instance().timeout());

    // compare half data
    sourceFile.seek(sourceFile.size() / 2);
    QByteArray data = sourceFile.readAll();
    QCOMPARE(reply->readAll().trimmed(), md5sum(data).toHex());
}

void tst_QNetworkReply::ioPostToHttpFromMiddleOfFileFiveBytes()
{
    QFile sourceFile(SRCDIR "/rfc3252.txt");
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));
    // seeking to the middle
    sourceFile.seek(sourceFile.size() / 2);

    QUrl url = "http://" + QtNetworkSettings::serverName() + "/qtest/protected/cgi-bin/md5sum.cgi";
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    // only send 5 bytes
    request.setHeader(QNetworkRequest::ContentLengthHeader, 5);
    QVERIFY(request.header(QNetworkRequest::ContentLengthHeader).isValid());
    QNetworkReplyPtr reply = manager.post(request, &sourceFile);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

    QTestEventLoop::instance().enterLoop(2);
    disconnect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
               this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QVERIFY(!QTestEventLoop::instance().timeout());

    // compare half data
    sourceFile.seek(sourceFile.size() / 2);
    QByteArray data = sourceFile.read(5);
    QCOMPARE(reply->readAll().trimmed(), md5sum(data).toHex());
}

void tst_QNetworkReply::ioPostToHttpFromMiddleOfQBufferFiveBytes()
{
    // test needed since a QBuffer goes with a different codepath than the QFile
    // tested in ioPostToHttpFromMiddleOfFileFiveBytes
    QBuffer uploadBuffer;
    uploadBuffer.open(QIODevice::ReadWrite);
    uploadBuffer.write("1234567890");
    uploadBuffer.seek(5);

    QUrl url = "http://" + QtNetworkSettings::serverName() + "/qtest/protected/cgi-bin/md5sum.cgi";
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply = manager.post(request, &uploadBuffer);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

    QTestEventLoop::instance().enterLoop(2);
    disconnect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
               this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QVERIFY(!QTestEventLoop::instance().timeout());

    // compare half data
    uploadBuffer.seek(5);
    QByteArray data = uploadBuffer.read(5);
    QCOMPARE(reply->readAll().trimmed(), md5sum(data).toHex());
}


void tst_QNetworkReply::ioPostToHttpNoBufferFlag()
{
    QByteArray data = QByteArray("daaaaaaataaaaaaa");
    // create a sequential QIODevice by feeding the data into a local TCP server
    SocketPair socketpair;
    socketpair.create();
    QVERIFY(socketpair.endPoints[0] && socketpair.endPoints[1]);
    socketpair.endPoints[0]->write(data);

    QUrl url = "http://" + QtNetworkSettings::serverName() + "/qtest/protected/cgi-bin/md5sum.cgi";
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    // disallow buffering
    request.setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute, true);
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    QNetworkReplyPtr reply = manager.post(request, socketpair.endPoints[1]);
    socketpair.endPoints[0]->close();

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

    QTestEventLoop::instance().enterLoop(2);
    disconnect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
               this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));

    // verify: error code is QNetworkReply::ContentReSendError
    QCOMPARE(reply->error(), QNetworkReply::ContentReSendError);
}

#ifndef QT_NO_OPENSSL
class SslServer : public QTcpServer {
    Q_OBJECT
public:
    SslServer() : socket(0) {};
    void incomingConnection(int socketDescriptor) {
        QSslSocket *serverSocket = new QSslSocket;
        serverSocket->setParent(this);

        if (serverSocket->setSocketDescriptor(socketDescriptor)) {
            connect(serverSocket, SIGNAL(encrypted()), this, SLOT(encryptedSlot()));
            serverSocket->setProtocol(QSsl::AnyProtocol);
            connect(serverSocket, SIGNAL(sslErrors(const QList<QSslError>&)), serverSocket, SLOT(ignoreSslErrors()));
            serverSocket->setLocalCertificate(SRCDIR "/certs/server.pem");
            serverSocket->setPrivateKey(SRCDIR  "/certs/server.key");
            serverSocket->startServerEncryption();
        } else {
            delete serverSocket;
        }
    }
signals:
    void newEncryptedConnection();
public slots:
    void encryptedSlot() {
        socket = (QSslSocket*) sender();
        emit newEncryptedConnection();
    }
public:
    QSslSocket *socket;
};

// very similar to ioPostToHttpUploadProgress but for SSL
void tst_QNetworkReply::ioPostToHttpsUploadProgress()
{
    QFile sourceFile(SRCDIR "/bigfile");
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));

    // emulate a minimal https server
    SslServer server;
    server.listen(QHostAddress(QHostAddress::LocalHost), 0);

    // create the request
    QUrl url = QUrl(QString("https://127.0.0.1:%1/").arg(server.serverPort()));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply = manager.post(request, &sourceFile);
    QSignalSpy spy(reply, SIGNAL(uploadProgress(qint64,qint64)));
    connect(&server, SIGNAL(newEncryptedConnection()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), reply, SLOT(ignoreSslErrors()));

    // get the request started and the incoming socket connected
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QTcpSocket *incomingSocket = server.socket;
    QVERIFY(incomingSocket);
    disconnect(&server, SIGNAL(newEncryptedConnection()), &QTestEventLoop::instance(), SLOT(exitLoop()));


    incomingSocket->setReadBufferSize(1*1024);
    QTestEventLoop::instance().enterLoop(2);
    // some progress should have been made
    QVERIFY(!spy.isEmpty());
    QList<QVariant> args = spy.last();
    QVERIFY(args.at(0).toLongLong() > 0);

    QVERIFY(args.at(0).toLongLong() != sourceFile.size());

    incomingSocket->setReadBufferSize(32*1024);
    incomingSocket->read(16*1024);
    QTestEventLoop::instance().enterLoop(2);
    // some more progress than before
    QVERIFY(!spy.isEmpty());
    QList<QVariant> args2 = spy.last();
    QVERIFY(args2.at(0).toLongLong() > args.at(0).toLongLong());

    // set the read buffer to unlimited
    incomingSocket->setReadBufferSize(0);
    QTestEventLoop::instance().enterLoop(10);
    // progress should be finished
    QVERIFY(!spy.isEmpty());
    QList<QVariant> args3 = spy.last();
    QVERIFY(args3.at(0).toLongLong() > args2.at(0).toLongLong());
    QCOMPARE(args3.at(0).toLongLong(), args3.at(1).toLongLong());
    QCOMPARE(args3.at(0).toLongLong(), sourceFile.size());

    // after sending this, the QNAM should emit finished()
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    incomingSocket->write("HTTP/1.0 200 OK\r\n");
    incomingSocket->write("Content-Length: 0\r\n");
    incomingSocket->write("\r\n");
    QTestEventLoop::instance().enterLoop(10);
    // not timeouted -> finished() was emitted
    QVERIFY(!QTestEventLoop::instance().timeout());

    incomingSocket->close();
    server.close();
}
#endif

void tst_QNetworkReply::ioGetFromBuiltinHttp_data()
{
    QTest::addColumn<bool>("https");
    QTest::addColumn<int>("bufferSize");
    QTest::newRow("http+unlimited") << false << 0;
    QTest::newRow("http+limited") << false << 4096;
#ifndef QT_NO_OPENSSL
    QTest::newRow("https+unlimited") << true << 0;
    QTest::newRow("https+limited") << true << 4096;
#endif
}

void tst_QNetworkReply::ioGetFromBuiltinHttp()
{
    QSKIP("Limiting is broken right now, check QTBUG-15065", SkipAll);
    QFETCH(bool, https);
    QFETCH(int, bufferSize);

    QByteArray testData;
    // Make the data big enough so that it can fill the kernel buffer
    // (which seems to hold 202 KB here)
    const int wantedSize = 1200 * 1000;
    testData.reserve(wantedSize);
    // And in the case of SSL, the compression can fool us and let the
    // server send the data much faster than expected.
    // So better provide random data that cannot be compressed.
    for (int i = 0; i < wantedSize; ++i)
        testData += (char)qrand();

    QByteArray httpResponse = QByteArray("HTTP/1.0 200 OK\r\nContent-Length: ");
    httpResponse += QByteArray::number(testData.size());
    httpResponse += "\r\n\r\n";
    httpResponse += testData;

    qDebug() << "Server will send" << (httpResponse.size()-testData.size()) << "bytes of header and"
             << testData.size() << "bytes of data";

    const bool fillKernelBuffer = bufferSize > 0;
    FastSender server(httpResponse, https, fillKernelBuffer);

    QUrl url(QString("%1://127.0.0.1:%2/qtest/rfc3252.txt")
             .arg(https?"https":"http")
             .arg(server.serverPort()));
    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.get(request);
    reply->setReadBufferSize(bufferSize);
    reply->ignoreSslErrors();
    const int rate = 200; // in kB per sec
    RateControlledReader reader(server, reply, rate, bufferSize);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTime loopTime;
    loopTime.start();
    QTestEventLoop::instance().enterLoop(30);
    const int elapsedTime = loopTime.elapsed();
    server.wait();
    reader.wrapUp();

    qDebug() << "send rate:" << server.transferRate << "B/s";
    qDebug() << "receive rate:" << reader.totalBytesRead * 1000 / elapsedTime
             << "(it received" << reader.totalBytesRead << "bytes in" << elapsedTime << "ms)";

    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), (qint64)testData.size());
    if (reader.data.size() < testData.size()) { // oops?
        QCOMPARE(reader.data, testData.mid(0, reader.data.size()));
        qDebug() << "The data is incomplete, the last" << testData.size() - reader.data.size() << "bytes are missing";
        QEXPECT_FAIL("http+limited", "Limiting is broken right now, check QTBUG-15065", Abort);
        QEXPECT_FAIL("https+limited", "Limiting is broken right now, check QTBUG-15065", Abort);
    }
    QCOMPARE(reader.data.size(), testData.size());
    QCOMPARE(reader.data, testData);

    // OK we got the file alright, but did setReadBufferSize work?
    QVERIFY(server.transferRate != -1);
    if (bufferSize > 0) {
        const int allowedDeviation = 16; // TODO find out why the send rate is 13% faster currently
        const int minRate = rate * 1024 * (100-allowedDeviation) / 100;
        const int maxRate = rate * 1024 * (100+allowedDeviation) / 100;
        qDebug() << minRate << "<="<< server.transferRate << "<=" << maxRate << "?";
        QEXPECT_FAIL("http+limited", "Limiting is broken right now, check QTBUG-15065", Continue);
        QEXPECT_FAIL("https+limited", "Limiting is broken right now, check QTBUG-15065", Continue);
        QVERIFY(server.transferRate >= minRate && server.transferRate <= maxRate);
    }
}

void tst_QNetworkReply::ioPostToHttpUploadProgress()
{
    QFile sourceFile(SRCDIR "/bigfile");
    QVERIFY(sourceFile.open(QIODevice::ReadOnly));

    // emulate a minimal http server
    QTcpServer server;
    server.listen(QHostAddress(QHostAddress::LocalHost), 0);

    // create the request
    QUrl url = QUrl(QString("http://127.0.0.1:%1/").arg(server.serverPort()));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply = manager.post(request, &sourceFile);
    QSignalSpy spy(reply, SIGNAL(uploadProgress(qint64,qint64)));
    connect(&server, SIGNAL(newConnection()), &QTestEventLoop::instance(), SLOT(exitLoop()));

    // get the request started and the incoming socket connected
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QTcpSocket *incomingSocket = server.nextPendingConnection();
    QVERIFY(incomingSocket);
    disconnect(&server, SIGNAL(newConnection()), &QTestEventLoop::instance(), SLOT(exitLoop()));

    incomingSocket->setReadBufferSize(1*1024);
    QTestEventLoop::instance().enterLoop(5);
    // some progress should have been made
    QList<QVariant> args = spy.last();
    QVERIFY(!args.isEmpty());
    QVERIFY(args.at(0).toLongLong() > 0);
    // but not everything!
    QVERIFY(args.at(0).toLongLong() != sourceFile.size());

    // set the read buffer to unlimited
    incomingSocket->setReadBufferSize(0);
    QTestEventLoop::instance().enterLoop(10);
    // progress should be finished
    QList<QVariant> args3 = spy.last();
    QVERIFY(!args3.isEmpty());
    // More progress than before
    QVERIFY(args3.at(0).toLongLong() > args.at(0).toLongLong());
    QCOMPARE(args3.at(0).toLongLong(), args3.at(1).toLongLong());
    // And actually finished..
    QCOMPARE(args3.at(0).toLongLong(), sourceFile.size());

    // after sending this, the QNAM should emit finished()
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    incomingSocket->write("HTTP/1.0 200 OK\r\n");
    incomingSocket->write("Content-Length: 0\r\n");
    incomingSocket->write("\r\n");
    QTestEventLoop::instance().enterLoop(10);
    // not timeouted -> finished() was emitted
    QVERIFY(!QTestEventLoop::instance().timeout());

    incomingSocket->close();
    server.close();
}

void tst_QNetworkReply::ioPostToHttpEmptyUploadProgress()
{
    QByteArray ba;
    ba.resize(0);
    QBuffer buffer(&ba,0);
    QVERIFY(buffer.open(QIODevice::ReadOnly));

    // emulate a minimal http server
    QTcpServer server;
    server.listen(QHostAddress(QHostAddress::LocalHost), 0);

    // create the request
    QUrl url = QUrl(QString("http://127.0.0.1:%1/").arg(server.serverPort()));
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply = manager.post(request, &buffer);
    QSignalSpy spy(reply, SIGNAL(uploadProgress(qint64,qint64)));
    connect(&server, SIGNAL(newConnection()), &QTestEventLoop::instance(), SLOT(exitLoop()));


    // get the request started and the incoming socket connected
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QTcpSocket *incomingSocket = server.nextPendingConnection();
    QVERIFY(incomingSocket);

    // after sending this, the QNAM should emit finished()
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    incomingSocket->write("HTTP/1.0 200 OK\r\n");
    incomingSocket->write("Content-Length: 0\r\n");
    incomingSocket->write("\r\n");
    incomingSocket->flush();
    QTestEventLoop::instance().enterLoop(10);
    // not timeouted -> finished() was emitted
    QVERIFY(!QTestEventLoop::instance().timeout());

    // final check: only 1 uploadProgress has been emitted
    QVERIFY(spy.length() == 1);
    QList<QVariant> args = spy.last();
    QVERIFY(!args.isEmpty());
    QCOMPARE(args.at(0).toLongLong(), buffer.size());
    QCOMPARE(args.at(0).toLongLong(), buffer.size());

    incomingSocket->close();
    server.close();
}

void tst_QNetworkReply::lastModifiedHeaderForFile()
{
    QFileInfo fileInfo(SRCDIR "/bigfile");
    QVERIFY(fileInfo.exists());

    QUrl url = QUrl::fromLocalFile(fileInfo.filePath());

    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.head(request);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QDateTime header = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
    QCOMPARE(header, fileInfo.lastModified());
}

void tst_QNetworkReply::lastModifiedHeaderForHttp()
{
    // Tue, 22 May 2007 12:04:57 GMT according to webserver
    QUrl url = "http://" + QtNetworkSettings::serverName() + "/qtest/fluke.gif";

    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.head(request);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QDateTime header = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
    QDateTime realDate = QDateTime::fromString("2007-05-22T12:04:57", Qt::ISODate);
    realDate.setTimeSpec(Qt::UTC);

    QCOMPARE(header, realDate);
}

void tst_QNetworkReply::httpCanReadLine()
{
    QNetworkRequest request(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt"));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QVERIFY(reply->canReadLine());
    QVERIFY(!reply->readAll().isEmpty());
    QVERIFY(!reply->canReadLine());
}

void tst_QNetworkReply::rateControl_data()
{
    QTest::addColumn<int>("rate");

    QTest::newRow("15") << 15;
    QTest::newRow("40") << 40;
    QTest::newRow("73") << 73;
    QTest::newRow("80") << 80;
    QTest::newRow("125") << 125;
    QTest::newRow("250") << 250;
    QTest::newRow("1024") << 1024;
}

void tst_QNetworkReply::rateControl()
{
    QSKIP("Test disabled -- only for manual purposes", SkipAll);
    // this function tests that we aren't reading from the network
    // faster than the data is being consumed.
    QFETCH(int, rate);

    // ask for 20 seconds worth of data
    FastSender sender(20 * rate * 1024);

    QNetworkRequest request("debugpipe://localhost:" + QString::number(sender.serverPort()));
    QNetworkReplyPtr reply = manager.get(request);
    reply->setReadBufferSize(32768);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
    QSignalSpy errorSpy(reply, SIGNAL(error(QNetworkReply::NetworkError)));

    RateControlledReader reader(sender, reply, rate, 20);

    // this test is designed to run for 25 seconds at most
    QTime loopTime;
    loopTime.start();
    QTestEventLoop::instance().enterLoop(40);
    int elapsedTime = loopTime.elapsed();

    if (!errorSpy.isEmpty()) {
        qDebug() << "ERROR!" << errorSpy[0][0] << reply->errorString();
    }

    qDebug() << "tst_QNetworkReply::rateControl" << "send rate:" << sender.transferRate;
    qDebug() << "tst_QNetworkReply::rateControl" << "receive rate:" << reader.totalBytesRead * 1000 / elapsedTime
             << "(it received" << reader.totalBytesRead << "bytes in" << elapsedTime << "ms)";

    sender.wait();
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QVERIFY(sender.transferRate != -1);
    int minRate = rate * 1024 * 9 / 10;
    int maxRate = rate * 1024 * 11 / 10;
    QVERIFY(sender.transferRate >= minRate);
    QVERIFY(sender.transferRate <= maxRate);
}

void tst_QNetworkReply::downloadProgress_data()
{
    QTest::addColumn<int>("loopCount");

    QTest::newRow("empty") << 0;
    QTest::newRow("small") << 4;
#ifndef Q_OS_SYMBIAN
    QTest::newRow("big") << 4096;
#else
    // it can run even with 4096
    // but it takes lot time
    //especially on emulator
    QTest::newRow("big") << 1024;
#endif
}

void tst_QNetworkReply::downloadProgress()
{
    QTcpServer server;
    QVERIFY(server.listen());

    QNetworkRequest request("debugpipe://127.0.0.1:" + QString::number(server.serverPort()) + "/?bare=1");
    QNetworkReplyPtr reply = manager.get(request);
    QSignalSpy spy(reply, SIGNAL(downloadProgress(qint64,qint64)));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QVERIFY(spy.isValid());
    QVERIFY(!reply->isFinished());
    QVERIFY(reply->isRunning());

    QCoreApplication::instance()->processEvents();
    if (!server.hasPendingConnections())
        server.waitForNewConnection(1000);
    QVERIFY(server.hasPendingConnections());
    QCOMPARE(spy.count(), 0);

    QByteArray data(128, 'a');
    QTcpSocket *sender = server.nextPendingConnection();
    QVERIFY(sender);

    QFETCH(int, loopCount);
    for (int i = 1; i <= loopCount; ++i) {
        sender->write(data);
        QVERIFY2(sender->waitForBytesWritten(2000), "Network timeout");

        spy.clear();
        QTestEventLoop::instance().enterLoop(2);
        QVERIFY(!QTestEventLoop::instance().timeout());
        QVERIFY(spy.count() > 0);
        QVERIFY(!reply->isFinished());
        QVERIFY(reply->isRunning());

        QList<QVariant> args = spy.last();
        QCOMPARE(args.at(0).toInt(), i*data.size());
        QCOMPARE(args.at(1).toInt(), -1);
    }

    // close the connection:
    delete sender;

    spy.clear();
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(spy.count() > 0);
    QVERIFY(!reply->isRunning());
    QVERIFY(reply->isFinished());

    QList<QVariant> args = spy.last();
    QCOMPARE(args.at(0).toInt(), loopCount * data.size());
    QCOMPARE(args.at(1).toInt(), loopCount * data.size());
}

void tst_QNetworkReply::uploadProgress_data()
{
    putToFile_data();
}

void tst_QNetworkReply::uploadProgress()
{
    QFETCH(QByteArray, data);
    QTcpServer server;
    QVERIFY(server.listen());

    QNetworkRequest request("debugpipe://127.0.0.1:" + QString::number(server.serverPort()) + "/?bare=1");
    QNetworkReplyPtr reply = manager.put(request, data);
    QSignalSpy spy(reply, SIGNAL(uploadProgress(qint64,qint64)));
    QSignalSpy finished(reply, SIGNAL(finished()));
    QVERIFY(spy.isValid());
    QVERIFY(finished.isValid());

    QCoreApplication::instance()->processEvents();
    if (!server.hasPendingConnections())
        server.waitForNewConnection(1000);
    QVERIFY(server.hasPendingConnections());

    QTcpSocket *receiver = server.nextPendingConnection();
    if (finished.count() == 0) {
        // it's not finished yet, so wait for it to be
        connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(2);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }
    delete receiver;

    QVERIFY(finished.count() > 0);
    QVERIFY(spy.count() > 0);

    QList<QVariant> args = spy.last();
    QCOMPARE(args.at(0).toInt(), data.size());
    QCOMPARE(args.at(1).toInt(), data.size());
}

void tst_QNetworkReply::chaining_data()
{
    putToFile_data();
}

void tst_QNetworkReply::chaining()
{
    QTemporaryFile sourceFile(QDir::currentPath() + "/temp-XXXXXX");
    sourceFile.setAutoRemove(true);
    QVERIFY(sourceFile.open());

    QFETCH(QByteArray, data);
    QVERIFY(sourceFile.write(data) == data.size());
    sourceFile.flush();
    QCOMPARE(sourceFile.size(), qint64(data.size()));

    QNetworkRequest request(QUrl::fromLocalFile(sourceFile.fileName()));
    QNetworkReplyPtr getReply = manager.get(request);

    QFile targetFile(testFileName);
    QUrl url = QUrl::fromLocalFile(targetFile.fileName());
    request.setUrl(url);
    QNetworkReplyPtr putReply = manager.put(request, getReply);

    connect(putReply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(getReply->url(), QUrl::fromLocalFile(sourceFile.fileName()));
    QCOMPARE(getReply->error(), QNetworkReply::NoError);
    QCOMPARE(getReply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), sourceFile.size());

    QCOMPARE(putReply->url(), url);
    QCOMPARE(putReply->error(), QNetworkReply::NoError);
    QCOMPARE(putReply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), Q_INT64_C(0));
    QVERIFY(putReply->readAll().isEmpty());

    QVERIFY(sourceFile.atEnd());
    sourceFile.seek(0);         // reset it to the beginning

    QVERIFY(targetFile.open(QIODevice::ReadOnly));
    QCOMPARE(targetFile.size(), sourceFile.size());
    QCOMPARE(targetFile.readAll(), sourceFile.readAll());
}

void tst_QNetworkReply::receiveCookiesFromHttp_data()
{
    QTest::addColumn<QString>("cookieString");
    QTest::addColumn<QList<QNetworkCookie> >("expectedCookiesFromHttp");
    QTest::addColumn<QList<QNetworkCookie> >("expectedCookiesInJar");

    QTest::newRow("empty") << "" << QList<QNetworkCookie>() << QList<QNetworkCookie>();

    QList<QNetworkCookie> header, jar;
    QNetworkCookie cookie("a", "b");
    header << cookie;
    cookie.setDomain(QtNetworkSettings::serverName());
    cookie.setPath("/qtest/cgi-bin/");
    jar << cookie;
    QTest::newRow("simple-cookie") << "a=b" << header << jar;

    header << QNetworkCookie("c", "d");
    cookie.setName("c");
    cookie.setValue("d");
    jar << cookie;
    QTest::newRow("two-cookies") << "a=b, c=d" << header << jar;
    QTest::newRow("two-cookies-2") << "a=b\nc=d" << header << jar;

    header.clear();
    jar.clear();
    cookie = QNetworkCookie("a", "b");
    cookie.setPath("/not/part-of-path");
    header << cookie;
    cookie.setDomain(QtNetworkSettings::serverName());
    jar << cookie;
    QTest::newRow("invalid-cookie-path") << "a=b; path=/not/part-of-path" << header << jar;

    jar.clear();
    cookie = QNetworkCookie("a", "b");
    cookie.setDomain(".example.com");
    header.clear();
    header << cookie;
    QTest::newRow("invalid-cookie-domain") << "a=b; domain=.example.com" << header << jar;
}

void tst_QNetworkReply::receiveCookiesFromHttp()
{
    QFETCH(QString, cookieString);

    QByteArray data = cookieString.toLatin1() + '\n';
    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/set-cookie.cgi");
    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    QNetworkReplyPtr reply;
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PostOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QList<QNetworkCookie> setCookies =
        qvariant_cast<QList<QNetworkCookie> >(reply->header(QNetworkRequest::SetCookieHeader));
    QTEST(setCookies, "expectedCookiesFromHttp");
    QTEST(cookieJar->allCookies(), "expectedCookiesInJar");
}

void tst_QNetworkReply::receiveCookiesFromHttpSynchronous_data()
{
    tst_QNetworkReply::receiveCookiesFromHttp_data();
}

void tst_QNetworkReply::receiveCookiesFromHttpSynchronous()
{
    QFETCH(QString, cookieString);

    QByteArray data = cookieString.toLatin1() + '\n';
    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/set-cookie.cgi");

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/octet-stream");
    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QNetworkReplyPtr reply;
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::PostOperation, request, reply, data));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QList<QNetworkCookie> setCookies =
            qvariant_cast<QList<QNetworkCookie> >(reply->header(QNetworkRequest::SetCookieHeader));
    QTEST(setCookies, "expectedCookiesFromHttp");
    QTEST(cookieJar->allCookies(), "expectedCookiesInJar");
}

void tst_QNetworkReply::sendCookies_data()
{
    QTest::addColumn<QList<QNetworkCookie> >("cookiesToSet");
    QTest::addColumn<QString>("expectedCookieString");

    QList<QNetworkCookie> list;
    QTest::newRow("empty") << list << "";

    QNetworkCookie cookie("a", "b");
    cookie.setPath("/");
    cookie.setDomain("example.com");
    list << cookie;
    QTest::newRow("no-match-domain") << list << "";

    cookie.setDomain(QtNetworkSettings::serverName());
    cookie.setPath("/something/else");
    list << cookie;
    QTest::newRow("no-match-path") << list << "";

    cookie.setPath("/");
    list << cookie;
    QTest::newRow("simple-cookie") << list << "a=b";

    cookie.setPath("/qtest");
    cookie.setValue("longer");
    list << cookie;
    QTest::newRow("two-cookies") << list << "a=longer; a=b";

    list.clear();
    cookie = QNetworkCookie("a", "b");
    cookie.setPath("/");
    cookie.setDomain("." + QtNetworkSettings::serverDomainName());
    list << cookie;
    QTest::newRow("domain-match") << list << "a=b";

    // but it shouldn't match this:
    cookie.setDomain(QtNetworkSettings::serverDomainName());
    list << cookie;
    QTest::newRow("domain-match-2") << list << "a=b";
}

void tst_QNetworkReply::sendCookies()
{
    QFETCH(QString, expectedCookieString);
    QFETCH(QList<QNetworkCookie>, cookiesToSet);
    cookieJar->setAllCookies(cookiesToSet);

    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/get-cookie.cgi");
    QNetworkRequest request(url);
    QNetworkReplyPtr reply;
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QCOMPARE(QString::fromLatin1(reply->readAll()).trimmed(), expectedCookieString);
}

void tst_QNetworkReply::sendCookiesSynchronous_data()
{
    tst_QNetworkReply::sendCookies_data();
}

void tst_QNetworkReply::sendCookiesSynchronous()
{
    QFETCH(QString, expectedCookieString);
    QFETCH(QList<QNetworkCookie>, cookiesToSet);
    cookieJar->setAllCookies(cookiesToSet);

    QUrl url("http://" + QtNetworkSettings::serverName() + "/qtest/cgi-bin/get-cookie.cgi");
    QNetworkRequest request(url);

    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QNetworkReplyPtr reply;
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200); // 200 Ok

    QCOMPARE(QString::fromLatin1(reply->readAll()).trimmed(), expectedCookieString);
}

void tst_QNetworkReply::nestedEventLoops_slot()
{
    QEventLoop subloop;

    // 16 seconds: fluke times out in 15 seconds, which triggers a QTcpSocket error
    QTimer::singleShot(16000, &subloop, SLOT(quit()));
    subloop.exec();

    QTestEventLoop::instance().exitLoop();
}

void tst_QNetworkReply::nestedEventLoops()
{
    // Slightly fragile test, it may not be testing anything
    // This is certifying that we're not running into the same issue
    // that QHttp had (task 200432): the QTcpSocket connection is
    // closed by the remote end because of the kept-alive HTTP
    // connection timed out.
    //
    // The exact time required for this to happen is not exactly
    // defined. Our server (Apache httpd) times out after 15
    // seconds. (see above)

    qDebug("Takes 16 seconds to run, please wait");
    qRegisterMetaType<QNetworkReply::NetworkError>();

    QUrl url("http://" + QtNetworkSettings::serverName());
    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.get(request);

    QSignalSpy finishedspy(reply, SIGNAL(finished()));
    QSignalSpy errorspy(reply, SIGNAL(error(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(finished()), SLOT(nestedEventLoops_slot()));
    QTestEventLoop::instance().enterLoop(20);
    QVERIFY2(!QTestEventLoop::instance().timeout(), "Network timeout");

    QCOMPARE(finishedspy.count(), 1);
    QCOMPARE(errorspy.count(), 0);
}

void tst_QNetworkReply::httpProxyCommands_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QByteArray>("responseToSend");
    QTest::addColumn<QString>("expectedCommand");

    QTest::newRow("http")
        << QUrl("http://0.0.0.0:4443/http-request")
        << QByteArray("HTTP/1.0 200 OK\r\nProxy-Connection: close\r\nContent-Length: 1\r\n\r\n1")
        << "GET http://0.0.0.0:4443/http-request HTTP/1.";
#ifndef QT_NO_OPENSSL
    QTest::newRow("https")
        << QUrl("https://0.0.0.0:4443/https-request")
        << QByteArray("HTTP/1.0 200 Connection Established\r\n\r\n")
        << "CONNECT 0.0.0.0:4443 HTTP/1.";
#endif
}

void tst_QNetworkReply::httpProxyCommands()
{
    QFETCH(QUrl, url);
    QFETCH(QByteArray, responseToSend);
    QFETCH(QString, expectedCommand);

    MiniHttpServer proxyServer(responseToSend);
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, "127.0.0.1", proxyServer.serverPort());

    manager.setProxy(proxy);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "QNetworkReplyAutoTest/1.0");
    QNetworkReplyPtr reply = manager.get(request);
    //clearing the proxy here causes the test to fail.
    //the proxy isn't used until after the bearer has been started
    //which is correct in general, because system proxy isn't known until that time.
    //removing this line is safe, as the proxy is also reset by the cleanup() function
    //manager.setProxy(QNetworkProxy());

    // wait for the finished signal
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));

    QTestEventLoop::instance().enterLoop(15);

    QVERIFY(!QTestEventLoop::instance().timeout());

    //qDebug() << reply->error() << reply->errorString();
    //qDebug() << proxyServer.receivedData;

    // we don't really care if the request succeeded
    // especially since it won't succeed in the HTTPS case
    // so just check that the command was correct

    QString receivedHeader = proxyServer.receivedData.left(expectedCommand.length());
    QCOMPARE(receivedHeader, expectedCommand);

    //QTBUG-17223 - make sure the user agent from the request is sent to proxy server even for CONNECT
    int uapos = proxyServer.receivedData.indexOf("User-Agent");
    int uaend = proxyServer.receivedData.indexOf("\r\n", uapos);
    QByteArray uaheader = proxyServer.receivedData.mid(uapos, uaend - uapos);
    QCOMPARE(uaheader, QByteArray("User-Agent: QNetworkReplyAutoTest/1.0"));
}

class ProxyChangeHelper : public QObject {
    Q_OBJECT
public:
    ProxyChangeHelper() : QObject(), signalCount(0) {};
public slots:
    void finishedSlot() {
        signalCount++;
        if (signalCount == 2)
            QMetaObject::invokeMethod(&QTestEventLoop::instance(), "exitLoop", Qt::QueuedConnection);
    }
private:
   int signalCount;
};

void tst_QNetworkReply::httpProxyCommandsSynchronous_data()
{
    httpProxyCommands_data();
}

struct QThreadCleanup
{
    static inline void cleanup(QThread *thread)
    {
        thread->quit();
        if (thread->wait(3000))
            delete thread;
        else
            qWarning("thread hung, leaking memory so test can finish");
    }
};

struct QDeleteLaterCleanup
{
    static inline void cleanup(QObject *o)
    {
        o->deleteLater();
    }
};

void tst_QNetworkReply::httpProxyCommandsSynchronous()
{
    QFETCH(QUrl, url);
    QFETCH(QByteArray, responseToSend);
    QFETCH(QString, expectedCommand);

    // when using synchronous commands, we need a different event loop for
    // the server thread, because the client is never returning to the
    // event loop
    QScopedPointer<QThread, QThreadCleanup> serverThread(new QThread);
    QScopedPointer<MiniHttpServer, QDeleteLaterCleanup> proxyServer(new MiniHttpServer(responseToSend, false, serverThread.data()));
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, "127.0.0.1", proxyServer->serverPort());

    manager.setProxy(proxy);
    QNetworkRequest request(url);

    // send synchronous request
    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QNetworkReplyPtr reply = manager.get(request);
    QVERIFY(reply->isFinished()); // synchronous
    manager.setProxy(QNetworkProxy());

    //qDebug() << reply->error() << reply->errorString();

    // we don't really care if the request succeeded
    // especially since it won't succeed in the HTTPS case
    // so just check that the command was correct

    QString receivedHeader = proxyServer->receivedData.left(expectedCommand.length());
    QCOMPARE(receivedHeader, expectedCommand);
}

void tst_QNetworkReply::proxyChange()
{
    ProxyChangeHelper helper;
    MiniHttpServer proxyServer(
        "HTTP/1.0 200 OK\r\nProxy-Connection: keep-alive\r\n"
        "Content-Length: 1\r\n\r\n1");
    QNetworkProxy dummyProxy(QNetworkProxy::HttpProxy, "127.0.0.1", proxyServer.serverPort());
    QNetworkRequest req(QUrl("http://" + QtNetworkSettings::serverName()));
    proxyServer.doClose = false;

    manager.setProxy(dummyProxy);
    QNetworkReplyPtr reply1 = manager.get(req);
    connect(reply1, SIGNAL(finished()), &helper, SLOT(finishedSlot()));

    manager.setProxy(QNetworkProxy());
    QNetworkReplyPtr reply2 = manager.get(req);
    connect(reply2, SIGNAL(finished()), &helper, SLOT(finishedSlot()));

    QTestEventLoop::instance().enterLoop(20);
    QVERIFY(!QTestEventLoop::instance().timeout());

    // verify that the replies succeeded
    QCOMPARE(reply1->error(), QNetworkReply::NoError);
    QCOMPARE(reply1->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QVERIFY(reply1->size() == 1);

    QCOMPARE(reply2->error(), QNetworkReply::NoError);
    QCOMPARE(reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QVERIFY(reply2->size() > 1);

    // now try again and get an error
    // this verifies that we reuse the already-open connection

    proxyServer.doClose = true;
    proxyServer.dataToTransmit =
        "HTTP/1.0 403 Forbidden\r\nProxy-Connection: close\r\n"
        "Content-Length: 1\r\n\r\n1";

    manager.setProxy(dummyProxy);
    QNetworkReplyPtr reply3 = manager.get(req);
    connect(reply3, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(int(reply3->error()) > 0);
}

void tst_QNetworkReply::authorizationError_data()
{

    QTest::addColumn<QString>("url");
    QTest::addColumn<int>("errorSignalCount");
    QTest::addColumn<int>("finishedSignalCount");
    QTest::addColumn<int>("error");
    QTest::addColumn<int>("httpStatusCode");
    QTest::addColumn<QString>("httpBody");

    QTest::newRow("unknown-authorization-method") << "http://" + QtNetworkSettings::serverName() +
                                                     "/qtest/cgi-bin/http-unknown-authentication-method.cgi?401-authorization-required" << 1 << 1
                                                  << int(QNetworkReply::AuthenticationRequiredError) << 401 << "authorization required";
    QTest::newRow("unknown-proxy-authorization-method") << "http://" + QtNetworkSettings::serverName() +
                                                           "/qtest/cgi-bin/http-unknown-authentication-method.cgi?407-proxy-authorization-required" << 1 << 1
                                                        << int(QNetworkReply::ProxyAuthenticationRequiredError) << 407
                                                        << "authorization required";
}

void tst_QNetworkReply::authorizationError()
{
    QFETCH(QString, url);
    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.get(request);

    QCOMPARE(reply->error(), QNetworkReply::NoError);

    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
    QSignalSpy errorSpy(reply, SIGNAL(error(QNetworkReply::NetworkError)));
    QSignalSpy finishedSpy(reply, SIGNAL(finished()));
    // now run the request:
    connect(reply, SIGNAL(finished()),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QFETCH(int, errorSignalCount);
    QCOMPARE(errorSpy.count(), errorSignalCount);
    QFETCH(int, finishedSignalCount);
    QCOMPARE(finishedSpy.count(), finishedSignalCount);
    QFETCH(int, error);
    QCOMPARE(reply->error(), QNetworkReply::NetworkError(error));

    QFETCH(int, httpStatusCode);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), httpStatusCode);

    QFETCH(QString, httpBody);
    QCOMPARE(qint64(reply->size()), qint64(httpBody.size()));
    QCOMPARE(QString(reply->readAll()), httpBody);
}

void tst_QNetworkReply::httpConnectionCount()
{
    QTcpServer server;
    QVERIFY(server.listen());
    QCoreApplication::instance()->processEvents();

    for (int i = 0; i < 10; i++) {
        QNetworkRequest request (QUrl("http://127.0.0.1:" + QString::number(server.serverPort()) + "/" +  QString::number(i)));
        QNetworkReply* reply = manager.get(request);
        reply->setParent(&server);
    }

    int pendingConnectionCount = 0;
    QTime time;
    time.start();

    while(pendingConnectionCount <= 20) {
        QTestEventLoop::instance().enterLoop(1);
        QTcpSocket *socket = server.nextPendingConnection();
        while (socket != 0) {
            pendingConnectionCount++;
            socket->setParent(&server);
            socket = server.nextPendingConnection();
        }

        // at max. wait 10 sec
        if (time.elapsed() > 10000)
            break;
    }

#ifdef Q_OS_SYMBIAN
    // see in qhttpnetworkconnection.cpp
    // hardcoded defaultChannelCount = 3
    QCOMPARE(pendingConnectionCount, 3);
#else
    QCOMPARE(pendingConnectionCount, 6);
#endif
}

void tst_QNetworkReply::httpReUsingConnectionSequential_data()
{
    QTest::addColumn<bool>("doDeleteLater");
    QTest::newRow("deleteLater") << true;
    QTest::newRow("noDeleteLater") << false;
}

void tst_QNetworkReply::httpReUsingConnectionSequential()
{
    QFETCH(bool, doDeleteLater);

    QByteArray response("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
    MiniHttpServer server(response);
    server.multiple = true;
    server.doClose = false;

    QUrl url;
    url.setScheme("http");
    url.setPort(server.serverPort());
    url.setHost("127.0.0.1");
    // first request
    QNetworkReply* reply1 = manager.get(QNetworkRequest(url));
    connect(reply1, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(!reply1->error());
    int reply1port = server.client->peerPort();

    if (doDeleteLater)
        reply1->deleteLater();

    // finished received, send the next one
    QNetworkReply*reply2 = manager.get(QNetworkRequest(url));
    connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(!reply2->error());
    int reply2port = server.client->peerPort(); // should still be the same object

    QVERIFY(reply1port > 0);
    QCOMPARE(server.totalConnections, 1);
    QCOMPARE(reply2port, reply1port);

    if (!doDeleteLater)
        reply1->deleteLater(); // only do it if it was not done earlier
    reply2->deleteLater();
}

class HttpReUsingConnectionFromFinishedSlot : public QObject {
    Q_OBJECT
public:
    QNetworkReply* reply1;
    QNetworkReply* reply2;
    QUrl url;
    QNetworkAccessManager manager;
public slots:
    void finishedSlot() {
        QVERIFY(!reply1->error());

        QFETCH(bool, doDeleteLater);
        if (doDeleteLater) {
            reply1->deleteLater();
            reply1 = 0;
        }

        // kick off 2nd request and exit the loop when it is done
        reply2 = manager.get(QNetworkRequest(url));
        reply2->setParent(this);
        connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    }
};

void tst_QNetworkReply::httpReUsingConnectionFromFinishedSlot_data()
{
    httpReUsingConnectionSequential_data();
}

void tst_QNetworkReply::httpReUsingConnectionFromFinishedSlot()
{
    QByteArray response("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
    MiniHttpServer server(response);
    server.multiple = true;
    server.doClose = false;

    HttpReUsingConnectionFromFinishedSlot helper;
    helper.reply1 = 0;
    helper.reply2 = 0;
    helper.url.setScheme("http");
    helper.url.setPort(server.serverPort());
    helper.url.setHost("127.0.0.1");

    // first request
    helper.reply1 = helper.manager.get(QNetworkRequest(helper.url));
    helper.reply1->setParent(&helper);
    connect(helper.reply1, SIGNAL(finished()), &helper, SLOT(finishedSlot()));
    QTestEventLoop::instance().enterLoop(4);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(helper.reply2);
    QVERIFY(!helper.reply2->error());

    QCOMPARE(server.totalConnections, 1);
}

class HttpRecursiveCreationHelper : public QObject {
    Q_OBJECT
public:

    HttpRecursiveCreationHelper():
            QObject(0),
            requestsStartedCount_finished(0),
            requestsStartedCount_readyRead(0),
            requestsFinishedCount(0)
    {
    }
    QNetworkAccessManager manager;
    int requestsStartedCount_finished;
    int requestsStartedCount_readyRead;
    int requestsFinishedCount;
public slots:
    void finishedSlot() {
        requestsFinishedCount++;

        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        QVERIFY(!reply->error());
        QVERIFY(reply->bytesAvailable() == 27906);

        if (requestsFinishedCount == 60) {
            QTestEventLoop::instance().exitLoop();
            return;
        }

        if (requestsStartedCount_finished < 30) {
            startOne();
            requestsStartedCount_finished++;
        }

        reply->deleteLater();
    }
    void readyReadSlot() {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        QVERIFY(!reply->error());

        if (requestsStartedCount_readyRead < 30 && reply->bytesAvailable() > 27906/2) {
            startOne();
            requestsStartedCount_readyRead++;
        }
    }
    void startOne() {
        QUrl url = "http://" + QtNetworkSettings::serverName() + "/qtest/fluke.gif";
        QNetworkRequest request(url);
        QNetworkReply *reply = manager.get(request);
        reply->setParent(this);
        connect(reply, SIGNAL(finished()), this, SLOT(finishedSlot()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
    }
};

void tst_QNetworkReply::httpRecursiveCreation()
{
    // this test checks if creation of new requests to the same host properly works
    // from readyRead() and finished() signals
    HttpRecursiveCreationHelper helper;
    helper.startOne();
    QTestEventLoop::instance().enterLoop(30);
    QVERIFY(!QTestEventLoop::instance().timeout());
}

#ifndef QT_NO_OPENSSL
void tst_QNetworkReply::ignoreSslErrorsList_data()
{
    QTest::addColumn<QString>("url");
    QTest::addColumn<QList<QSslError> >("expectedSslErrors");
    QTest::addColumn<QNetworkReply::NetworkError>("expectedNetworkError");

    QList<QSslError> expectedSslErrors;
    // apparently, because of some weird behaviour of SRCDIR, the file name below needs to start with a slash
    QList<QSslCertificate> certs = QSslCertificate::fromPath(QLatin1String(SRCDIR "/certs/qt-test-server-cacert.pem"));
    QSslError rightError(QSslError::SelfSignedCertificate, certs.at(0));
    QSslError wrongError(QSslError::SelfSignedCertificate);

    QTest::newRow("SSL-failure-empty-list") << "https://" + QtNetworkSettings::serverName() + "/index.html" << expectedSslErrors << QNetworkReply::SslHandshakeFailedError;
    expectedSslErrors.append(wrongError);
    QTest::newRow("SSL-failure-wrong-error") << "https://" + QtNetworkSettings::serverName() + "/index.html" << expectedSslErrors << QNetworkReply::SslHandshakeFailedError;
    expectedSslErrors.append(rightError);
    QTest::newRow("allErrorsInExpectedList1") << "https://" + QtNetworkSettings::serverName() + "/index.html" << expectedSslErrors << QNetworkReply::NoError;
    expectedSslErrors.removeAll(wrongError);
    QTest::newRow("allErrorsInExpectedList2") << "https://" + QtNetworkSettings::serverName() + "/index.html" << expectedSslErrors << QNetworkReply::NoError;
    expectedSslErrors.removeAll(rightError);
    QTest::newRow("SSL-failure-empty-list-again") << "https://" + QtNetworkSettings::serverName() + "/index.html" << expectedSslErrors << QNetworkReply::SslHandshakeFailedError;
}

void tst_QNetworkReply::ignoreSslErrorsList()
{
    QFETCH(QString, url);
    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.get(request);

    QFETCH(QList<QSslError>, expectedSslErrors);
    reply->ignoreSslErrors(expectedSslErrors);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QFETCH(QNetworkReply::NetworkError, expectedNetworkError);
    QCOMPARE(reply->error(), expectedNetworkError);
}

void tst_QNetworkReply::ignoreSslErrorsListWithSlot_data()
{
    ignoreSslErrorsList_data();
}

// this is not a test, just a slot called in the test below
void tst_QNetworkReply::ignoreSslErrorListSlot(QNetworkReply *reply, const QList<QSslError> &)
{
    reply->ignoreSslErrors(storedExpectedSslErrors);
}

// do the same as in ignoreSslErrorsList, but ignore the errors in the slot
void tst_QNetworkReply::ignoreSslErrorsListWithSlot()
{
    QFETCH(QString, url);
    QNetworkRequest request(url);
    QNetworkReplyPtr reply = manager.get(request);

    QFETCH(QList<QSslError>, expectedSslErrors);
    // store the errors to ignore them later in the slot connected below
    storedExpectedSslErrors = expectedSslErrors;
    connect(&manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
            this, SLOT(ignoreSslErrorListSlot(QNetworkReply *, const QList<QSslError> &)));


    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QFETCH(QNetworkReply::NetworkError, expectedNetworkError);
    QCOMPARE(reply->error(), expectedNetworkError);
}

void tst_QNetworkReply::sslConfiguration_data()
{
    QTest::addColumn<QSslConfiguration>("configuration");
    QTest::addColumn<bool>("works");

    QTest::newRow("empty") << QSslConfiguration() << false;
    QSslConfiguration conf = QSslConfiguration::defaultConfiguration();
    QTest::newRow("default") << conf << false; // does not contain test server cert
    QList<QSslCertificate> testServerCert = QSslCertificate::fromPath(SRCDIR "/certs/qt-test-server-cacert.pem");
    conf.setCaCertificates(testServerCert);
    QTest::newRow("set-root-cert") << conf << true;
    conf.setProtocol(QSsl::SecureProtocols);
    QTest::newRow("secure") << conf << true;
}

void tst_QNetworkReply::sslConfiguration()
{
    QNetworkRequest request(QUrl("https://" + QtNetworkSettings::serverName() + "/index.html"));
    QFETCH(QSslConfiguration, configuration);
    request.setSslConfiguration(configuration);
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QFETCH(bool, works);
    QNetworkReply::NetworkError expectedError = works ? QNetworkReply::NoError : QNetworkReply::SslHandshakeFailedError;
    QCOMPARE(reply->error(), expectedError);
}

#endif // QT_NO_OPENSSL

void tst_QNetworkReply::getAndThenDeleteObject_data()
{
    QTest::addColumn<bool>("replyFirst");

    QTest::newRow("delete-reply-first") << true;
    QTest::newRow("delete-qnam-first") << false;
}

void tst_QNetworkReply::getAndThenDeleteObject()
{
    // yes, this will leak if the testcase fails. I don't care. It must not fail then :P
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request("http://" + QtNetworkSettings::serverName() + "/qtest/bigfile");
    QNetworkReply *reply = manager->get(request);
    reply->setReadBufferSize(1);
    reply->setParent((QObject*)0); // must be 0 because else it is the manager

    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }

    QVERIFY(reply->bytesAvailable());
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QVERIFY(!reply->isFinished()); // must not be finished

    QFETCH(bool, replyFirst);

    if (replyFirst) {
        delete reply;
        delete manager;
    } else {
        delete manager;
        delete reply;
    }
}

// see https://bugs.webkit.org/show_bug.cgi?id=38935
void tst_QNetworkReply::symbianOpenCDataUrlCrash()
{
    QString requestUrl("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAWCAYAAAA1vze2AAAAB3RJTUUH2AUSEgolrgBvVQAAAAlwSFlzAAALEwAACxMBAJqcGAAAAARnQU1BAACxjwv8YQUAAAHlSURBVHja5VbNShxBEK6ZaXtnHTebQPA1gngNmfaeq+QNPIlIXkC9iQdJxJNvEHLN3VkxhxxE8gTmEhAVddXZ6Z3f9Ndriz89/sHmkBQUVVT1fB9d9c3uOERUKTunIdn3HzstxGpYBDS4wZk7TAJj/wlJ90J+jnuygqs8svSj+/rGHBos3rE18XBvfU3no7NzlJfUaY/5whAwl8Lr/WDUv4ODxTMb+P5xLExe5LmO559WqTX/MQR4WZYEAtSePS4pE0qSnuhnRUcBU5Gm2k9XljU4Z26I3NRxBrd80rj2fh+KNE0FY4xevRgTjREvPFpasAK8Xli6MUbbuKw3afAGgSBXozo5u4hkmncAlkl5wx8iMGbdyQjnCFEiEwGiosj1UQA/x2rVddiVoi+l4IxE0PTDnx+mrQBvvnx9cFz3krhVvuhzFn579/aq/n5rW8fbtTqiWhIQZEo17YBvbkxOXNVndnYpTvod7AtiuN2re0+siwcB9oH8VxxrNwQQAhzyRs30n7wTI2HIN2g2QtQwjjhJIQatOq7E8bIVCLwzpl83Lvtvl+NohWWlE8UZTWEMAGCcR77fHKhPnZF5tYie6dfdxCphACmLPM+j8bYfmTryg64kV9Vh3mV8jP0b/4wO/YUPiT/8i0MLf55lSQAAAABJRU5ErkJggg==");
    QUrl url = QUrl::fromEncoded(requestUrl.toLatin1());
    QNetworkRequest req(url);
    QNetworkReplyPtr reply;

    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, req, reply));

    QCOMPARE(reply->url(), url);
    QCOMPARE(reply->error(), QNetworkReply::NoError);

    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), qint64(598));
}

void tst_QNetworkReply::getFromHttpIntoBuffer_data()
{
    QTest::addColumn<QUrl>("url");

    QTest::newRow("rfc-internal") << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt");
}

// Please note that the whole "zero copy" download buffer API is private right now. Do not use it.
void tst_QNetworkReply::getFromHttpIntoBuffer()
{
    QFETCH(QUrl, url);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::MaximumDownloadBufferSizeAttribute, 1024*128); // 128 kB

    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));

    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(reply->isFinished());

    QFile reference(SRCDIR "/rfc3252.txt");
    QVERIFY(reference.open(QIODevice::ReadOnly));

    QCOMPARE(reference.bytesAvailable(), reply->bytesAvailable());
    QCOMPARE(reference.size(), reply->size());

    // Compare the memory buffer
    QVariant downloadBufferAttribute = reply->attribute(QNetworkRequest::DownloadBufferAttribute);
    QVERIFY(downloadBufferAttribute.isValid());
    QSharedPointer<char> sharedPointer = downloadBufferAttribute.value<QSharedPointer<char> >();
    bool memoryComparison =
            (0 == memcmp(static_cast<void*>(reference.readAll().data()),
                         sharedPointer.data(), reference.size()));
    QVERIFY(memoryComparison);

    // Make sure the normal reading works
    reference.seek(0);
    QCOMPARE(reply->read(42), reference.read(42));
    QCOMPARE(reply->getChar(0), reference.getChar(0));
    QCOMPARE(reply->peek(23), reference.peek(23));
    QCOMPARE(reply->readLine(), reference.readLine());
    QCOMPARE(reference.bytesAvailable(), reply->bytesAvailable());
    QCOMPARE(reply->readAll(), reference.readAll());
    QVERIFY(reply->atEnd());
}

// FIXME we really need to consolidate all those server implementations
class GetFromHttpIntoBuffer2Server : QObject {
    Q_OBJECT
    qint64 dataSize;
    qint64 dataSent;
    QTcpServer server;
    QTcpSocket *client;
    bool serverSendsContentLength;
    bool chunkedEncoding;

public:
    GetFromHttpIntoBuffer2Server (qint64 ds, bool sscl, bool ce) : dataSize(ds), dataSent(0),
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

class GetFromHttpIntoBuffer2Client : QObject {
    Q_OBJECT
private:
    bool useDownloadBuffer;
    QNetworkReply *reply;
    qint64 uploadSize;
    QList<qint64> bytesAvailableList;
public:
    GetFromHttpIntoBuffer2Client (QNetworkReply *reply, bool useDownloadBuffer, qint64 uploadSize)
        : useDownloadBuffer(useDownloadBuffer), reply(reply), uploadSize(uploadSize)
    {
        connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChangedSlot()));
        connect(reply, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
        connect(reply, SIGNAL(finished()), this, SLOT(finishedSlot()));
    }

    public slots:
    void metaDataChangedSlot() {
        if (useDownloadBuffer) {
            QSharedPointer<char> sharedPointer = qvariant_cast<QSharedPointer<char> >(reply->attribute(QNetworkRequest::DownloadBufferAttribute));
            QVERIFY(!sharedPointer.isNull()); // It will be 0 if it failed
        }

        // metaDataChanged needs to come before everything else
        QVERIFY(bytesAvailableList.isEmpty());
    }

    void readyReadSlot() {
        QVERIFY(!reply->isFinished());

        qint64 bytesAvailable = reply->bytesAvailable();

        // bytesAvailable must never be 0
        QVERIFY(bytesAvailable != 0);

        if (bytesAvailableList.length() < 5) {
            // We assume that the first few times the bytes available must be less than the complete size, e.g.
            // the bytesAvailable() function works correctly in case of a downloadBuffer.
            QVERIFY(bytesAvailable < uploadSize);
        }
        if (!bytesAvailableList.isEmpty()) {
            // Also check that the same bytesAvailable is not coming twice in a row
            QVERIFY(bytesAvailableList.last() != bytesAvailable);
        }

        bytesAvailableList.append(bytesAvailable);
        // Add bytesAvailable to a list an parse
    }

    void finishedSlot() {
        // We should have already received all readyRead
        QVERIFY(!bytesAvailableList.isEmpty());
        QVERIFY(bytesAvailableList.last() == uploadSize);
    }
};

void tst_QNetworkReply::getFromHttpIntoBuffer2_data()
{
    QTest::addColumn<bool>("useDownloadBuffer");

    QTest::newRow("use-download-buffer") << true;
    QTest::newRow("do-not-use-download-buffer") << false;
}

// This test checks mostly that signal emissions are in correct order
// Please note that the whole "zero copy" download buffer API is private right now. Do not use it.
void tst_QNetworkReply::getFromHttpIntoBuffer2()
{
    QFETCH(bool, useDownloadBuffer);

    // On my Linux Desktop the results are already visible with 128 kB, however we use this to have good results.
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_WINCE_WM)
    // Show some mercy to non-desktop platform/s
    enum {UploadSize = 4*1024*1024}; // 4 MB
#else
    enum {UploadSize = 32*1024*1024}; // 32 MB
#endif

    GetFromHttpIntoBuffer2Server server(UploadSize, true, false);

    QNetworkRequest request(QUrl("http://127.0.0.1:" + QString::number(server.serverPort()) + "/?bare=1"));
    if (useDownloadBuffer)
        request.setAttribute(QNetworkRequest::MaximumDownloadBufferSizeAttribute, 1024*1024*128); // 128 MB is max allowed

    QNetworkAccessManager manager;
    QNetworkReplyPtr reply = manager.get(request);

    GetFromHttpIntoBuffer2Client client(reply, useDownloadBuffer, UploadSize);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
    QTestEventLoop::instance().enterLoop(40);
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QVERIFY(!QTestEventLoop::instance().timeout());
}


// Is handled somewhere else too, introduced this special test to have it more accessible
void tst_QNetworkReply::ioGetFromHttpWithoutContentLength()
{
    QByteArray dataToSend("HTTP/1.0 200 OK\r\n\r\nHALLO! 123!");
    MiniHttpServer server(dataToSend);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->url(), request.url());
    QVERIFY(reply->isFinished());
    QVERIFY(reply->error() == QNetworkReply::NoError);
}

// Is handled somewhere else too, introduced this special test to have it more accessible
void tst_QNetworkReply::ioGetFromHttpBrokenChunkedEncoding()
{
    // This is wrong chunked encoding because of the X. What actually has to follow is \r\n
    // and then the declaration of the final 0 chunk
    QByteArray dataToSend("HTTP/1.0 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n3\r\nABCX");
    MiniHttpServer server(dataToSend);
    server.doClose = false; // FIXME

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);

    QEXPECT_FAIL(0, "We should close the socket and not just do nothing", Continue);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QEXPECT_FAIL(0, "We should close the socket and not just do nothing", Continue);
    QVERIFY(reply->isFinished());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
}

// TODO:
// Prepare a gzip that has one chunk that expands to the size mentioned in the bugreport.
// Then have a custom HTTP server that waits after this chunk so the returning gets
// triggered.
void tst_QNetworkReply::qtbug12908compressedHttpReply()
{
    QString header("HTTP/1.0 200 OK\r\nContent-Encoding: gzip\r\nContent-Length: 63\r\n\r\n");

    // dd if=/dev/zero of=qtbug-12908 bs=16384  count=1 && gzip qtbug-12908 && base64 -w 0 qtbug-12908.gz
    QString encodedFile("H4sICDdDaUwAA3F0YnVnLTEyOTA4AO3BMQEAAADCoPVPbQwfoAAAAAAAAAAAAAAAAAAAAIC3AYbSVKsAQAAA");
    QByteArray decodedFile = QByteArray::fromBase64(encodedFile.toAscii());
    QCOMPARE(decodedFile.size(), 63);

    MiniHttpServer server(header.toAscii() + decodedFile);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->size(), qint64(16384));
    QCOMPARE(reply->readAll(), QByteArray(16384, '\0'));
}

void tst_QNetworkReply::compressedHttpReplyBrokenGzip()
{
    QString header("HTTP/1.0 200 OK\r\nContent-Encoding: gzip\r\nContent-Length: 63\r\n\r\n");

    // dd if=/dev/zero of=qtbug-12908 bs=16384  count=1 && gzip qtbug-12908 && base64 -w 0 qtbug-12908.gz
    // Then change "BMQ" to "BMX"
    QString encodedFile("H4sICDdDaUwAA3F0YnVnLTEyOTA4AO3BMXEAAADCoPVPbQwfoAAAAAAAAAAAAAAAAAAAAIC3AYbSVKsAQAAA");
    QByteArray decodedFile = QByteArray::fromBase64(encodedFile.toAscii());
    QCOMPARE(decodedFile.size(), 63);

    MiniHttpServer server(header.toAscii() + decodedFile);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->error(), QNetworkReply::ProtocolFailure);
}

// TODO add similar test for FTP
void tst_QNetworkReply::getFromUnreachableIp()
{
    QNetworkAccessManager manager;

    QNetworkRequest request(QUrl("http://255.255.255.255/42/23/narf/narf/narf"));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(reply->error() != QNetworkReply::NoError);
}

void tst_QNetworkReply::qtbug4121unknownAuthentication()
{
    MiniHttpServer server(QByteArray("HTTP/1.1 401 bla\r\nWWW-Authenticate: crap\r\nContent-Length: 0\r\n\r\n"));
    server.doClose = false;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkAccessManager manager;
    QNetworkReplyPtr reply = manager.get(request);

    qRegisterMetaType<QNetworkReply*>("QNetworkReply*");
    qRegisterMetaType<QAuthenticator*>("QAuthenticator*");
    QSignalSpy authSpy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QSignalSpy finishedSpy(&manager, SIGNAL(finished(QNetworkReply*)));
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
    QSignalSpy errorSpy(reply, SIGNAL(error(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(authSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);

    QCOMPARE(reply->error(), QNetworkReply::AuthenticationRequiredError);
}

class QtBug13431Helper : public QObject {
    Q_OBJECT
public:
    QNetworkReply* m_reply;
    QTimer m_dlTimer;
public slots:
    void replyFinished(QNetworkReply*) {
        QTestEventLoop::instance().exitLoop();
    }

    void onReadAndReschedule() {
        const qint64 bytesReceived = m_reply->bytesAvailable();
        if (bytesReceived) {
           QByteArray data = m_reply->read(bytesReceived);
           // reschedule read
           const int millisecDelay = static_cast<int>(bytesReceived * 1000 / m_reply->readBufferSize());
           m_dlTimer.start(millisecDelay);
        }
        else {
           // reschedule read
           m_dlTimer.start(200);
        }
    }
};

void tst_QNetworkReply::qtbug13431replyThrottling()
{
    QtBug13431Helper helper;

    QNetworkAccessManager nam;
    connect(&nam, SIGNAL(finished(QNetworkReply*)), &helper, SLOT(replyFinished(QNetworkReply*)));

    // Download a bigger file
    QNetworkRequest netRequest(QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/bigfile"));
    helper.m_reply = nam.get(netRequest);
    // Set the throttle
    helper.m_reply->setReadBufferSize(36000);

    // Schedule a timer that tries to read

    connect(&helper.m_dlTimer, SIGNAL(timeout()), &helper, SLOT(onReadAndReschedule()));
    helper.m_dlTimer.setSingleShot(true);
    helper.m_dlTimer.start(0);

    QTestEventLoop::instance().enterLoop(30);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(helper.m_reply->isFinished());
    QCOMPARE(helper.m_reply->error(), QNetworkReply::NoError);
}

void tst_QNetworkReply::httpWithNoCredentialUsage()
{
    QNetworkRequest request(QUrl("http://httptest:httptest@" + QtNetworkSettings::serverName() + "/qtest/protected/cgi-bin/md5sum.cgi"));
    // Do not use credentials
    request.setAttribute(QNetworkRequest::AuthenticationReuseAttribute, QNetworkRequest::Manual);
    QNetworkAccessManager manager;
    QNetworkReplyPtr reply = manager.get(request);

    qRegisterMetaType<QNetworkReply*>("QNetworkReply*");
    qRegisterMetaType<QAuthenticator*>("QAuthenticator*");
    QSignalSpy authSpy(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QSignalSpy finishedSpy(&manager, SIGNAL(finished(QNetworkReply*)));
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
    QSignalSpy errorSpy(reply, SIGNAL(error(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    // We check if authenticationRequired was emitted, however we do not anything in it so it should be 401
    QCOMPARE(authSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);

    QCOMPARE(reply->error(), QNetworkReply::AuthenticationRequiredError);
}

void tst_QNetworkReply::qtbug15311doubleContentLength()
{
    QByteArray response("HTTP/1.0 200 OK\r\nContent-Length: 3\r\nServer: bogus\r\nContent-Length: 3\r\n\r\nABC");
    MiniHttpServer server(response);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(reply->isFinished());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->size(), qint64(3));
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), qint64(3));
    QCOMPARE(reply->rawHeader("Content-length"), QByteArray("3, 3"));
    QCOMPARE(reply->readAll(), QByteArray("ABC"));
}

void tst_QNetworkReply::qtbug18232gzipContentLengthZero()
{
    QByteArray response("HTTP/1.0 200 OK\r\nContent-Encoding: gzip\r\nContent-Length: 0\r\n\r\n");
    MiniHttpServer server(response);
    server.doClose = true;

    QNetworkRequest request(QUrl("http://localhost:" + QString::number(server.serverPort())));
    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QVERIFY(reply->isFinished());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->size(), qint64(0));
    QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), qint64(0));
    QCOMPARE(reply->readAll(), QByteArray());
}

void tst_QNetworkReply::synchronousRequest_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<bool>("checkContentLength");
    QTest::addColumn<QString>("mimeType");

    // ### cache, auth, proxies

    QTest::newRow("http")
        << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt")
        << QString("file:" SRCDIR "/rfc3252.txt")
        << true
        << QString("text/plain");

    QTest::newRow("http-gzip")
        << QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/deflate/rfc3252.txt")
        << QString("file:" SRCDIR "/rfc3252.txt")
        << false // don't check content length, because it's gzip encoded
        //  ### we would need to enflate (un-deflate) the file content and compare the sizes
        << QString("text/plain");

#ifndef QT_NO_OPENSSL
    QTest::newRow("https")
        << QUrl("https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt")
        << QString("file:" SRCDIR "/rfc3252.txt")
        << true
        << QString("text/plain");
#endif

    QTest::newRow("data")
        << QUrl(QString::fromLatin1("data:text/plain,hello world"))
        << QString("data:hello world")
        << true // check content length
        << QString("text/plain");

    QTest::newRow("simple-file")
        << QUrl::fromLocalFile(SRCDIR "/rfc3252.txt")
        << QString("file:" SRCDIR "/rfc3252.txt")
        << true
        << QString();
}

// FIXME add testcase for failing network etc
void tst_QNetworkReply::synchronousRequest()
{
    QFETCH(QUrl, url);
    QFETCH(QString, expected);
    QFETCH(bool, checkContentLength);
    QFETCH(QString, mimeType);

    QNetworkRequest request(url);

#ifndef QT_NO_OPENSSL
    // workaround for HTTPS requests: add self-signed server cert to list of CA certs,
    // since we cannot react to the sslErrors() signal
    // to fix this properly we would need to have an ignoreSslErrors() method in the
    // QNetworkRequest, see http://bugreports.qt.nokia.com/browse/QTBUG-14774
    if (url.scheme() == "https") {
        QSslConfiguration sslConf;
        QList<QSslCertificate> certs = QSslCertificate::fromPath(SRCDIR "/certs/qt-test-server-cacert.pem");
        sslConf.setCaCertificates(certs);
        request.setSslConfiguration(sslConf);
    }
#endif

    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);

    QNetworkReplyPtr reply;
    QSignalSpy finishedSpy(&manager, SIGNAL(finished(QNetworkReply*)));
    QSignalSpy sslErrorsSpy(&manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)));
    RUN_REQUEST(runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply, 0));
    QVERIFY(reply->isFinished());
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(sslErrorsSpy.count(), 0);

    QCOMPARE(reply->header(QNetworkRequest::ContentTypeHeader).toString(), mimeType);

    QByteArray expectedContent;

    if (expected.startsWith("file:")) {
        QString path = expected.mid(5);
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        expectedContent = file.readAll();
    } else if (expected.startsWith("data:")) {
        expectedContent = expected.mid(5).toUtf8();
    }

    if (checkContentLength)
        QCOMPARE(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong(), qint64(expectedContent.size()));
    QCOMPARE(reply->readAll(), expectedContent);

    reply->deleteLater();
}

#ifndef QT_NO_OPENSSL
void tst_QNetworkReply::synchronousRequestSslFailure()
{
    // test that SSL won't be accepted with self-signed certificate,
    // and that we do not emit the sslError signal (in the manager that is,
    // in the reply we don't care)

    QUrl url("https://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt");
    QNetworkRequest request(url);
    request.setAttribute(
            QNetworkRequest::SynchronousRequestAttribute,
            true);
    QNetworkReplyPtr reply;
    QSignalSpy sslErrorsSpy(&manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)));
    runSimpleRequest(QNetworkAccessManager::GetOperation, request, reply, 0);
    QVERIFY(reply->isFinished());
    QCOMPARE(reply->error(), QNetworkReply::SslHandshakeFailedError);
    QCOMPARE(sslErrorsSpy.count(), 0);
}
#endif

class HttpAbortHelper : public QObject
{
    Q_OBJECT
public:
    HttpAbortHelper(QNetworkReply *parent)
    : QObject(parent)
    {
        mReply = parent;
        connect(parent, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }

    ~HttpAbortHelper()
    {
    }

public slots:
    void readyRead()
    {
        mReply->abort();
        QMetaObject::invokeMethod(&QTestEventLoop::instance(), "exitLoop", Qt::QueuedConnection);
    }

private:
    QNetworkReply *mReply;
};

void tst_QNetworkReply::httpAbort()
{
    // FIXME Also implement one where we do a big upload and then abort().
    // It must not crash either.

    // Abort after the first readyRead()
    QNetworkRequest request("http://" + QtNetworkSettings::serverName() + "/qtest/bigfile");
    QNetworkReplyPtr reply;
    reply = manager.get(request);
    HttpAbortHelper replyHolder(reply);
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QCOMPARE(reply->error(), QNetworkReply::OperationCanceledError);
    QVERIFY(reply->isFinished());

    // Abort immediately after the get()
    QNetworkReplyPtr reply2 = manager.get(request);
    connect(reply2, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    reply2->abort();
    QCOMPARE(reply2->error(), QNetworkReply::OperationCanceledError);
    QVERIFY(reply2->isFinished());

    // Abort after the finished()
    QNetworkRequest request3("http://" + QtNetworkSettings::serverName() + "/qtest/rfc3252.txt");
    QNetworkReplyPtr reply3 = manager.get(request3);
    connect(reply3, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(reply3->isFinished());
    reply3->abort();
    QCOMPARE(reply3->error(), QNetworkReply::NoError);
}

void tst_QNetworkReply::dontInsertPartialContentIntoTheCache()
{
    QByteArray reply206 =
            "HTTP/1.0 206\r\n"
            "Connection: keep-alive\r\n"
            "Content-Type: text/plain\r\n"
            "Cache-control: no-cache\r\n"
            "Content-Range: bytes 2-6/8\r\n"
            "Content-length: 4\r\n"
            "\r\n"
            "load";

    MiniHttpServer server(reply206);
    server.doClose = false;

    MySpyMemoryCache *memoryCache = new MySpyMemoryCache(&manager);
    manager.setCache(memoryCache);

    QUrl url = "http://localhost:" + QString::number(server.serverPort());
    QNetworkRequest request(url);
    request.setRawHeader("Range", "bytes=2-6");

    QNetworkReplyPtr reply = manager.get(request);

    connect(reply, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(server.totalConnections > 0);
    QCOMPARE(reply->readAll().constData(), "load");
    QCOMPARE(memoryCache->m_insertedUrls.count(), 0);
}

// NOTE: This test must be last testcase in tst_qnetworkreply!
void tst_QNetworkReply::parentingRepliesToTheApp()
{
    QNetworkRequest request (QUrl("http://" + QtNetworkSettings::serverName()));
    manager.get(request)->setParent(this); // parent to this object
    manager.get(request)->setParent(qApp); // parent to the app
}

QTEST_MAIN(tst_QNetworkReply)

#include "tst_qnetworkreply.moc"
