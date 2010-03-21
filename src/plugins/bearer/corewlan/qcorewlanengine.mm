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

#include "qcorewlanengine.h"
#include "../qnetworksession_impl.h"

#include <QtNetwork/private/qnetworkconfiguration_p.h>

#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>
#include <QDir>
#include <CoreWLAN/CoreWLAN.h>
#include <CoreWLAN/CWInterface.h>
#include <CoreWLAN/CWNetwork.h>
#include <CoreWLAN/CWNetwork.h>

#include <Foundation/NSEnumerator.h>
#include <Foundation/NSKeyValueObserving.h>
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSLock.h>

#include <SystemConfiguration/SCNetworkConfiguration.h>
#include <private/qt_cocoa_helpers_mac_p.h>
#include "private/qcore_mac_p.h"

QMap <QString, QString> networkInterfaces;

@interface QNSListener : NSObject
{
    NSNotificationCenter *center;
    CWInterface *currentInterface;
    QCoreWlanEngine *engine;
    NSLock *locker;
}
- (void)notificationHandler;//:(NSNotification *)notification;
- (void)remove;
- (void)setEngine:(QCoreWlanEngine *)coreEngine;
- (void)dealloc;

@property (assign) QCoreWlanEngine* engine;

@end

@implementation QNSListener
- (id) init
{
    [locker lock];
    QMacCocoaAutoReleasePool pool;
    center = [NSNotificationCenter defaultCenter];
    currentInterface = [CWInterface interface];
//    [center addObserver:self selector:@selector(notificationHandler:) name:kCWLinkDidChangeNotification object:nil];
    [center addObserver:self selector:@selector(notificationHandler:) name:kCWPowerDidChangeNotification object:nil];
    [locker unlock];
    return self;
}

-(void)dealloc
{
    [super dealloc];
}

-(void)setEngine:(QCoreWlanEngine *)coreEngine
{
    [locker lock];
    if(!engine)
        engine = coreEngine;
    [locker unlock];
}

-(void)remove
{
    [locker lock];
    [center removeObserver:self];
    [locker unlock];
}

- (void)notificationHandler//:(NSNotification *)notification
{
    engine->requestUpdate();
}
@end

QNSListener *listener = 0;

QT_BEGIN_NAMESPACE


static QString qGetInterfaceType(const QString &interfaceString)
{
    return networkInterfaces.value(interfaceString, QLatin1String("WLAN"));
}

void networkChangeCallback(SCDynamicStoreRef/* store*/, CFArrayRef changedKeys, void *info)
{
    for ( long i = 0; i < CFArrayGetCount(changedKeys); i++) {

        QString changed =  QCFString::toQString((CFStringRef)CFArrayGetValueAtIndex(changedKeys, i));
        if( changed.contains("/Network/Global/IPv4")) {
            QCoreWlanEngine* wlanEngine = static_cast<QCoreWlanEngine*>(info);
            wlanEngine->requestUpdate();
        }
    }
    return;
}

QCoreWlanEngine::QCoreWlanEngine(QObject *parent)
:   QBearerEngineImpl(parent)
{
    startNetworkChangeLoop();

    QMacCocoaAutoReleasePool pool;
    if([[CWInterface supportedInterfaces] count] > 0 && !listener) {
        listener = [[QNSListener alloc] init];
        listener.engine = this;
        hasWifi = true;
    } else {
        hasWifi = false;
    }
    requestUpdate();
}

QCoreWlanEngine::~QCoreWlanEngine()
{
    while (!foundConfigurations.isEmpty())
        delete foundConfigurations.takeFirst();
    [listener remove];
    [listener release];
}

QString QCoreWlanEngine::getInterfaceFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    return configurationInterface.value(id);
}

bool QCoreWlanEngine::hasIdentifier(const QString &id)
{
    QMutexLocker locker(&mutex);

    return configurationInterface.contains(id);
}

