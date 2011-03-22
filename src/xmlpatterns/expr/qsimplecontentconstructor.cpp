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
#include "qatomicstring_p.h"

#include "qsimplecontentconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SimpleContentConstructor::SimpleContentConstructor(const Expression::Ptr &operand) : SingleContainer(operand)
{
}

Item SimpleContentConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
    Item next(it->next());
    QString result;

    if(next)
    {
        result = next.stringValue();
        next = it->next();
    }
    else
        return Item();

    while(next)
    {
        result += QLatin1Char(' ');
        result += next.stringValue();
        next = it->next();
    }

    return AtomicString::fromValue(result);
}

Expression::Ptr SimpleContentConstructor::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    if(me.data() == this)
    {
        /* Optimization: if we will evaluate to a single string, we're not
         * necessary. */
        if(CommonSequenceTypes::ExactlyOneString->matches(m_operand->staticType()))
            return m_operand;
    }

    return me;
}

SequenceType::List SimpleContentConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreAtomicTypes);
    return result;
}

SequenceType::Ptr SimpleContentConstructor::staticType() const
{
    if(m_operand->staticType()->cardinality().allowsEmpty())
        return CommonSequenceTypes::ZeroOrOneString;
    else
        return CommonSequenceTypes::ExactlyOneString;
}

ExpressionVisitorResult::Ptr SimpleContentConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
