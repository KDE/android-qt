/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifndef QHTTPTHREADDELEGATE_H
#define QHTTPTHREADDELEGATE_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QThreadStorage>
#include <QNetworkProxy>
#include <QSslConfiguration>
#include <QSslError>
#include <QList>
#include <QNetworkReply>
#include "qhttpnetworkrequest_p.h"
#include "qhttpnetworkconnection_p.h"
#include <QSharedPointer>
#include "qsslconfiguration.h"
#include "private/qnoncontiguousbytedevice_p.h"
#include "qnetworkaccessauthenticationmanager_p.h"

QT_BEGIN_NAMESPACE

class QAuthenticator;
class QHttpNetworkReply;
class QEventLoop;
class QNetworkAccessCache;
class QNetworkAccessCachedHttpConnection;

class QHttpThreadDelegate : public QObject
{
    Q_OBJECT
public:
    explicit QHttpThreadDelegate(QObject *parent = 0);

    ~QHttpThreadDelegate();

    // incoming
    bool ssl;
#ifndef QT_NO_OPENSSL
    QSslConfiguration incomingSslConfiguration;
#endif
    QHttpNetworkRequest httpRequest;
    qint64 downloadBufferMaximumSize;
    // From backend, modified by us for signal compression
    QSharedPointer<QAtomicInt> pendingDownloadData;
    QSharedPointer<QAtomicInt> pendingDownloadProgress;
#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy cacheProxy;
    QNetworkProxy transparentProxy;
#endif
    QSharedPointer<QNetworkAccessAuthenticationManager> authenticationManager;
    bool synchronous;

    // outgoing, Retrieved in the synchronous HTTP case
    QByteArray synchronousDownloadData;
    QList<QPair<QByteArray,QByteArray> > incomingHeaders;
    int incomingStatusCode;
    QString incomingReasonPhrase;
    bool isPipeliningUsed;
    qint64 incomingContentLength;
    QNetworkReply::NetworkError incomingErrorCode;
    QString incomingErrorDetail;
#ifndef QT_NO_BEARERMANAGEMENT
    QSharedPointer<QNetworkSession> networkSession;
#endif

protected:
    // The zerocopy download buffer, if used:
    QSharedPointer<char> downloadBuffer;
    // The QHttpNetworkConnection that is used
    QNetworkAccessCachedHttpConnection *httpConnection;
    QByteArray cacheKey;
    QHttpNetworkReply *httpReply;

    // Used for implementing the synchronous HTTP, see startRequestSynchronously()
    QEventLoop *synchronousRequestLoop;

signals:
    void authenticationRequired(const QHttpNetworkRequest &request, QAuthenticator *);
#ifndef QT_NO_NETWORKPROXY
    void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
#endif
#ifndef QT_NO_OPENSSL
    void sslErrors(const QList<QSslError> &, bool *, QList<QSslError> *);
    void sslConfigurationChanged(const QSslConfiguration);
#endif
    void downloadMetaData(QList<QPair<QByteArray,QByteArray> >,int,QString,bool,QSharedPointer<char>,qint64);
    void downloadProgress(qint64, qint64);
    void downloadData(QByteArray);
    void error(QNetworkReply::NetworkError, const QString);
    void downloadFinished();
public slots:
    // This are called via QueuedConnection from user thread
    void startRequest();
    void abortRequest();
    // This is called with a BlockingQueuedConnection from user thread
    void startRequestSynchronously();
protected slots:
    // From QHttp*
    void readyReadSlot();
    void finishedSlot();
    void finishedWithErrorSlot(QNetworkReply::NetworkError errorCode, const QString &detail = QString());
    void synchronousFinishedSlot();
    void synchronousFinishedWithErrorSlot(QNetworkReply::NetworkError errorCode, const QString &detail = QString());
    void headerChangedSlot();
    void synchronousHeaderChangedSlot();
    void dataReadProgressSlot(int done, int total);
    void cacheCredentialsSlot(const QHttpNetworkRequest &request, QAuthenticator *authenticator);
#ifndef QT_NO_OPENSSL
    void sslErrorsSlot(const QList<QSslError> &errors);
#endif

    void synchronousAuthenticationRequiredSlot(const QHttpNetworkRequest &request, QAuthenticator *);
#ifndef QT_NO_NETWORKPROXY
    void synchronousProxyAuthenticationRequiredSlot(const QNetworkProxy &, QAuthenticator *);
#endif

protected:
    // Cache for all the QHttpNetworkConnection objects.
    // This is per thread.
    static QThreadStorage<QNetworkAccessCache *> connections;

};

// This QNonContiguousByteDevice is connected to the QNetworkAccessHttpBackend
// and represents the PUT/POST data.
class QNonContiguousByteDeviceThreadForwardImpl : public QNonContiguousByteDevice
{
    Q_OBJECT
protected:
    bool wantDataPending;
    qint64 m_amount;
    char *m_data;
    QByteArray m_dataArray;
    bool m_atEnd;
    qint64 m_size;
public:
    QNonContiguousByteDeviceThreadForwardImpl(bool aE, qint64 s)
        : QNonContiguousByteDevice(),
          wantDataPending(false),
          m_amount(0),
          m_data(0),
          m_atEnd(aE),
          m_size(s)
    {
    }

    ~QNonContiguousByteDeviceThreadForwardImpl()
    {
    }

    const char* readPointer(qint64 maximumLength, qint64 &len)
    {
        if (m_amount == 0 && wantDataPending == false) {
            len = 0;
            wantDataPending = true;
            emit wantData(maximumLength);
        } else if (m_amount == 0 && wantDataPending == true) {
            // Do nothing, we already sent a wantData signal and wait for results
            len = 0;
        } else if (m_amount > 0) {
            len = m_amount;
            return m_data;
        }
        // cannot happen
        return 0;
    }

    bool advanceReadPointer(qint64 a)
    {
        if (m_data == 0)
            return false;

        m_amount -= a;
        m_data += a;

        // To main thread to inform about our state
        emit processedData(a);

        // FIXME possible optimization, already ask user thread for some data

        return true;
    }

    bool atEnd()
    {
        if (m_amount > 0)
            return false;
        else
            return m_atEnd;
    }

    bool reset()
    {
        m_amount = 0;
        m_data = 0;

        // Communicate as BlockingQueuedConnection
        bool b = false;
        emit resetData(&b);
        return b;
    }

    qint64 size()
    {
        return m_size;
    }

public slots:
    // From user thread:
    void haveDataSlot(QByteArray dataArray, bool dataAtEnd, qint64 dataSize)
    {
        wantDataPending = false;

        m_dataArray = dataArray;
        m_data = const_cast<char*>(m_dataArray.constData());
        m_amount = dataArray.size();

        m_atEnd = dataAtEnd;
        m_size = dataSize;

        // This will tell the HTTP code (QHttpNetworkConnectionChannel) that we have data available now
        emit readyRead();
    }

signals:
    // void readyRead(); in parent class
    // void readProgress(qint64 current, qint64 total); happens in the main thread with the real bytedevice

    // to main thread:
    void wantData(qint64);
    void processedData(qint64);
    void resetData(bool *b);
};

QT_END_NAMESPACE

#endif // QHTTPTHREADDELEGATE_H