void QCoreWlanEngine::connectToId(const QString &id)
{
    QMutexLocker locker(&mutex);
    QMacCocoaAutoReleasePool pool;
    QString interfaceString = getInterfaceFromId(id);

    if(networkInterfaces.value(interfaceString) == "WLAN") {
        CWInterface *wifiInterface = [CWInterface interfaceWithName: qt_mac_QStringToNSString(interfaceString)];

        if([wifiInterface power]) {
            NSError *err = nil;
            NSMutableDictionary *params = [NSMutableDictionary dictionaryWithCapacity:0];

            NSString *wantedSsid = 0;
            bool okToProceed = true;
            if(getNetworkNameFromSsid(id) != id) {
                NSArray *array = [CW8021XProfile allUser8021XProfiles];
                for (NSUInteger i=0; i<[array count]; ++i) {
                    const QString idCheck = QString::number(qHash(QLatin1String("corewlan:") + qt_mac_NSStringToQString([[array objectAtIndex:i] userDefinedName])));
                    const QString idCheck2 = QString::number(qHash(QLatin1String("corewlan:") + qt_mac_NSStringToQString([[array objectAtIndex:i] ssid])));

                    if(id == idCheck
                        || id == idCheck2) {
                        QString thisName = getSsidFromNetworkName(id);
                        if(thisName.isEmpty()) {
                            wantedSsid = qt_mac_QStringToNSString(id);
                        } else {
                            wantedSsid = qt_mac_QStringToNSString(thisName);
                        }
                        okToProceed = false;
                        [params setValue: [array objectAtIndex:i] forKey:kCWAssocKey8021XProfile];
                        break;
                    }
                }
            }

            if(okToProceed) {
                NSUInteger index = 0;

                CWConfiguration *userConfig = [ wifiInterface configuration];
                NSSet *remNets = [userConfig rememberedNetworks];
                NSEnumerator *enumerator = [remNets objectEnumerator];
                CWWirelessProfile *wProfile;

                while ((wProfile = [enumerator nextObject])) {
                    const QString idCheck = QString::number(qHash(QLatin1String("corewlan:") + qt_mac_NSStringToQString([wProfile ssid])));

                    if(id == idCheck) {
                        wantedSsid = [wProfile ssid];
                        [params setValue: [wProfile passphrase] forKey: kCWAssocKeyPassphrase];
                        break;
                    }
                    index++;
                }
            }

            NSDictionary *parametersDict = nil;
            NSArray *apArray = [NSMutableArray arrayWithArray:[wifiInterface scanForNetworksWithParameters:parametersDict error:&err]];

            if(!err) {
                for(uint row=0; row < [apArray count]; row++ ) {
                    CWNetwork *apNetwork = [apArray objectAtIndex:row];
                    if([[apNetwork ssid] compare:wantedSsid] == NSOrderedSame) {
                        bool result = [wifiInterface associateToNetwork: apNetwork parameters:[NSDictionary dictionaryWithDictionary:params] error:&err];

                        if(!result) {
                            emit connectionError(id, ConnectError);
                        } else {
                            return;
                        }
                    }
                }
            } else {
                qDebug() <<"ERROR"<<  qt_mac_NSStringToQString([err localizedDescription ]);
            }

            emit connectionError(id, InterfaceLookupError);
        } else {
            // not wifi
        }
        locker.unlock();
        emit connectionError(id, InterfaceLookupError);
        locker.relock();
    }
    emit connectionError(id, OperationNotSupported);
}

void QCoreWlanEngine::disconnectFromId(const QString &id)
{
    QMutexLocker locker(&mutex);

    QString interfaceString = getInterfaceFromId(id);
    if(networkInterfaces.value(getInterfaceFromId(id)) == "WLAN") { //wifi only for now
#if defined(MAC_SDK_10_6)
        QMacCocoaAutoReleasePool pool;
        CWInterface *wifiInterface = [CWInterface interfaceWithName:  qt_mac_QStringToNSString(interfaceString)];
        [wifiInterface disassociate];
        if([[wifiInterface interfaceState]intValue] != kCWInterfaceStateInactive) {
            locker.unlock();
            emit connectionError(id, DisconnectionError);
            locker.relock();
        }
       return;
#endif
    } else {

    }

    locker.unlock();
    emit connectionError(id, OperationNotSupported);
}

void QCoreWlanEngine::requestUpdate()
{
    getUserConfigurations();
    doRequestUpdate();
}

