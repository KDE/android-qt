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

#include <QList>

#include "qtriplecontainer_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

TripleContainer::TripleContainer(const Expression::Ptr &operand1,
                                 const Expression::Ptr &operand2,
                                 const Expression::Ptr &operand3) : m_operand1(operand1),
                                                                    m_operand2(operand2),
                                                                    m_operand3(operand3)
{
    Q_ASSERT(operand1);
    Q_ASSERT(operand2);
    Q_ASSERT(operand3);
}

Expression::List TripleContainer::operands() const
{
    Expression::List result;
    result.append(m_operand1);
    result.append(m_operand2);
    result.append(m_operand3);
    return result;
}

void TripleContainer::setOperands(const Expression::List &ops)
{
    Q_ASSERT(ops.count() == 3);
    m_operand1 = ops.first();
    m_operand2 = ops.at(1);
    m_operand3 = ops.at(2);
}

bool TripleContainer::compressOperands(const StaticContext::Ptr &context)
{
    rewrite(m_operand1, m_operand1->compress(context), context);
    rewrite(m_operand2, m_operand2->compress(context), context);
    rewrite(m_operand3, m_operand3->compress(context), context);

    return m_operand1->isEvaluated() && m_operand2->isEvaluated() && m_operand3->isEvaluated();
}

QT_END_NAMESPACE
