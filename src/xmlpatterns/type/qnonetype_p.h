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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_NoneType_H
#define Patternist_NoneType_H

#include "qatomictype_p.h"
#include "qsequencetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Represents the special <tt>none</tt> type.
     *
     * @ingroup Patternist_types
     * @see <a href="http://www.w3.org/TR/xquery-semantics/#sec_content_models">XQuery 1.0 and
     * XPath 2.0 Formal Semantics, 2.4.3 Content models</a>
     * @see <a href="http://www.w3.org/TR/xquery-semantics/#sec_fnerror">XQuery 1.0 and XPath 2.0
     * Formal Semantics, 7.2.9 The fn:error function</a>
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class NoneType : public ItemType,
                     public SequenceType
    {
    public:
        typedef QExplicitlySharedDataPointer<NoneType> Ptr;

        virtual bool itemMatches(const Item &item) const;
        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;

        /**
         * @returns always "none". That is, no namespace prefix
         */
        virtual QString displayName(const NamePool::Ptr &np) const;

        /**
         * @note The semantical meaning of this type's item type can
         * surely be discussed. The function is provided due to
         * it being mandated by the SequenceType base class.
         *
         * @returns always 'this' since NoneType is also an ItemType
         */
        virtual ItemType::Ptr itemType() const;

        /**
         * @note The semantical meaning of this type's cardinality
         * can surely be discussed. The function is provided due to
         * it being mandated by the SequenceType base class.
         *
         * @returns always Cardinality::zeroOrMore()
         */
        virtual Cardinality cardinality() const;

        /**
         * @returns always @c false
         */
        virtual bool isAtomicType() const;

        /**
         * This can be thought to be a weird function for this type(none). There
         * is no atomized type for none, perhaps the best from a conceptual perspective
         * would be to return @c null.
         *
         * This function returns BuiltinTypes::xsAnyAtomicType because
         * the generic type checking code inserts an Atomizer in the AST
         * when an error() function(or other node which has type none) is part of
         * an operator expression(value/general comparison, arithmetics). The Atomizer
         * returns the atomizedType() of its child, and by here returning xsAnyAtomicType,
         * static operator lookup is postponed to runtime. Subsequently, expressions like error()
         * works properly with other XPath expressions.
         */
        virtual ItemType::Ptr atomizedType() const;

        /**
         * @returns always @c false
         */
        virtual bool isNodeType() const;

        /**
         * @returns always item()
         */
        virtual ItemType::Ptr xdtSuperType() const;

        /**
         * @returns always @p other. The none type can be thought as
         * disappearing when attempting to find the union of it and
         * another type.
         */
        virtual const ItemType &operator|(const ItemType &other) const;

    protected:

        friend class CommonSequenceTypes;
        NoneType();
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
