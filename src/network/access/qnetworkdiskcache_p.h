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

#ifndef QNETWORKDISKCACHE_P_H
#define QNETWORKDISKCACHE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qabstractnetworkcache_p.h"

#include <qbuffer.h>
#include <qhash.h>
#include <qtemporaryfile.h>

#ifndef QT_NO_NETWORKDISKCACHE

QT_BEGIN_NAMESPACE

class QFile;

class QCacheItem
{
public:
    QCacheItem() : file(0)
    {
    }
    ~QCacheItem()
    {
        reset();
    }

    QNetworkCacheMetaData metaData;
    QBuffer data;
    QTemporaryFile *file;
    inline qint64 size() const
        { return file ? file->size() : data.size(); }

    inline void reset() {
        metaData = QNetworkCacheMetaData();
        data.close();
        delete file;
        file = 0;
    }
    void writeHeader(QFile *device) const;
    void writeCompressedData(QFile *device) const;
    bool read(QFile *device, bool readData);

    bool canCompress() const;
};

class QNetworkDiskCachePrivate : public QAbstractNetworkCachePrivate
{
public:
    QNetworkDiskCachePrivate()
        : QAbstractNetworkCachePrivate()
        , maximumCacheSize(1024 * 1024 * 50)
        , currentCacheSize(-1)
        {}

    QByteArray generateId(const QUrl &url) const;
    QString cacheFileName(const QUrl &url) const;
    QString tmpCacheFileName() const;
    bool removeFile(const QString &file);
    void storeItem(QCacheItem *item);

    mutable QCacheItem lastItem;
    QString cacheDirectory;
    qint64 maximumCacheSize;
    qint64 currentCacheSize;

    QHash<QIODevice*, QCacheItem*> inserting;
    Q_DECLARE_PUBLIC(QNetworkDiskCache)
};

QT_END_NAMESPACE

#endif // QT_NO_NETWORKDISKCACHE

#endif // QNETWORKDISKCACHE_P_H
