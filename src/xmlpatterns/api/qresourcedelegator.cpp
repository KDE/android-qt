/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qresourcedelegator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool ResourceDelegator::isUnparsedTextAvailable(const QUrl &uri,
                                                const QString &encoding)
{
    return m_parentLoader->isUnparsedTextAvailable(uri, encoding);
}

ItemType::Ptr ResourceDelegator::announceUnparsedText(const QUrl &uri)
{
    return m_parentLoader->announceUnparsedText(uri);
}

Item ResourceDelegator::openUnparsedText(const QUrl &uri,
                                         const QString &encoding,
                                         const ReportContext::Ptr &context,
                                         const SourceLocationReflection *const where)
{
    return m_parentLoader->openUnparsedText(uri, encoding, context, where);
}

Item ResourceDelegator::openDocument(const QUrl &uri,
                                     const ReportContext::Ptr &context)
{
    if(m_needsOverride.contains(uri))
        return m_forDeviceLoader->openDocument(uri, context);
    else
        return m_parentLoader->openDocument(uri, context);
}

SequenceType::Ptr ResourceDelegator::announceDocument(const QUrl &uri, const Usage usageHint)
{
    return m_parentLoader->announceDocument(uri, usageHint);
}

bool ResourceDelegator::isDocumentAvailable(const QUrl &uri)
{
    return m_parentLoader->isDocumentAvailable(uri);
}

Item::Iterator::Ptr ResourceDelegator::openCollection(const QUrl &uri)
{
    return m_parentLoader->openCollection(uri);
}

SequenceType::Ptr ResourceDelegator::announceCollection(const QUrl &uri)
{
    return m_parentLoader->announceCollection(uri);
}

QSet<QUrl> ResourceDelegator::deviceURIs() const
{
    QSet<QUrl> uris(m_needsOverride);
    uris |= m_forDeviceLoader->deviceURIs();
    return uris;
}

QT_END_NAMESPACE
