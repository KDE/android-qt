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

#ifndef Patternist_TruthPredicate_H
#define Patternist_TruthPredicate_H

#include "qgenericpredicate_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A predicate which is optimized for filter expressions that
     * are of type @c xs:boolean.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     */
    class TruthPredicate : public GenericPredicate
    {
    public:
        /**
         * Creates a TruthPredicate which filters the items from the @p sourceExpression
         * through @p predicate.
         *
         * This constructor is protected. The proper way to create predicates is via the static
         * create() function.
         */
        TruthPredicate(const Expression::Ptr &sourceExpression,
                       const Expression::Ptr &predicate);

        inline Item mapToItem(const Item &item, const DynamicContext::Ptr &context) const
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "This is practically dead code because it never gets called in GenericPredicate, "
                                           "which binds to its own mapToItem for completely legitime reasons.");
            if(m_operand2->evaluateEBV(context))
                return item;
            else
                return Item();
        }

        inline Item::Iterator::Ptr map(const Item &item,
                                       const DynamicContext::Ptr &context) const
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "I don't expect this function to be called, for the same reasons as above.");
            if(m_operand2->evaluateEBV(context))
                return makeSingletonIterator(item);
            else
                return CommonValues::emptyIterator;
        }

        virtual SequenceType::List expectedOperandTypes() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