void QCoreWlanEngine::doRequestUpdate()
{
    QMutexLocker locker(&mutex);

    getWifiInterfaces();

    QStringList previous = accessPointConfigurations.keys();

    QMapIterator<QString, QString> i(networkInterfaces);
    while (i.hasNext()) {
        i.next();
        if (i.value() == QLatin1String("WLAN")) {
            QStringList added = scanForSsids(i.key());
            while (!added.isEmpty()) {
                previous.removeAll(added.takeFirst());
            }
        }

        QNetworkInterface interface = QNetworkInterface::interfaceFromName(i.key());

        if (!interface.isValid())
            continue;

        uint identifier;
        if (interface.index())
            identifier = qHash(QLatin1String("corewlan:") + QString::number(interface.index()));
        else
            identifier = qHash(QLatin1String("corewlan:") + interface.hardwareAddress());

        const QString id = QString::number(identifier);

        previous.removeAll(id);

        QString name = interface.humanReadableName();
        if (name.isEmpty())
            name = interface.name();

        QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;

        if (interface.flags() && QNetworkInterface::IsRunning)
            state = QNetworkConfiguration::Defined;

        if (!interface.addressEntries().isEmpty())
            state = QNetworkConfiguration::Active;

        if (accessPointConfigurations.contains(id)) { //handle only scanned AP's
            QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

            bool changed = false;

            ptr->mutex.lock();

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

            ptr->mutex.unlock();

            if (changed) {
                locker.unlock();
                emit configurationChanged(ptr);
                locker.relock();
            }
        }
    }

    while (!previous.isEmpty()) {
        QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.take(previous.takeFirst());

        configurationInterface.remove(ptr->id);

        locker.unlock();
        emit configurationRemoved(ptr);
        locker.relock();
    }

    locker.unlock();
    emit updateCompleted();
}

QStringList QCoreWlanEngine::scanForSsids(const QString &interfaceName)
{
    QMutexLocker locker(&mutex);

    QStringList found;

    if(!hasWifi) {
        return found;
    }
    QMacCocoaAutoReleasePool pool;

    CWInterface *currentInterface = [CWInterface interfaceWithName:qt_mac_QStringToNSString(interfaceName)];
    QStringList addedConfigs;

    if([currentInterface power]) {
        NSError *err = nil;
        NSDictionary *parametersDict = nil;
        NSArray* apArray = [currentInterface scanForNetworksWithParameters:parametersDict error:&err];
        CWNetwork *apNetwork;

        if (!err) {
            for(uint row=0; row < [apArray count]; row++ ) {

                apNetwork = [apArray objectAtIndex:row];

                const QString networkSsid = qt_mac_NSStringToQString([apNetwork ssid]);

                const QString id = QString::number(qHash(QLatin1String("corewlan:") + networkSsid));
                found.append(id);

                QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;
                bool known = isKnownSsid(networkSsid);
                if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                    if( networkSsid == qt_mac_NSStringToQString( [currentInterface ssid])) {
                        state = QNetworkConfiguration::Active;
                    }
                }
                if(state == QNetworkConfiguration::Undefined) {
                    if(known) {
                        state = QNetworkConfiguration::Discovered;
                    } else {
                        state = QNetworkConfiguration::Undefined;
                    }
                }

                if (accessPointConfigurations.contains(id)) {
                    QNetworkConfigurationPrivatePointer ptr = accessPointConfigurations.value(id);

                    bool changed = false;

                    ptr->mutex.lock();

                    if (!ptr->isValid) {
                        ptr->isValid = true;
                        changed = true;
                    }

                    if (ptr->name != networkSsid) {
                        ptr->name = networkSsid;
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

                    ptr->mutex.unlock();

                    if (changed) {
                        locker.unlock();
                        emit configurationChanged(ptr);
                        locker.relock();
                    }
                    addedConfigs << networkSsid;
                } else {
                    QNetworkConfigurationPrivatePointer ptr(new QNetworkConfigurationPrivate);

                    found.append(id);
                    ptr->name = networkSsid;
                    ptr->isValid = true;
                    ptr->id = id;
                    ptr->state = state;
                    ptr->type = QNetworkConfiguration::InternetAccessPoint;
                    ptr->bearer = QLatin1String("WLAN");

                    accessPointConfigurations.insert(ptr->id, ptr);
                    configurationInterface.insert(ptr->id, interfaceName);

                    locker.unlock();
                    emit configurationAdded(ptr);
                    locker.relock();
                    addedConfigs << networkSsid;
                }
            }
        }
    }

    // add known configurations that are not around.
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QString networkName = i.key();
        if(!addedConfigs.contains(networkName)) {
            QString interfaceName;
            QMapIterator<QString, QString> ij(i.value());
            while (ij.hasNext()) {
                ij.next();
                interfaceName = ij.value();
            }
            QNetworkConfigurationPrivatePointer ptr(new QNetworkConfigurationPrivate);
            const QString id = QString::number(qHash(QLatin1String("corewlan:") + networkName));
            found.append(id);

            ptr->name = networkName;
            ptr->isValid = true;
            ptr->id = id;
            ptr->bearer = QLatin1String("WLAN");
            ptr->type = QNetworkConfiguration::InternetAccessPoint;
            QString ssid = getSsidFromNetworkName(networkName);

            if( [currentInterface.interfaceState intValue] == kCWInterfaceStateRunning) {
                if( ssid == qt_mac_NSStringToQString([currentInterface ssid])) {
                    ptr->state = QNetworkConfiguration::Active;
                }
            }

            if(!ptr->state) {
                if( addedConfigs.contains(ssid)) {
                    ptr->state = QNetworkConfiguration::Discovered;
                }
            }

            if(!ptr->state) {
                ptr->state = QNetworkConfiguration::Defined;
            }
            accessPointConfigurations.insert(ptr->id, ptr);
            configurationInterface.insert(ptr->id, interfaceName);
        }
    }
    return found;
}

