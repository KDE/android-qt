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
#ifndef TEST_H
#define TEST_H

//------------------------------------------------------------------------------
// Qt
#ifdef QT3_SUPPORT
#include <Q3ServerSocket>
#include <Q3Socket>
#endif

#include <QTcpServer>
#include <QTcpSocket>

//------------------------------------------------------------------------------
#ifdef QT3_SUPPORT
class My3Socket : public Q3Socket
{
    Q_OBJECT
public:
    My3Socket(QObject *parent);

    void sendTest(Q_UINT32 num);
    bool safeShutDown;

private slots:
    void read();
    void closed();
};

//------------------------------------------------------------------------------
class My3Server : public Q3ServerSocket
{
    Q_OBJECT
public:
    My3Server(QObject *parent = 0);

    void newConnection(int socket);

private slots:
    void stopServer();

private:
    My3Socket *m_socket;
};
#endif

//------------------------------------------------------------------------------
class My4Socket : public QTcpSocket
{
    Q_OBJECT
public:
    My4Socket(QObject *parent);

    void sendTest(quint32 num);
    bool safeShutDown;

private slots:
    void read();
    void closed();
};

//------------------------------------------------------------------------------
class My4Server : public QTcpServer
{
    Q_OBJECT
public:
    My4Server(QObject *parent = 0);

protected:
    void incomingConnection(int socket);

private slots:
    void stopServer();

private:
    My4Socket *m_socket;
};

//------------------------------------------------------------------------------
class Test : public QObject
{
    Q_OBJECT

public:
    enum Type {
        Qt4Client,
        Qt4Server,
#ifdef QT3_SUPPORT
        Qt3Client,
        Qt3Server
#endif
    };
    Test(Type type);
};

//------------------------------------------------------------------------------
#endif	// TEST_H
