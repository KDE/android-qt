/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include <QUrl>

#include <QNetworkAccessManager>

#include "qnetworkaccessdelegator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NetworkAccessDelegator::NetworkAccessDelegator(QNetworkAccessManager *const genericManager,
                                               QNetworkAccessManager *const variableURIManager) : m_genericManager(genericManager)
                                                                                                , m_variableURIManager(variableURIManager)
{
}

QNetworkAccessManager *NetworkAccessDelegator::managerFor(const QUrl &uri)
{
    /* Unfortunately we have to do it this way, QUrl::isParentOf() doesn't
     * understand URI schemes like this one. */
    const QString requestedUrl(uri.toString());

    /* On the topic of timeouts:
     *
     * Currently the schemes QNetworkAccessManager handles should/will do
     * timeouts for 4.4, but we need to do timeouts for our own. */
    if(requestedUrl.startsWith(QLatin1String("tag:trolltech.com,2007:QtXmlPatterns:QIODeviceVariable:")))
        return m_variableURIManager;
    else
    {
        if(!m_genericManager)
            m_genericManager = new QNetworkAccessManager(this);

        return m_genericManager;
    }
}

QT_END_NAMESPACE

