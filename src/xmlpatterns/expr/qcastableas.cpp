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

#include "qatomictype_p.h"
#include "qitem_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"

#include "qcastableas_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

CastableAs::CastableAs(const Expression::Ptr &operand,
                       const SequenceType::Ptr &tType) : SingleContainer(operand),
                                                         m_targetType(tType)
{
    Q_ASSERT(tType);
    Q_ASSERT(!tType->cardinality().allowsMany());
    Q_ASSERT(tType->itemType()->isAtomicType());
}

bool CastableAs::evaluateEBV(const DynamicContext::Ptr &context) const
{
    Item item;

    if(m_operand->staticType()->cardinality().allowsMany())
    {
        const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
        item = it->next();

        if(it->next())
            return false;
    }
    else
        item = m_operand->evaluateSingleton(context);

    if(item)
        return !cast(item, context).as<AtomicValue>()->hasError();
    else
        return m_targetType->cardinality().allowsEmpty();
}

Expression::Ptr CastableAs::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    if(me != this) /* We already managed to const fold, how convenient. */
        return me;

    const AtomicType::Ptr t(m_targetType->itemType());

    const SequenceType::Ptr opType(m_operand->staticType());

    /* Casting to these always succeeds, assuming the cardinality also matches,
     * although the cardinality can fail. */
    if((   *t == *BuiltinTypes::xsString
        || *t == *BuiltinTypes::xsUntypedAtomic
        || *t == *opType->itemType())
       &&(m_targetType->cardinality().isMatch(opType->cardinality())))
    {
        return wrapLiteral(CommonValues::BooleanTrue, context, this);
    }
    else
        return me;
}

Expression::Ptr CastableAs::typeCheck(const StaticContext::Ptr &context,
                                      const SequenceType::Ptr &reqType)
{
    checkTargetType(context);
    const Expression::Ptr me(SingleContainer::typeCheck(context, reqType));

    return me;
    if(BuiltinTypes::xsQName->xdtTypeMatches(m_targetType->itemType()))
    {
        const SequenceType::Ptr seqt(m_operand->staticType());
        /* We can cast a string literal, an xs:QName value, and an
         * empty sequence(if empty is allowed), to xs:QName. */
        if(m_operand->is(IDStringValue) ||
           BuiltinTypes::xsQName->xdtTypeMatches(seqt->itemType()) ||
           (*seqt->itemType() == *CommonSequenceTypes::Empty && m_targetType->cardinality().allowsEmpty()))
        {
            return wrapLiteral(CommonValues::BooleanTrue, context, this)->typeCheck(context, reqType);
        }
        else
            return wrapLiteral(CommonValues::BooleanFalse, context, this)->typeCheck(context, reqType);
    }
    else
    {
        /* Let the CastingPlatform look up its AtomicCaster. */
        prepareCasting(context, m_operand->staticType()->itemType());

        return me;
    }
}

SequenceType::List CastableAs::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreAtomicTypes);
    return result;
}

SequenceType::Ptr CastableAs::staticType() const
{
    return CommonSequenceTypes::ExactlyOneBoolean;
}

ExpressionVisitorResult::Ptr CastableAs::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
