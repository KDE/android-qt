/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qitemmappingiterator_p.h"

#include "qquantifiedexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QuantifiedExpression::QuantifiedExpression(const VariableSlotID varSlot,
                                           const Operator quantifier,
                                           const Expression::Ptr &inClause,
                                           const Expression::Ptr &testExpression)
                                           : PairContainer(inClause, testExpression),
                                             m_varSlot(varSlot),
                                             m_quantifier(quantifier)
{
    Q_ASSERT(quantifier == Some || quantifier == Every);
}

Item QuantifiedExpression::mapToItem(const Item &item,
                                          const DynamicContext::Ptr &context) const
{
    context->setRangeVariable(m_varSlot, item);
    return item;
}

bool QuantifiedExpression::evaluateEBV(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(makeItemMappingIterator<Item>(ConstPtr(this),
                                                               m_operand1->evaluateSequence(context),
                                                               context));

    Item item(it->next());

    if(m_quantifier == Some)
    {
        while(item)
        {
            if(m_operand2->evaluateEBV(context))
                return true;
            else
                item = it->next();
        };

        return false;
    }
    else
    {
        Q_ASSERT(m_quantifier == Every);

        while(item)
        {
            if(m_operand2->evaluateEBV(context))
                item = it->next();
            else
                return false;
        }

        return true;
    }
}

QString QuantifiedExpression::displayName(const Operator quantifier)
{
    if(quantifier == Some)
        return QLatin1String("some");
    else
    {
        Q_ASSERT(quantifier == Every);
        return QLatin1String("every");
    }
}

SequenceType::Ptr QuantifiedExpression::staticType() const
{
    return CommonSequenceTypes::ExactlyOneBoolean;
}

SequenceType::List QuantifiedExpression::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::EBV);
    return result;
}

QuantifiedExpression::Operator QuantifiedExpression::operatorID() const
{
    return m_quantifier;
}

ExpressionVisitorResult::Ptr QuantifiedExpression::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
