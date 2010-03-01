/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qgenericengine.h"
#include "../qnetworksession_impl.h"

#include <QtNetwork/private/qnetworkconfiguration_p.h>

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

#ifdef Q_OS_WIN
#include "../platformdefs_win.h"
#endif

#ifdef Q_OS_LINUX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

static QString qGetInterfaceType(const QString &interface)
{
#ifdef Q_OS_WIN32
    unsigned long oid;
    DWORD bytesWritten;

    NDIS_MEDIUM medium;
    NDIS_PHYSICAL_MEDIUM physicalMedium;

    HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(interface).utf16(), 0,
                               FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
        return QLatin1String("Unknown");

    oid = OID_GEN_MEDIA_SUPPORTED;
    bytesWritten = 0;
    bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                  &medium, sizeof(medium), &bytesWritten, 0);
    if (!result) {
        CloseHandle(handle);
        return QLatin1String("Unknown");
    }

    oid = OID_GEN_PHYSICAL_MEDIUM;
    bytesWritten = 0;
    result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                             &physicalMedium, sizeof(physicalMedium), &bytesWritten, 0);
    if (!result) {
        CloseHandle(handle);

        if (medium == NdisMedium802_3)
            return QLatin1String("Ethernet");
        else
            return QLatin1String("Unknown");
    }

    CloseHandle(handle);

    if (medium == NdisMedium802_3) {
        switch (physicalMedium) {
        case NdisPhysicalMediumWirelessLan:
            return QLatin1String("WLAN");
        case NdisPhysicalMediumBluetooth:
            return QLatin1String("Bluetooth");
        case NdisPhysicalMediumWiMax:
            return QLatin1String("WiMAX");
        default:
#ifdef BEARER_MANAGEMENT_DEBUG
            qDebug() << "Physical Medium" << physicalMedium;
#endif
            return QLatin1String("Ethernet");
        }
    }

#ifdef BEARER_MANAGEMENT_DEBUG
    qDebug() << medium << physicalMedium;
#endif
#elif defined(Q_OS_LINUX)
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    ifreq request;
    strncpy(request.ifr_name, interface.toLocal8Bit().data(), sizeof(request.ifr_name));
    int result = ioctl(sock, SIOCGIFHWADDR, &request);
    close(sock);

    if (result >= 0 && request.ifr_hwaddr.sa_family == ARPHRD_ETHER)
        return QLatin1String("Ethernet");
#else
    Q_UNUSED(interface);
#endif

    return QLatin1String("Unknown");
}

QGenericEngine::QGenericEngine(QObject *parent)
:   QBearerEngineImpl(parent)
{
    connect(&pollTimer, SIGNAL(timeout()), this, SLOT(doRequestUpdate()));
    pollTimer.setInterval(10000);
    doRequestUpdate();
}

QGenericEngine::~QGenericEngine()
{
}

QString QGenericEngine::getInterfaceFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    return configurationInterface.value(id);
}

bool QGenericEngine::hasIdentifier(const QString &id)
{
    QMutexLocker locker(&mutex);

    return configurationInterface.contains(id);
}

void QGenericEngine::connectToId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QGenericEngine::disconnectFromId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QGenericEngine::requestUpdate()
{
    QMutexLocker locker(&mutex);

    pollTimer.stop();
    QTimer::singleShot(0, this, SLOT(doRequestUpdate()));
}

void QGenericEngine::doRequestUpdate()
{
    QMutexLocker locker(&mutex);

    // Immediately after connecting with a wireless access point
    // QNetworkInterface::allInterfaces() will sometimes return an empty list. Calling it again a
    // second time results in a non-empty list. If we loose interfaces we will end up removing
    // network configurations which will break current sessions.
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    if (interfaces.isEmpty())
        interfaces = QNetworkInterface::allInterfaces();

    QStringList previous = accessPointConfigurations.keys();

    // create configuration for each interface
    while (!interfaces.isEmpty()) {
        QNetworkInterface interface = interfaces.takeFirst();

        if (!interface.isValid())
            continue;

        // ignore loopback interface
        if (interface.flags() & QNetworkInterface::IsLoopBack)
            continue;

        // ignore WLAN interface handled in seperate engine
        if (qGetInterfaceType(interface.name()) == QLatin1String("WLAN"))
            continue;

        uint identifier;
        if (interface.index())
            identifier = qHash(QLatin1String("generic:") + QString::number(interface.index()));
        else
            identifier = qHash(QLatin1String("generic:") + interface.hardwareAddress());

        const QString id = QString::number(identifier);

        previous.removeAll(id);

        QString name = interface.humanReadableName();
        if (name.isEmpty())
            name = interface.name();

        QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Discovered;
        if (interface.flags() & QNetworkInterface::IsUp)
            state |= QNetworkConfiguration::Active;

        if (accessPointConfigurations.contains(id)) {
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

            bool changed = false;

            if (!ptr->isValid) {
                ptr->isValid = true;
                changed = true;
            }

            if (ptr->name != name) {
                ptr->name = name;
                changed = true;
            }

            if (ptr->id != id) {
                ptr->id = id;
                changed = true;
            }

            if (ptr->state != state) {
                ptr->state = state;
                changed = true;
            }

            if (changed)
                emit configurationChanged(ptr);
        } else {
            QNetworkConfigurationPrivatePointer ptr(new QNetworkConfigurationPrivate);

            ptr->name = name;
            ptr->isValid = true;
            ptr->id = id;
            ptr->state = state;
            ptr->type = QNetworkConfiguration::InternetAccessPoint;
            ptr->bearer = qGetInterfaceType(interface.name());

            accessPointConfigurations.insert(id, ptr);
            configurationInterface.insert(id, interface.name());

            emit configurationAdded(ptr);
        }
    }

    while (!previous.isEmpty()) {
        QNetworkConfigurationPrivatePointer ptr =
            accessPointConfigurations.take(previous.takeFirst());

        configurationInterface.remove(ptr->id);
        emit configurationRemoved(ptr);
    }

    pollTimer.start();

    emit updateCompleted();
}

QNetworkSession::State QGenericEngine::sessionStateForId(const QString &id)
{
    QMutexLocker locker(&mutex);

    QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

    if (!ptr)
        return QNetworkSession::Invalid;

    if (!ptr->isValid) {
        return QNetworkSession::Invalid;
    } else if ((ptr->state & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        return QNetworkSession::Connected;
    } else if ((ptr->state & QNetworkConfiguration::Discovered) ==
                QNetworkConfiguration::Discovered) {
        return QNetworkSession::Disconnected;
    } else if ((ptr->state & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
        return QNetworkSession::NotAvailable;
    } else if ((ptr->state & QNetworkConfiguration::Undefined) ==
                QNetworkConfiguration::Undefined) {
        return QNetworkSession::NotAvailable;
    }

    return QNetworkSession::Invalid;
}

QNetworkConfigurationManager::Capabilities QGenericEngine::capabilities() const
{
    return QNetworkConfigurationManager::ForcedRoaming;
}

QNetworkSessionPrivate *QGenericEngine::createSessionBackend()
{
    return new QNetworkSessionPrivateImpl;
}

QNetworkConfigurationPrivatePointer QGenericEngine::defaultConfiguration()
{
    return QNetworkConfigurationPrivatePointer();
}

QT_END_NAMESPACE

