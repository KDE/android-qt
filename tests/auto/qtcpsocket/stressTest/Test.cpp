/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
// Qt
#include <QByteArray>
#include <QCoreApplication>
#include <QDataStream>
#include <QTimer>

// Test
#include "Test.h"

#ifdef QT3_SUPPORT
//------------------------------------------------------------------------------
My3Socket::My3Socket(QObject *parent) 
    : Q3Socket(parent), safeShutDown(false)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(read()));
    connect(this, SIGNAL(delayedCloseFinished()), this, SLOT(closed()));
    connect(this, SIGNAL(connectionClosed()), this, SLOT(closed()));
}

//------------------------------------------------------------------------------
void My3Socket::read()
{
    QDataStream in(this);

    quint32 num, reply;

    while (bytesAvailable()) {
        in >> num;
        if (num == 42) {
            qDebug("SUCCESS");
            safeShutDown = true;
            QCoreApplication::instance()->quit();
            return;
        }
        reply = num + 1;
        if (reply == 42)
            ++reply;
    }
    
    // Reply with a bigger number
    sendTest(reply);
}

//------------------------------------------------------------------------------
void My3Socket::closed()
{
    if (!safeShutDown)
        qDebug("FAILED");
    QCoreApplication::instance()->quit();
}

//------------------------------------------------------------------------------
void My3Socket::sendTest(quint32 num)
{
    QByteArray  block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << num;
    writeBlock(block, block.size());
}

//------------------------------------------------------------------------------
My3Server::My3Server(QObject *parent)
    : Q3ServerSocket(7700, 1, parent), m_socket(0)
{
    if (ok())
        qDebug("qt3server");

    QTimer::singleShot(5000, this, SLOT(stopServer()));
}

//------------------------------------------------------------------------------
void My3Server::newConnection(int socketId)
{
    m_socket = new My3Socket(this);
    m_socket->setSocket(socketId);
}

//------------------------------------------------------------------------------
void My3Server::stopServer()
{
    if (m_socket) {
        qDebug("SUCCESS");
        m_socket->safeShutDown = true;
        m_socket->sendTest(42);
    } else {
        QCoreApplication::instance()->quit();
    }
}
#endif

//------------------------------------------------------------------------------
My4Socket::My4Socket(QObject *parent) 
    : QTcpSocket(parent), safeShutDown(false)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(read()));
    connect(this, SIGNAL(disconnected()), this, SLOT(closed()));
}

//------------------------------------------------------------------------------
void My4Socket::read(void)
{
    QDataStream in(this);

    quint32 num, reply;

    while (bytesAvailable()) {
        in >> num;
        if (num == 42) {
            safeShutDown = true;
            qDebug("SUCCESS");
            QCoreApplication::instance()->quit();
            return;
        }
        reply = num + 1;
        if (reply == 42)
            ++reply;
    }
    
    // Reply with a bigger number
    sendTest(reply);
}

//------------------------------------------------------------------------------
void My4Socket::closed(void)
{
    if (!safeShutDown)
        qDebug("FAILED");
    QCoreApplication::instance()->quit();
}

//------------------------------------------------------------------------------
void My4Socket::sendTest(quint32 num)
{
    QByteArray  block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << num;

    write(block, block.size());
}

//------------------------------------------------------------------------------
My4Server::My4Server(QObject *parent)
    : QTcpServer(parent)
{
    if (listen(QHostAddress::Any, 7700))
        qDebug("qt4server");
    QTimer::singleShot(5000, this, SLOT(stopServer()));
}

//------------------------------------------------------------------------------
void My4Server::incomingConnection(int socketId)
{
    m_socket = new My4Socket(this);
    m_socket->setSocketDescriptor(socketId);
}

//------------------------------------------------------------------------------
void My4Server::stopServer()
{
    if (m_socket) {
        qDebug("SUCCESS");
        m_socket->safeShutDown = true;
        m_socket->sendTest(42);
    } else {
        QCoreApplication::instance()->quit();
    }
}

//------------------------------------------------------------------------------
Test::Test(Type type)
{
    switch (type) {
#ifdef QT3_SUPPORT
    case Qt3Server: {
        new My3Server(this);
        break;
    }
    case Qt3Client: {
        qDebug("qt3client");
        My3Socket *s = new My3Socket(this);
        s->connectToHost("localhost", 7700);
        s->sendTest(1);
        break;
    }
#endif
    case Qt4Client: {
        qDebug("qt4client");
        My4Socket *s = new My4Socket(this);
        s->connectToHost("localhost", 7700);
        s->sendTest(1);
        break;
    }
    case Qt4Server: {
        new My4Server(this);
        break;
    }
    default:
        break;
    }
}
