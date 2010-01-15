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

#include <QHash>

#include "qbuiltintypes_p.h"
#include "qitem_p.h"
#include "qitem_p.h"

#include "qnamespacenametest_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NamespaceNameTest::NamespaceNameTest(const ItemType::Ptr &primaryType,
                                     const QXmlName::NamespaceCode namespaceURI) : AbstractNodeTest(primaryType),
                                                                                m_namespaceURI(namespaceURI)
{
}

ItemType::Ptr NamespaceNameTest::create(const ItemType::Ptr &primaryType, const QXmlName::NamespaceCode namespaceURI)
{
    Q_ASSERT(primaryType);

    return ItemType::Ptr(new NamespaceNameTest(primaryType, namespaceURI));
}

bool NamespaceNameTest::itemMatches(const Item &item) const
{
    Q_ASSERT(item.isNode());
    return m_primaryType->itemMatches(item) &&
           item.asNode().name().namespaceURI() == m_namespaceURI;
}

QString NamespaceNameTest::displayName(const NamePool::Ptr &np) const
{
    return QLatin1Char('{') + np->stringForNamespace(m_namespaceURI) + QLatin1String("}:*");
}

ItemType::InstanceOf NamespaceNameTest::instanceOf() const
{
    return ClassNamespaceNameTest;
}

bool NamespaceNameTest::operator==(const ItemType &other) const
{
    return other.instanceOf() == ClassNamespaceNameTest &&
           static_cast<const NamespaceNameTest &>(other).m_namespaceURI == m_namespaceURI;
}

PatternPriority NamespaceNameTest::patternPriority() const
{
    return -0.25;
}

QT_END_NAMESPACE
