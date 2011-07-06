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

#include "qarithmeticexpression_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"
#include "qschemanumeric_p.h"

#include "qunaryexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

UnaryExpression::UnaryExpression(const AtomicMathematician::Operator op,
                                 const Expression::Ptr &operand,
                                 const StaticContext::Ptr &context) : ArithmeticExpression(wrapLiteral(CommonValues::IntegerZero, context, operand.data()),
                                                                                           op,
                                                                                           operand)
{
    Q_ASSERT(op == AtomicMathematician::Substract ||
             op == AtomicMathematician::Add);
    Q_ASSERT(context);
}

Item UnaryExpression::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    if(operatorID() == AtomicMathematician::Substract)
    {
        const Item item(m_operand2->evaluateSingleton(context));

        if(item)
            return item.as<Numeric>()->toNegated();
        else
            return Item();
    }
    else
        return m_operand2->evaluateSingleton(context);
}

QT_END_NAMESPACE
