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

#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"
#include "qoptimizationpasses_p.h"

#include "qifthenclause_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

IfThenClause::IfThenClause(const Expression::Ptr &test,
                           const Expression::Ptr &then,
                           const Expression::Ptr &el) : TripleContainer(test, then, el)
{
}

Item::Iterator::Ptr IfThenClause::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context)
            ? m_operand2->evaluateSequence(context)
            : m_operand3->evaluateSequence(context);
}

Item IfThenClause::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context)
            ? m_operand2->evaluateSingleton(context)
            : m_operand3->evaluateSingleton(context);
}

bool IfThenClause::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context)
            ? m_operand2->evaluateEBV(context)
            : m_operand3->evaluateEBV(context);
}

void IfThenClause::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    if(m_operand1->evaluateEBV(context))
        m_operand2->evaluateToSequenceReceiver(context);
    else
        m_operand3->evaluateToSequenceReceiver(context);
}

Expression::Ptr IfThenClause::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(TripleContainer::compress(context));

    if(me != this)
        return me;

    /* All operands mustn't be evaluated in order for const folding to
     * be possible. Let's see how far we get. */

    if(m_operand1->isEvaluated())
    {
        if(m_operand1->evaluateEBV(context->dynamicContext()))
            return m_operand2;
        else
            return m_operand3;
    }
    else
        return me;
}

QList<QExplicitlySharedDataPointer<OptimizationPass> > IfThenClause::optimizationPasses() const
{
    return OptimizationPasses::ifThenPasses;
}

SequenceType::List IfThenClause::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::EBV);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr IfThenClause::staticType() const
{
    const SequenceType::Ptr t1(m_operand2->staticType());
    const SequenceType::Ptr t2(m_operand3->staticType());

    return makeGenericSequenceType(t1->itemType() | t2->itemType(),
                                   t1->cardinality() | t2->cardinality());
}

ExpressionVisitorResult::Ptr IfThenClause::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID IfThenClause::id() const
{
    return IDIfThenClause;
}

/*
Expression::Properties IfThenClause::properties() const
{
    return   m_operand1->properties()
           | m_operand2->properties()
           | m_operand3->properties()
           & (  Expression::RequiresFocus
              | Expression::IsEvaluated
              | Expression::DisableElimination);
}
*/
QT_END_NAMESPACE
