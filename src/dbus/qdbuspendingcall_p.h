/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the public API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifndef QDBUSPENDINGCALL_P_H
#define QDBUSPENDINGCALL_P_H

#include <qshareddata.h>
#include <qpointer.h>
#include <qlist.h>

#include "qdbusmessage.h"
#include "qdbus_symbols_p.h"

QT_BEGIN_NAMESPACE

class QDBusPendingCall;
class QDBusPendingCallWatcher;
class QDBusPendingCallWatcherHelper;
class QDBusConnectionPrivate;

class QDBusPendingCallPrivate: public QSharedData
{
public:
    QDBusMessage sentMessage;
    QDBusMessage replyMessage;
//    QDBusMessage pendingReplyMessage; // used in the local loop
    QDBusPendingCallWatcherHelper *watcherHelper;
    DBusPendingCall *pending;
    QDBusConnectionPrivate *connection;

    QString expectedReplySignature;
    int expectedReplyCount;

    // for the callback
    QPointer<QObject> receiver;
    QList<int> metaTypes;
    int methodIdx;

    bool autoDelete;

    QDBusPendingCallPrivate() : watcherHelper(0), pending(0), autoDelete(false)
    { }
    ~QDBusPendingCallPrivate();
    bool setReplyCallback(QObject *target, const char *member);
    void waitForFinished();
    void setMetaTypes(int count, const int *types);
    void checkReceivedSignature();

    static QDBusPendingCall fromMessage(const QDBusMessage &msg);
};

class QDBusPendingCallWatcherHelper: public QObject
{
    Q_OBJECT
public:
    void add(QDBusPendingCallWatcher *watcher);

    void emitSignals(const QDBusMessage &replyMessage, const QDBusMessage &sentMessage)
    {
        if (replyMessage.type() == QDBusMessage::ReplyMessage)
            emit reply(replyMessage);
        else
            emit error(replyMessage, sentMessage);
        emit finished();
    }

Q_SIGNALS:
    void finished();
    void reply(const QDBusMessage &msg);
    void error(const QDBusError &error, const QDBusMessage &msg);
};

QT_END_NAMESPACE

#endif
