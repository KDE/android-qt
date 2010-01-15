/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "qiodevicedelegate_p.h"
#include "quriloader_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

URILoader::URILoader(QObject *const parent,
                     const NamePool::Ptr &np,
                     const VariableLoader::Ptr &l) : QNetworkAccessManager(parent)
                                                   , m_variableNS(QLatin1String("tag:trolltech.com,2007:QtXmlPatterns:QIODeviceVariable:"))
                                                   , m_namePool(np)
                                                   , m_variableLoader(l)
{
    Q_ASSERT(m_variableLoader);
}

QNetworkReply *URILoader::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
    const QString requestedUrl(req.url().toString());

    /* We got a QIODevice variable. */
    const QString name(requestedUrl.right(requestedUrl.length() - m_variableNS.length()));

    const QVariant variant(m_variableLoader->valueFor(m_namePool->allocateQName(QString(), name, QString())));

    if(!variant.isNull() && variant.userType() == qMetaTypeId<QIODevice *>())
        return new QIODeviceDelegate(qVariantValue<QIODevice *>(variant));
    else
    {
        /* If we're entering this code path, the variable URI identified a variable
         * which we don't have, which means we either have a bug, or the user had
         * crafted an invalid URI manually. */

        return QNetworkAccessManager::createRequest(op, req, outgoingData);
    }
}

QT_END_NAMESPACE

