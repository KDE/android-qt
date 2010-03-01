/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qnetworkconfiguration.h"

#include "qnetworkconfiguration_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QNetworkConfiguration

    \brief The QNetworkConfiguration class provides an abstraction of one or more access point configurations.

    \since 4.7

    \inmodule QtNetwork
    \ingroup bearer

    QNetworkConfiguration encapsulates a single access point or service network.
    In most cases a single access point configuration can be mapped to one network
    interface. However a single network interface may not always map to only one
    access point configuration. Multiple configurations for the same
    network device may enable multiple access points. An example
    device that could exhibit such a configuration might be a
    Smartphone which allows the user to manage multiple WLAN
    configurations while the device itself has only one WLAN network device.

    The QNetworkConfiguration also supports the concept of service networks.
    This concept allows the grouping of multiple access point configurations
    into one entity. Such a group is called service network and can be
    beneficial in cases whereby a network session to a
    particular destination network is required (e.g. a company network).
    When using a service network the user doesn't usually care which one of the
    connectivity options is chosen (e.g. corporate WLAN or VPN via GPRS)
    as long as he can reach the company's target server. Depending
    on the current position and time some of the access points that make
    up the service network may not even be available. Furthermore
    automated access point roaming can be enabled which enables the device
    to change the network interface configuration dynamically while maintaining
    the applications connection to the target network. It allows adaption
    to the changing environment and may enable optimization with regards to
    cost, speed or other network parameters.

    Special configurations of type UserChoice provide a placeholder configuration which is
    resolved to an actual network configuration by the platform when a
    \l {QNetworkSession}{session} is \l {QNetworkSession::open()}{opened}. Not all platforms
    support the concept of a user choice configuration.

    \section1 Configuration states

    The list of available configurations can be obtained via
    QNetworkConfigurationManager::allConfigurations(). A configuration can have
    multiple states. The \l Defined configuration state indicates that the configuration
    is stored on the device. However the configuration is not yet ready to be activated
    as e.g. a WLAN may not be available at the current time.

    The \l Discovered state implies that the configuration is \l Defined and
    the outside conditions are such that the configuration can be used immediately
    to open a new network session. An example of such an outside condition may be
    that the Ethernet cable is actually connected to the device or that the WLAN
    with the specified SSID is in range.

    The \l Active state implies that the configuration is \l Discovered. A configuration
    in this state is currently being used by an application. The underlying network
    interface has a valid IP configuration and can transfer IP packets between the
    device and the target network.

    The \l Undefined state indicates that the system has knowledge of possible target
    networks but cannot actually use that knowledge to connect to it. An example
    for such a state could be an encrypted WLAN that has been discovered
    but the user hasn't actually saved a configuration including the required password
    which would allow the device to connect to it.

    Depending on the type of configuration some states are transient in nature. A GPRS/UMTS
    connection may almost always be \l Discovered if the GSM/UMTS network is available.
    However if the GSM/UMTS network looses the connection the associated configuration may change its state
    from \l Discovered to \l Defined as well. A similar use case might be triggered by
    WLAN availability. QNetworkConfigurationManager::updateConfigurations() can be used to
    manually trigger updates of states. Note that some platforms do not require such updates
    as they implicitly change the state once it has been discovered. If the state of a
    configuration changes all related QNetworkConfiguration instances change their state automatically.

    \sa QNetworkSession, QNetworkConfigurationManager
*/

/*!
    \enum QNetworkConfiguration::Type

    This enum describes the type of configuration.

    \value InternetAccessPoint  The configuration specifies the details for a single access point.
                                Note that configurations of type InternetAccessPoint may be part
                                of other QNetworkConfigurations of type ServiceNetwork.
    \value ServiceNetwork       The configuration is based on a group of QNetworkConfigurations of
                                type InternetAccessPoint. All group members can reach the same
                                target network. This type of configuration is a mandatory
                                requirement for roaming enabled network sessions. On some
                                platforms this form of configuration may also be called Service
                                Network Access Point (SNAP).
    \value UserChoice           The configuration is a placeholder which will be resolved to an
                                actual configuration by the platform when a session is opened. Depending
                                on the platform the selection may generate a popup dialog asking the user
                                for his preferred choice.
    \value Invalid              The configuration is invalid.
*/

/*!
    \enum QNetworkConfiguration::StateFlag

    Specifies the configuration states.

    \value Undefined    This state is used for transient configurations such as newly discovered
                        WLANs for which the user has not actually created a configuration yet.
    \value Defined      Defined configurations are known to the system but are not immediately
                        usable (e.g. a configured WLAN is not within range or the Ethernet cable
                        is currently not plugged into the machine).
    \value Discovered   A discovered configuration can be immediately used to create a new
                        QNetworkSession. An example of a discovered configuration could be a WLAN
                        which is within in range. If the device moves out of range the discovered
                        flag is dropped. A second example is a GPRS configuration which generally
                        remains discovered for as long as the device has network coverage. A
                        configuration that has this state is also in state
                        QNetworkConfiguration::Defined. If the configuration is a service network
                        this flag is set if at least one of the underlying access points
                        configurations has the Discovered state.
    \value Active       The configuration is currently used by an open network session
                        (see \l QNetworkSession::isOpen()). However this does not mean that the
                        current process is the entity that created the open session. It merely
                        indicates that if a new QNetworkSession were to be constructed based on
                        this configuration \l QNetworkSession::state() would return
                        \l QNetworkSession::Connected. This state implies the
                        QNetworkConfiguration::Discovered state.
*/

