/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qnativewifiengine.h"
#include "platformdefs.h"

#include <QtCore/private/qsystemlibrary_p.h>

#include <QtNetwork/private/qbearerplugin_p.h>

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_NAMESPACE

static void resolveLibrary()
{
    static bool triedResolve = false;
    if (!triedResolve) {
        QSystemLibrary wlanapi(QLatin1String("wlanapi"));
        if (wlanapi.load()) {
            local_WlanOpenHandle = (WlanOpenHandleProto)
                wlanapi.resolve("WlanOpenHandle");
            local_WlanRegisterNotification = (WlanRegisterNotificationProto)
                wlanapi.resolve("WlanRegisterNotification");
            local_WlanEnumInterfaces = (WlanEnumInterfacesProto)
                wlanapi.resolve("WlanEnumInterfaces");
            local_WlanGetAvailableNetworkList = (WlanGetAvailableNetworkListProto)
                wlanapi.resolve("WlanGetAvailableNetworkList");
            local_WlanQueryInterface = (WlanQueryInterfaceProto)
                wlanapi.resolve("WlanQueryInterface");
            local_WlanConnect = (WlanConnectProto)
                wlanapi.resolve("WlanConnect");
            local_WlanDisconnect = (WlanDisconnectProto)
                wlanapi.resolve("WlanDisconnect");
            local_WlanScan = (WlanScanProto)
                wlanapi.resolve("WlanScan");
            local_WlanFreeMemory = (WlanFreeMemoryProto)
                wlanapi.resolve("WlanFreeMemory");
            local_WlanCloseHandle = (WlanCloseHandleProto)
                wlanapi.resolve("WlanCloseHandle");
        }
        triedResolve = true;
    }
}

class QNativeWifiEnginePlugin : public QBearerEnginePlugin
{
public:
    QNativeWifiEnginePlugin();
    ~QNativeWifiEnginePlugin();

    QStringList keys() const;
    QBearerEngine *create(const QString &key) const;
};

QNativeWifiEnginePlugin::QNativeWifiEnginePlugin()
{
}

QNativeWifiEnginePlugin::~QNativeWifiEnginePlugin()
{
}

QStringList QNativeWifiEnginePlugin::keys() const
{
    return QStringList() << QLatin1String("nativewifi");
}

QBearerEngine *QNativeWifiEnginePlugin::create(const QString &key) const
{
    if (key != QLatin1String("nativewifi"))
        return 0;

    resolveLibrary();

    // native wifi dll not available
    if (!local_WlanOpenHandle)
        return 0;

    QNativeWifiEngine *engine = new QNativeWifiEngine;

    // could not initialise subsystem
    if (engine && !engine->available()) {
        delete engine;
        return 0;
    }

    return engine;
}

Q_EXPORT_STATIC_PLUGIN(QNativeWifiEnginePlugin)
Q_EXPORT_PLUGIN2(qnativewifibearer, QNativeWifiEnginePlugin)

QT_END_NAMESPACE

#endif // QT_NO_BEARERMANAGEMENT