bool QCoreWlanEngine::isWifiReady(const QString &wifiDeviceName)
{
    QMutexLocker locker(&mutex);

    if(hasWifi) {
        QMacCocoaAutoReleasePool pool;
        CWInterface *defaultInterface = [CWInterface interfaceWithName: qt_mac_QStringToNSString(wifiDeviceName)];
        if([defaultInterface power])
            return true;
    }
    return false;
}

bool QCoreWlanEngine::isKnownSsid(const QString &ssid)
{
    QMutexLocker locker(&mutex);

    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        if(map.keys().contains(ssid)) {
            return true;
        }
    }
    return false;
}

bool QCoreWlanEngine::getWifiInterfaces()
{
    QMutexLocker locker(&mutex);

    networkInterfaces.clear();
    QMacCocoaAutoReleasePool pool;

    NSArray *wifiInterfaces = [CWInterface supportedInterfaces];
    for(uint row=0; row < [wifiInterfaces count]; row++ ) {
        networkInterfaces.insert( qt_mac_NSStringToQString([wifiInterfaces objectAtIndex:row]),"WLAN");
    }
    return true;
}

QNetworkSession::State QCoreWlanEngine::sessionStateForId(const QString &id)
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

QNetworkConfigurationManager::Capabilities QCoreWlanEngine::capabilities() const
{
    return QNetworkConfigurationManager::ForcedRoaming;
}

void QCoreWlanEngine::startNetworkChangeLoop()
{
    storeSession = NULL;

    SCDynamicStoreContext dynStoreContext = { 0, this/*(void *)storeSession*/, NULL, NULL, NULL };
    storeSession = SCDynamicStoreCreate(NULL,
                                 CFSTR("networkChangeCallback"),
                                 networkChangeCallback,
                                 &dynStoreContext);
    if (!storeSession ) {
        qWarning() << "could not open dynamic store: error:" << SCErrorString(SCError());
        return;
    }

    CFMutableArrayRef notificationKeys;
    notificationKeys = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    CFMutableArrayRef patternsArray;
    patternsArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

    CFStringRef storeKey;
    storeKey = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL,
                                                     kSCDynamicStoreDomainState,
                                                     kSCEntNetIPv4);
    CFArrayAppendValue(notificationKeys, storeKey);
    CFRelease(storeKey);

    storeKey = SCDynamicStoreKeyCreateNetworkServiceEntity(NULL,
                                                      kSCDynamicStoreDomainState,
                                                      kSCCompAnyRegex,
                                                      kSCEntNetIPv4);
    CFArrayAppendValue(patternsArray, storeKey);
    CFRelease(storeKey);

    if (!SCDynamicStoreSetNotificationKeys(storeSession , notificationKeys, patternsArray)) {
        qWarning() << "register notification error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        CFRelease(notificationKeys);
        CFRelease(patternsArray);
        return;
    }
    CFRelease(notificationKeys);
    CFRelease(patternsArray);

    runloopSource = SCDynamicStoreCreateRunLoopSource(NULL, storeSession , 0);
    if (!runloopSource) {
        qWarning() << "runloop source error:"<< SCErrorString(SCError());
        CFRelease(storeSession );
        return;
    }

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    return;
}

