/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include "connectionmanager.h"
#include "commands.h"
#include <QtCore/QDebug>

ConnectionManager::ConnectionManager()
    : QObject()
    , m_server(0)
{
    debugOutput(0, "ConnectionManager::ConnectionManager()");
}

ConnectionManager::~ConnectionManager()
{
    debugOutput(0, "ConnectionManager::~ConnectionManager()");
    cleanUp();
}

bool ConnectionManager::init()
{
    debugOutput(0, "ConnectionManager::init()");
    debugOutput(3, "Initializing server...");
    cleanUp();
    m_server = new QTcpServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    bool result = m_server->listen(QHostAddress::Any, SERVER_PORT);
    if (!result)
        debugOutput(3, QString::fromLatin1("   Error: Server start failed:") + m_server->errorString());
    debugOutput(3, "   Waiting for action");
    return result;
}

void ConnectionManager::cleanUp()
{
    debugOutput(0, "ConnectionManager::cleanUp()");

    if (m_server) {
        debugOutput(1, "Removing server instance...");
        disconnect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
        delete m_server;
        m_server = 0;
    }
}

void ConnectionManager::newConnection()
{
    debugOutput(0, "ConnectionManager::newConnection()");

    QTcpSocket* connection = m_server->nextPendingConnection();
    if (!connection) {
        debugOutput(3, "Received connection has empty socket");
        return;
    }
    debugOutput(0, QString::fromLatin1("   received a connection: %1").arg((int) connection));
    new Connection(connection);
}

Connection::Connection(QTcpSocket *socket)
        : QObject()
        , m_connection(socket)
        , m_command(0)
{
    connect(m_connection, SIGNAL(readyRead()), this, SLOT(receiveCommand()));
    connect(m_connection, SIGNAL(disconnected()), this, SLOT(closedConnection()));
}

Connection::~Connection()
{
    if (m_command) {
        m_command->commandFinished();
        delete m_command;
        m_command = 0;
    }
    delete m_connection;
}

void Connection::receiveCommand()
{
    QByteArray arr = m_connection->readAll();
    debugOutput(1, QString::fromLatin1("Command received: ") + (arr));
    QList<CommandInfo> commands = availableCommands();
    for(QList<CommandInfo>::iterator it = commands.begin(); it != commands.end(); ++it) {
        if (it->commandName == QString::fromLatin1(arr)) {
            debugOutput(1, "Found command in list");
            disconnect(m_connection, SIGNAL(readyRead()), this, SLOT(receiveCommand()));
            AbstractCommand* command = (*it).commandFunc();
            command->setSocket(m_connection);
            m_command = command;
            return;
        }
    }
    debugOutput(2, QString::fromLatin1("Unknown command received: ") + (arr));
}

void Connection::closedConnection()
{
    debugOutput(0, "connection being closed...");
    this->deleteLater();
}
