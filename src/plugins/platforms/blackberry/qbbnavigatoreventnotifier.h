/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
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
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBNAVIGATOREVENTNOTIFIER_H
#define QBBNAVIGATOREVENTNOTIFIER_H

#include <QObject>

QT_BEGIN_NAMESPACE

class QBBNavigatorEventHandler;
class QSocketNotifier;

class QBBNavigatorEventNotifier : public QObject
{
    Q_OBJECT
public:
    explicit QBBNavigatorEventNotifier(QBBNavigatorEventHandler *eventHandler, QObject *parent = 0);
    ~QBBNavigatorEventNotifier();

public Q_SLOTS:
    void start();

private Q_SLOTS:
    void readData();

private:
    QBBNavigatorEventHandler *mEventHandler;
    int mFd;
    QSocketNotifier *mReadNotifier;

    void parsePPS(const QByteArray &ppsData, QByteArray &msg, QByteArray &dat, QByteArray &id);
    void replyPPS(const QByteArray &res, const QByteArray &id, const QByteArray &dat);
    void handleMessage(const QByteArray &msg, const QByteArray &dat, const QByteArray &id);
};

QT_END_NAMESPACE

#endif // QBBNAVIGATOREVENTNOTIFIER_H