QNetworkSessionPrivate *QCoreWlanEngine::createSessionBackend()
{
    return new QNetworkSessionPrivateImpl;
}

QNetworkConfigurationPrivatePointer QCoreWlanEngine::defaultConfiguration()
{
    return QNetworkConfigurationPrivatePointer();
}

bool QCoreWlanEngine::requiresPolling() const
{
    return true;
}

QString QCoreWlanEngine::getSsidFromNetworkName(const QString &name)
{
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();

        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             const QString idCheck = QString::number(qHash(QLatin1String("corewlan:") +i.key()));
             if(name == i.key() || name == idCheck) {
                 return ij.key();
             }
         }
    }
    return QString();
}

QString QCoreWlanEngine::getNetworkNameFromSsid(const QString &ssid)
{
    QMapIterator<QString, QMap<QString,QString> > i(userProfiles);
    while (i.hasNext()) {
        i.next();
        QMap<QString,QString> map = i.value();
        QMapIterator<QString, QString> ij(i.value());
         while (ij.hasNext()) {
             ij.next();
             if(ij.key() == ssid) {
                 return i.key();
             }

         }
            return map.key(ssid);
    }
    return QString();
}


void QCoreWlanEngine::getUserConfigurations()
{
    QMacCocoaAutoReleasePool pool;
    userProfiles.clear();

    NSArray *wifiInterfaces = [CWInterface supportedInterfaces];
    for(uint row=0; row < [wifiInterfaces count]; row++ ) {

        CWInterface *wifiInterface = [CWInterface interfaceWithName: [wifiInterfaces objectAtIndex:row]];

// add user configured system networks
        SCDynamicStoreRef dynRef = SCDynamicStoreCreate(kCFAllocatorSystemDefault, (CFStringRef)@"Qt corewlan", nil, nil);
        CFDictionaryRef airportPlist = (const __CFDictionary*)SCDynamicStoreCopyValue(dynRef, (CFStringRef)[NSString stringWithFormat:@"Setup:/Network/Interface/%@/AirPort", [wifiInterface name]]);
        CFRelease(dynRef);

        NSDictionary *prefNetDict = [airportPlist objectForKey:@"PreferredNetworks"];

        NSArray *thisSsidarray = [prefNetDict valueForKey:@"SSID_STR"];
        for(NSString *ssidkey in thisSsidarray) {
            QString thisSsid = qt_mac_NSStringToQString(ssidkey);
            if(!userProfiles.contains(thisSsid)) {
                QMap <QString,QString> map;
                map.insert(thisSsid, qt_mac_NSStringToQString([wifiInterface name]));
                userProfiles.insert(thisSsid, map);
            }
        }
        CFRelease(airportPlist);

        // 802.1X user profiles
        QString userProfilePath = QDir::homePath() + "/Library/Preferences/com.apple.eap.profiles.plist";
        NSDictionary* eapDict = [[NSMutableDictionary alloc] initWithContentsOfFile:qt_mac_QStringToNSString(userProfilePath)];
        NSString *profileStr= @"Profiles";
        NSString *nameStr = @"UserDefinedName";
        NSString *networkSsidStr = @"Wireless Network";
        for (id profileKey in eapDict) {
            if ([profileStr isEqualToString:profileKey]) {
                NSDictionary *itemDict = [eapDict objectForKey:profileKey];
                for (id itemKey in itemDict) {

                    NSInteger dictSize = [itemKey count];
                    id objects[dictSize];
                    id keys[dictSize];

                    [itemKey getObjects:objects andKeys:keys];
                    QString networkName;
                    QString ssid;
                    for(int i = 0; i < dictSize; i++) {
                        if([nameStr isEqualToString:keys[i]]) {
                            networkName = qt_mac_NSStringToQString(objects[i]);
                        }
                        if([networkSsidStr isEqualToString:keys[i]]) {
                            ssid = qt_mac_NSStringToQString(objects[i]);
                        }
                        if(!userProfiles.contains(networkName)
                            && !ssid.isEmpty()) {
                            QMap<QString,QString> map;
                            map.insert(ssid, qt_mac_NSStringToQString([wifiInterface name]));
                            userProfiles.insert(networkName, map);
                        }
                    }
                }
            }
        }
    }
}

QT_END_NAMESPACE
