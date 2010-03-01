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

#ifndef SYMBIANENGINE_H
#define SYMBIANENGINE_H

#include <QtCore/qstringlist.h>
#include <QtNetwork/private/qbearerengine_p.h>
#include <QtNetwork/qnetworkconfigmanager.h>

#include <QHash>
#include <rconnmon.h>
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    #include <cmmanager.h>
#endif

class CCommsDatabase;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QNetworkSessionPrivate;
class AccessPointsAvailabilityScanner;

class SymbianNetworkConfigurationPrivate : public QNetworkConfigurationPrivate
{
public:
    enum Bearer {
        BearerEthernet,
        BearerWLAN,
        Bearer2G,
        BearerCDMA2000,
        BearerWCDMA,
        BearerHSPA,
        BearerBluetooth,
        BearerWiMAX,
        BearerUnknown = -1
    };

    SymbianNetworkConfigurationPrivate();
    ~SymbianNetworkConfigurationPrivate();

    QString bearerName() const;

    Bearer bearer;

    TUint32 numericId;
    TUint connectionId;

    QNetworkConfigurationPrivatePointer serviceNetworkPtr;

    QString mappingName;
};

inline SymbianNetworkConfigurationPrivate *toSymbianConfig(QNetworkConfigurationPrivatePointer ptr)
{
    return static_cast<SymbianNetworkConfigurationPrivate *>(ptr.data());
}

class SymbianEngine : public QBearerEngine, public CActive,
                      public MConnectionMonitorObserver
{
    Q_OBJECT

public:
    SymbianEngine(QObject *parent = 0);
    virtual ~SymbianEngine();

    bool hasIdentifier(const QString &id);

    void requestUpdate();

    QNetworkConfigurationManager::Capabilities capabilities() const;

    QNetworkSessionPrivate *createSessionBackend();

    QNetworkConfigurationPrivatePointer defaultConfiguration();

    QStringList accessPointConfigurationIdentifiers();

Q_SIGNALS:
    void onlineStateChanged(bool isOnline);
    
public Q_SLOTS:
    void updateConfigurations();

private:
    void updateStatesToSnaps();
    bool changeConfigurationStateTo(QNetworkConfigurationPrivatePointer ptr,
                                    QNetworkConfiguration::StateFlags newState);
    bool changeConfigurationStateAtMinTo(QNetworkConfigurationPrivatePointer ptr,
                                         QNetworkConfiguration::StateFlags newState);
    bool changeConfigurationStateAtMaxTo(QNetworkConfigurationPrivatePointer ptr,
                                         QNetworkConfiguration::StateFlags newState);
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    SymbianNetworkConfigurationPrivate *configFromConnectionMethodL(RCmConnectionMethod& connectionMethod);
#else
    bool readNetworkConfigurationValuesFromCommsDb(
            TUint32 aApId, SymbianNetworkConfigurationPrivate *apNetworkConfiguration);
    void readNetworkConfigurationValuesFromCommsDbL(
            TUint32 aApId, SymbianNetworkConfigurationPrivate *apNetworkConfiguration);
#endif    
    
    void updateConfigurationsL();
    void updateActiveAccessPoints();
    void updateAvailableAccessPoints();
    void accessPointScanningReady(TBool scanSuccessful, TConnMonIapInfo iapInfo);
    void startCommsDatabaseNotifications();
    void stopCommsDatabaseNotifications();

    QNetworkConfigurationPrivatePointer defaultConfigurationL();
    TBool GetS60PlatformVersion(TUint& aMajor, TUint& aMinor) const;
    void startMonitoringIAPData(TUint32 aIapId);
    QNetworkConfigurationPrivatePointer dataByConnectionId(TUint aConnectionId);

protected: // From CActive
    void RunL();
    void DoCancel();
    
private: // MConnectionMonitorObserver
    void EventL(const CConnMonEventBase& aEvent);

private: // Data
    CCommsDatabase*    ipCommsDB;
    RConnectionMonitor iConnectionMonitor;

    TBool              iWaitingCommsDatabaseNotifications;
    TBool              iOnline;
    TBool              iInitOk;
    TBool              iUpdateGoingOn;

    
    AccessPointsAvailabilityScanner* ipAccessPointsAvailabilityScanner;
    
    friend class QNetworkSessionPrivate;
    friend class AccessPointsAvailabilityScanner;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    RCmManager iCmManager;
#endif
};

class AccessPointsAvailabilityScanner : public CActive
{
public:
    AccessPointsAvailabilityScanner(SymbianEngine& owner,
                                   RConnectionMonitor& connectionMonitor); 
    ~AccessPointsAvailabilityScanner();

    void StartScanning();
    
protected: // From CActive
    void RunL();
    void DoCancel();

private: // Data
    SymbianEngine& iOwner;
    RConnectionMonitor& iConnectionMonitor;
    TConnMonIapInfoBuf iIapBuf;
};

QT_END_NAMESPACE

#endif