/*!
    \enum QNetworkConfiguration::Purpose

    Specifies the purpose of the configuration.

    \value UnknownPurpose           The configuration doesn't specify any purpose. This is the default value.
    \value PublicPurpose            The configuration can be used for general purpose internet access.
    \value PrivatePurpose           The configuration is suitable to access a private network such as an office Intranet.
    \value ServiceSpecificPurpose   The configuration can be used for operator specific services (e.g.
                                    receiving MMS messages or content streaming).
*/

/*!
    Constructs an invalid configuration object.

    \sa isValid()
*/
QNetworkConfiguration::QNetworkConfiguration()
    : d(0)
{
}

/*!
    Creates a copy of the QNetworkConfiguration object contained in \a other.
*/
QNetworkConfiguration::QNetworkConfiguration(const QNetworkConfiguration& other)
    : d(other.d)
{
}

/*!
    Copies the content of the QNetworkConfiguration object contained in \a other into this one.
*/
QNetworkConfiguration& QNetworkConfiguration::operator=(const QNetworkConfiguration& other)
{
    d = other.d;
    return *this;
}

/*!
    Frees the resources associated with the QNetworkConfiguration object.
*/
QNetworkConfiguration::~QNetworkConfiguration()
{
}

/*!
    Returns true, if this configuration is the same as the \a other
    configuration given; otherwise returns false.
*/
bool QNetworkConfiguration::operator==(const QNetworkConfiguration& other) const
{
    if (!d)
        return !other.d;

    if (!other.d)
        return false;

    return (d == other.d);
}

/*!
    \fn bool QNetworkConfiguration::operator!=(const QNetworkConfiguration& other) const

    Returns true if this configuration is not the same as the \a other
    configuration given; otherwise returns false.
*/

/*!
    Returns the user visible name of this configuration.

    The name may either be the name of the underlying access point or the
    name for service network that this configuration represents.
*/
QString QNetworkConfiguration::name() const
{
    return d ? d->name : QString();
}

/*!
    Returns the unique and platform specific identifier for this network configuration;
    otherwise an empty string.
*/
QString QNetworkConfiguration::identifier() const
{
    return d ? d->id : QString();
}

/*!
    Returns the type of the configuration.

    A configuration can represent a single access point configuration or
    a set of access point configurations. Such a set is called service network.
    A configuration that is based on a service network can potentially support
    roaming of network sessions.
*/
QNetworkConfiguration::Type QNetworkConfiguration::type() const
{
    return d ? d->type : QNetworkConfiguration::Invalid;
}

/*!
    Returns true if this QNetworkConfiguration object is valid.
    A configuration may become invalid if the user deletes the configuration or
    the configuration was default-constructed.

    The addition and removal of configurations can be monitored via the
    QNetworkConfigurationManager.

    \sa QNetworkConfigurationManager
*/
bool QNetworkConfiguration::isValid() const
{
    return d ? d->isValid : false;
}

/*!
    Returns the current state of the configuration.
*/
QNetworkConfiguration::StateFlags QNetworkConfiguration::state() const
{
    return d ? d->state : QNetworkConfiguration::Undefined;
}

/*!
    Returns the purpose of this configuration.

    The purpose field may be used to programmatically determine the
    purpose of a configuration. Such information is usually part of the
    access point or service network meta data.
*/
QNetworkConfiguration::Purpose QNetworkConfiguration::purpose() const
{
    return d ? d->purpose : QNetworkConfiguration::UnknownPurpose;
}

/*!
    Returns true if this configuration supports roaming; otherwise false.
*/
bool QNetworkConfiguration::isRoamingAvailable() const
{
    return d ? d->roamingSupported : false;
}

/*!
    Returns all sub configurations of this network configuration.
    Only network configurations of type \l ServiceNetwork can have children. Otherwise
    this function returns an empty list.
*/
QList<QNetworkConfiguration> QNetworkConfiguration::children() const
{
    QList<QNetworkConfiguration> results;
    if (type() != QNetworkConfiguration::ServiceNetwork || !isValid() )
        return results;

    QMutableListIterator<QNetworkConfigurationPrivatePointer> iter(d->serviceNetworkMembers);
    while(iter.hasNext()) {
        QNetworkConfigurationPrivatePointer p = iter.next();
        //if we have an invalid member get rid of it -> was deleted earlier on
        if (!p->isValid)
            iter.remove();

        QNetworkConfiguration item;
        item.d = p;
        results << item;
    }

    return results;
}

/*!
    Returns the type of bearer. The string is not translated and
    therefore can not be shown to the user. The subsequent table presents the currently known
    bearer types:

    \table
        \header 
            \o Value
            \o Description
        \row
            \o Unknown
            \o The session is based on an unknown or unspecified bearer type.
        \row
            \o Ethernet
            \o The session is based on Ethernet.
        \row
            \o WLAN
            \o The session is based on Wireless LAN.
        \row
            \o 2G
            \o The session uses CSD, GPRS, HSCSD, EDGE or cdmaOne.
        \row 
            \o CDMA2000
            \o The session uses CDMA.
        \row
            \o WCDMA
            \o The session uses W-CDMA/UMTS.
        \row
            \o HSPA
            \o The session uses High Speed Packet Access.
        \row
            \o Bluetooth
            \o The session uses Bluetooth.
        \row
            \o WiMAX
            \o The session uses WiMAX.
    \endtable

    This function returns an empty string if this is an invalid configuration,
    a network configuration of type \l QNetworkConfiguration::ServiceNetwork or
    \l QNetworkConfiguration::UserChoice.
*/
QString QNetworkConfiguration::bearerName() const
{
    if (!isValid())
        return QString();

    return d->bearerName();
}


QT_END_NAMESPACE

