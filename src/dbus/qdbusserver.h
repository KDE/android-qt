/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef QDBUSSERVER_H
#define QDBUSSERVER_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtDBus/qdbusmacros.h>

#ifndef QT_NO_DBUS

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBusConnectionPrivate;
class QDBusError;
class QDBusConnection;

class Q_DBUS_EXPORT QDBusServer: public QObject
{
    Q_OBJECT
public:
    QDBusServer(const QString &address, QObject *parent = 0);

    bool isConnected() const;
    QDBusError lastError() const;
    QString address() const;

Q_SIGNALS:
    void newConnection(const QDBusConnection &connection);

private:
    Q_DISABLE_COPY(QDBusServer)
    QDBusConnectionPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_DBUS
#endif
