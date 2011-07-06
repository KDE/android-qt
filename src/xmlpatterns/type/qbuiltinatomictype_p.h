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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_BuiltinAtomicType_H
#define Patternist_BuiltinAtomicType_H

#include "qatomictype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Instances of this class represents types that are sub-classes
     * of @c xs:anyAtomicType.
     *
     * Retrieving instances of builtin types is done
     * via BuiltinTypesFactory::createSchemaType(), not by instantiating this
     * class manually.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class BuiltinAtomicType : public AtomicType
    {
    public:

        typedef QExplicitlySharedDataPointer<BuiltinAtomicType> Ptr;

        /**
         * @returns always @c false
         */
        virtual bool isAbstract() const;

        /**
         * @returns the base type as specified in the constructors baseType argument.
         */
        virtual SchemaType::Ptr wxsSuperType() const;

        /**
         * @returns the same type as wxsSuperType(), except for the type @c xs:anyAtomicType, which
         * returns item()
         */
        virtual ItemType::Ptr xdtSuperType() const;

        virtual AtomicComparatorLocator::Ptr comparatorLocator() const;
        virtual AtomicMathematicianLocator::Ptr mathematicianLocator() const;
        virtual AtomicCasterLocator::Ptr casterLocator() const;

    protected:
        friend class BuiltinTypes;

        /**
         * @param baseType the type that is the super type of the constructed
         * atomic type. In the case of AnyAtomicType, @c null is passed.
         * @param comp the AtomicComparatorLocator this type should return. May be @c null.
         * @param mather similar to @p comp, this is the AtomicMathematicianLocator
         * that's appropriate for this type May be @c null.
         * @param casterLocator the CasterLocator that locates classes performing
         * casting with this type. May be @c null.
         */
        BuiltinAtomicType(const AtomicType::Ptr &baseType,
                          const AtomicComparatorLocator::Ptr &comp,
                          const AtomicMathematicianLocator::Ptr &mather,
                          const AtomicCasterLocator::Ptr &casterLocator);

    private:
        const AtomicType::Ptr                   m_superType;
        const AtomicComparatorLocator::Ptr      m_comparatorLocator;
        const AtomicMathematicianLocator::Ptr   m_mathematicianLocator;
        const AtomicCasterLocator::Ptr          m_casterLocator;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
