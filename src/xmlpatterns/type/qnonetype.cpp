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

#include "qbuiltintypes_p.h"

#include "qnonetype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NoneType::NoneType()
{
}

bool NoneType::itemMatches(const Item &) const
{
    return false;
}

bool NoneType::xdtTypeMatches(const ItemType::Ptr &t) const
{
    return *this == *t;
}

const ItemType &NoneType::operator|(const ItemType &other) const
{
    return other;
}

QString NoneType::displayName(const NamePool::Ptr &) const
{
    return QLatin1String("none");
}

Cardinality NoneType::cardinality() const
{
    return Cardinality::zeroOrMore();
}

ItemType::Ptr NoneType::itemType() const
{
    return ItemType::Ptr(const_cast<NoneType *>(this));
}

bool NoneType::isAtomicType() const
{
    return false;
}

bool NoneType::isNodeType() const
{
    return false;
}

ItemType::Ptr NoneType::atomizedType() const
{
    return BuiltinTypes::xsAnyAtomicType;
}

ItemType::Ptr NoneType::xdtSuperType() const
{
    return BuiltinTypes::item;
}

QT_END_NAMESPACE
