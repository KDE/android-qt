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

#ifndef QNETWORKSESSION_IMPL_H
#define QNETWORKSESSION_IMPL_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtNetwork/private/qnetworksession_p.h>

#include <QDateTime>

#include <e32base.h>
#include <CommDbConnPref.h>
#include <es_sock.h>
#include <rconnmon.h>
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    #include <comms-infras/cs_mobility_apiext.h>
#endif

typedef int(*TOpenCSetdefaultifFunction)(const struct ifreq*);

QT_BEGIN_NAMESPACE

class ConnectionProgressNotifier;
class SymbianEngine;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
class QNetworkSessionPrivateImpl : public QNetworkSessionPrivate, public CActive, public MMobilityProtocolResp,
                                     public MConnectionMonitorObserver
#else
class QNetworkSessionPrivateImpl : public QNetworkSessionPrivate, public CActive, public MConnectionMonitorObserver
#endif
{
    Q_OBJECT
public:
    QNetworkSessionPrivateImpl(SymbianEngine *engine);
    ~QNetworkSessionPrivateImpl();
    
    //called by QNetworkSession constructor and ensures
    //that the state is immediately updated (w/o actually opening
    //a session). Also this function should take care of 
    //notification hooks to discover future state changes.
    void syncStateWithInterface();

    QNetworkInterface currentInterface() const;
    QVariant sessionProperty(const QString& key) const;
    void setSessionProperty(const QString& key, const QVariant& value);
    
    void setALREnabled(bool enabled);

    void open();
    inline void close() { close(true); }
    void close(bool allowSignals);
    void stop();
    void migrate();
    void accept();
    void ignore();
    void reject();

    QString errorString() const; //must return translated string
    QNetworkSession::SessionError error() const;

    quint64 bytesWritten() const;
    quint64 bytesReceived() const;
    quint64 activeTime() const;
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
public: // From MMobilityProtocolResp
    void PreferredCarrierAvailable(TAccessPointInfo aOldAPInfo,
                                   TAccessPointInfo aNewAPInfo,
                                   TBool aIsUpgrade,
                                   TBool aIsSeamless);

    void NewCarrierActive(TAccessPointInfo aNewAPInfo, TBool aIsSeamless);

    void Error(TInt aError);
#endif    

protected: // From CActive
    void RunL();
    void DoCancel();
    
private: // MConnectionMonitorObserver
    void EventL(const CConnMonEventBase& aEvent);
    
private:
    TUint iapClientCount(TUint aIAPId) const;
    quint64 transferredData(TUint dataType) const;
    bool newState(QNetworkSession::State newState, TUint accessPointId = 0);
    void handleSymbianConnectionStatusChange(TInt aConnectionStatus, TInt aError, TUint accessPointId = 0);
    QNetworkConfiguration bestConfigFromSNAP(const QNetworkConfiguration& snapConfig) const;
    QNetworkConfiguration activeConfiguration(TUint32 iapId = 0) const;
    QNetworkInterface interface(TUint iapId) const;

private: // data
    SymbianEngine *engine;

    mutable QNetworkInterface activeInterface;

    QDateTime startTime;

    RLibrary iOpenCLibrary;
    TOpenCSetdefaultifFunction iDynamicSetdefaultif;

    mutable RSocketServ iSocketServ;
    mutable RConnection iConnection;
    mutable RConnectionMonitor iConnectionMonitor;
    ConnectionProgressNotifier* ipConnectionNotifier;
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    CActiveCommsMobilityApiExt* iMobility;
#endif    
    
    QNetworkSession::SessionError iError;
    TInt iALREnabled;
    TBool iALRUpgradingConnection;
    
    QList<QString> iKnownConfigsBeforeConnectionStart;
    
    TUint32 iOldRoamingIap;
    TUint32 iNewRoamingIap;

    friend class ConnectionProgressNotifier;
};

class ConnectionProgressNotifier : public CActive
{
public:
    ConnectionProgressNotifier(QNetworkSessionPrivateImpl &owner, RConnection &connection);
    ~ConnectionProgressNotifier();
    
    void StartNotifications();
    void StopNotifications();
    
protected: // From CActive
    void RunL();
    void DoCancel();

private: // Data
    QNetworkSessionPrivateImpl &iOwner;
    RConnection& iConnection;
    TNifProgressBuf iProgress;
    
};

QT_END_NAMESPACE

#endif //QNETWORKSESSION_IMPL_H

