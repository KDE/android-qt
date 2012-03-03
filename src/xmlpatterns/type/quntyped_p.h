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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_Untyped_H
#define Patternist_Untyped_H

#include "qanytype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class AtomicType;

    /**
     * @short Represents the complex W3C XML Schema type <tt>xs:untyped</tt>.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <frans.englich@nokia.com>
     * @see <a href="http://www.w3.org/TR/xpath-datamodel/#types-predefined">XQuery 1.0 and XPath 2.0
     * Data Model, 2.6.2 Predefined Types</a>
     */
    class Untyped : public AnyType
    {
    public:

        typedef QExplicitlySharedDataPointer<Untyped> Ptr;

        /**
         * @returns always BuiltinTypes::xsAnyType.
         */
        virtual SchemaType::Ptr wxsSuperType() const;

        virtual QXmlName name(const NamePool::Ptr &np) const;

        /**
         * @returns always <tt>xs:untypedAtomic</tt>
         */
        virtual ItemType::Ptr atomizedType() const;

        /**
         * @returns always SchemaType::ComplexType
         */
        virtual TypeCategory category() const;

        /**
         * @returns always NoDerivation
         */
        virtual DerivationMethod derivationMethod() const;

    protected:
        friend class BuiltinTypes;

        Untyped();
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
