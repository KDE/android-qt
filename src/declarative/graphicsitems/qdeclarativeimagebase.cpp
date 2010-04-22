/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qdeclarativeimagebase_p.h"
#include "private/qdeclarativeimagebase_p_p.h"

#include <qdeclarativeengine.h>
#include <qdeclarativeinfo.h>
#include <qdeclarativepixmapcache_p.h>

#include <QFile>

QT_BEGIN_NAMESPACE

QDeclarativeImageBase::QDeclarativeImageBase(QDeclarativeImageBasePrivate &dd, QDeclarativeItem *parent)
  : QDeclarativeItem(dd, parent)
{
}

QDeclarativeImageBase::~QDeclarativeImageBase()
{
    Q_D(QDeclarativeImageBase);
    if (d->pendingPixmapCache)
        QDeclarativePixmapCache::cancel(d->url, this);
}

QDeclarativeImageBase::Status QDeclarativeImageBase::status() const
{
    Q_D(const QDeclarativeImageBase);
    return d->status;
}


qreal QDeclarativeImageBase::progress() const
{
    Q_D(const QDeclarativeImageBase);
    return d->progress;
}


bool QDeclarativeImageBase::asynchronous() const
{
    Q_D(const QDeclarativeImageBase);
    return d->async;
}

void QDeclarativeImageBase::setAsynchronous(bool async)
{
    Q_D(QDeclarativeImageBase);
    if (d->async != async) {
        d->async = async;
        emit asynchronousChanged();
    }
}


QUrl QDeclarativeImageBase::source() const
{
    Q_D(const QDeclarativeImageBase);
    return d->url;
}

void QDeclarativeImageBase::setSource(const QUrl &url)
{
    Q_D(QDeclarativeImageBase);
    //equality is fairly expensive, so we bypass for simple, common case
    if ((d->url.isEmpty() == url.isEmpty()) && url == d->url)
        return;

    if (d->pendingPixmapCache) {
        QDeclarativePixmapCache::cancel(d->url, this);
        d->pendingPixmapCache = false;
    }

    d->url = url;
    emit sourceChanged(d->url);

    if (isComponentComplete())
        load();
}

void QDeclarativeImageBase::setSourceSize(const QSize& size)
{
    Q_D(QDeclarativeImageBase);
    if (d->sourcesize == size)
        return;
    d->sourcesize = size;
    emit sourceSizeChanged();
    if (isComponentComplete())
        load();
}

QSize QDeclarativeImageBase::sourceSize() const
{
    Q_D(const QDeclarativeImageBase);
    return d->sourcesize.isValid() ? d->sourcesize : QSize(implicitWidth(),implicitHeight());
}

void QDeclarativeImageBase::load()
{
    Q_D(QDeclarativeImageBase);
    if (d->progress != 0.0) {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (d->url.isEmpty()) {
        d->pix = QPixmap();
        d->status = Null;
        setImplicitWidth(0);
        setImplicitHeight(0);
        emit statusChanged(d->status);
        pixmapChange();
        update();
    } else {
        d->status = Loading;
        int reqwidth = d->sourcesize.width();
        int reqheight = d->sourcesize.height();
        QSize impsize;
        QString errorString;
        QDeclarativePixmapReply::Status status = QDeclarativePixmapCache::get(d->url, &d->pix, &errorString, &impsize, d->async, reqwidth, reqheight);
        if (status != QDeclarativePixmapReply::Ready && status != QDeclarativePixmapReply::Error) {
            QDeclarativePixmapReply *reply = QDeclarativePixmapCache::request(qmlEngine(this), d->url, reqwidth, reqheight);
            d->pendingPixmapCache = true;

            static int replyDownloadProgress = -1;
            static int replyFinished = -1;
            static int thisRequestProgress = -1;
            static int thisRequestFinished = -1;
            if (replyDownloadProgress == -1) {
                replyDownloadProgress =
                    QDeclarativePixmapReply::staticMetaObject.indexOfSignal("downloadProgress(qint64,qint64)");
                replyFinished =
                    QDeclarativePixmapReply::staticMetaObject.indexOfSignal("finished()");
                thisRequestProgress =
                    QDeclarativeImageBase::staticMetaObject.indexOfSlot("requestProgress(qint64,qint64)");
                thisRequestFinished =
                    QDeclarativeImageBase::staticMetaObject.indexOfSlot("requestFinished()");
            }

            QMetaObject::connect(reply, replyFinished, this,
                                 thisRequestFinished, Qt::DirectConnection);
            QMetaObject::connect(reply, replyDownloadProgress, this,
                                 thisRequestProgress, Qt::DirectConnection);
        } else {
            //### should be unified with requestFinished
            if (status == QDeclarativePixmapReply::Ready) {
                setImplicitWidth(impsize.width());
                setImplicitHeight(impsize.height());

                if (d->status == Loading)
                    d->status = Ready;

                if (!d->sourcesize.isValid())
                    emit sourceSizeChanged();
            } else {
                d->status = Error;
                qmlInfo(this) << errorString;
            }
            d->progress = 1.0;
            emit statusChanged(d->status);
            emit progressChanged(d->progress);
            pixmapChange();
            update();
        }
    }

    emit statusChanged(d->status);
}

void QDeclarativeImageBase::requestFinished()
{
    Q_D(QDeclarativeImageBase);

    d->pendingPixmapCache = false;

    QSize impsize;
    QString errorString;
    if (QDeclarativePixmapCache::get(d->url, &d->pix, &errorString, &impsize, d->async, d->sourcesize.width(), d->sourcesize.height()) != QDeclarativePixmapReply::Ready) {
        d->status = Error;
        qmlInfo(this) << errorString;
    }
    setImplicitWidth(impsize.width());
    setImplicitHeight(impsize.height());

    if (d->status == Loading)
        d->status = Ready;
    d->progress = 1.0;
    emit statusChanged(d->status);
    emit progressChanged(1.0);
    if (!d->sourcesize.isValid())
        emit sourceSizeChanged();
    pixmapChange();
    update();
}

void QDeclarativeImageBase::requestProgress(qint64 received, qint64 total)
{
    Q_D(QDeclarativeImageBase);
    if (d->status == Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
    }
}

void QDeclarativeImageBase::componentComplete()
{
    Q_D(QDeclarativeImageBase);
    QDeclarativeItem::componentComplete();
    if (d->url.isValid())
        load();
}

void QDeclarativeImageBase::pixmapChange()
{
}

QT_END_NAMESPACE
