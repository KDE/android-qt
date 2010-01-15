/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QSSLSOCKET_OPENSSL_P_H
#define QSSLSOCKET_OPENSSL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "qsslsocket_p.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#if defined(OCSP_RESPONSE)
#undef OCSP_RESPONSE
#endif
#endif

#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/stack.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/dsa.h>
#include <openssl/rsa.h>

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
typedef _STACK STACK;
#endif

QT_BEGIN_NAMESPACE

class QSslSocketBackendPrivate : public QSslSocketPrivate
{
    Q_DECLARE_PUBLIC(QSslSocket)
public:
    QSslSocketBackendPrivate();
    virtual ~QSslSocketBackendPrivate();

    // SSL context
    bool initSslContext();
    SSL *ssl;
    SSL_CTX *ctx;
    BIO *readBio;
    BIO *writeBio;
    SSL_SESSION *session;
    X509_STORE *certificateStore;
    X509_STORE_CTX *certificateStoreCtx;
    QList<QPair<int, int> > errorList;

    // Platform specific functions
    void startClientEncryption();
    void startServerEncryption();
    void transmit();
    bool startHandshake();
    void disconnectFromHost();
    void disconnected();
    QSslCipher sessionCipher() const;

    static QSslCipher QSslCipher_from_SSL_CIPHER(SSL_CIPHER *cipher);
    static QList<QSslCertificate> STACKOFX509_to_QSslCertificates(STACK_OF(X509) *x509);
};

QT_END_NAMESPACE

#endif
