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
#include "qcommonnamespaces_p.h"

#include "qbasictypesfactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SchemaTypeFactory::Ptr BasicTypesFactory::self(const NamePool::Ptr &np)
{
    /* We don't store a global static here, because it's dependent on the NamePool. */
    return SchemaTypeFactory::Ptr(new BasicTypesFactory(np));
}

BasicTypesFactory::BasicTypesFactory(const NamePool::Ptr &np)
{
    m_types.reserve(48);

#define add(aName)   m_types.insert(BuiltinTypes::aName->name(np), AtomicType::Ptr(BuiltinTypes::aName))
#define addNA(aName) m_types.insert(BuiltinTypes::aName->name(np), BuiltinTypes::aName)
    add(xsString);
    add(xsBoolean);
    add(xsDecimal);
    add(xsDouble);
    add(xsFloat);
    add(xsDate);
    add(xsTime);
    add(xsDateTime);
    add(xsDuration);
    add(xsAnyURI);
    add(xsGDay);
    add(xsGMonthDay);
    add(xsGMonth);
    add(xsGYearMonth);
    add(xsGYear);
    add(xsBase64Binary);
    add(xsHexBinary);
    add(xsQName);
    add(xsInteger);
    addNA(xsAnyType);
    addNA(xsAnySimpleType);
    add(xsYearMonthDuration);
    add(xsDayTimeDuration);
    add(xsAnyAtomicType);
    addNA(xsUntyped);
    add(xsUntypedAtomic);
    add(xsNOTATION);
    /* Add derived primitives. */
    add(xsNonPositiveInteger);
    add(xsNegativeInteger);
    add(xsLong);
    add(xsInt);
    add(xsShort);
    add(xsByte);
    add(xsNonNegativeInteger);
    add(xsUnsignedLong);
    add(xsUnsignedInt);
    add(xsUnsignedShort);
    add(xsUnsignedByte);
    add(xsPositiveInteger);
    add(xsNormalizedString);
    add(xsToken);
    add(xsLanguage);
    add(xsNMTOKEN);
    add(xsName);
    add(xsNCName);
    add(xsID);
    add(xsIDREF);
    add(xsENTITY);

#undef add
#undef addNA
}

SchemaType::Ptr BasicTypesFactory::createSchemaType(const QXmlName name) const
{
    return m_types.value(name);
}

SchemaType::Hash BasicTypesFactory::types() const
{
    return m_types;
}

QT_END_NAMESPACE
