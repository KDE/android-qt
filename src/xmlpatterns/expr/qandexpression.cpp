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

#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"

#include "qandexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AndExpression::AndExpression(const Expression::Ptr &operand1,
                             const Expression::Ptr &operand2) : PairContainer(operand1, operand2)
{
}

bool AndExpression::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operand1->evaluateEBV(context) && m_operand2->evaluateEBV(context);
}

Expression::Ptr AndExpression::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr newMe(PairContainer::compress(context));

    if(newMe != this)
        return newMe;

    /* Both operands mustn't be evaluated in order to be able to compress. */
    if(m_operand1->isEvaluated() && !m_operand1->evaluateEBV(context->dynamicContext()))
        return wrapLiteral(CommonValues::BooleanFalse, context, this);
    else if(m_operand2->isEvaluated() && !m_operand2->evaluateEBV(context->dynamicContext()))
        return wrapLiteral(CommonValues::BooleanFalse, context, this);
    else
        return Expression::Ptr(this);
}

SequenceType::List AndExpression::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::EBV);
    result.append(CommonSequenceTypes::EBV);
    return result;
}

SequenceType::Ptr AndExpression::staticType() const
{
    return CommonSequenceTypes::ExactlyOneBoolean;
}

ExpressionVisitorResult::Ptr AndExpression::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
