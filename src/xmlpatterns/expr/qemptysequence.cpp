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

#include "qcommonvalues_p.h"
#include "qcommonsequencetypes_p.h"
#include "qemptysequencetype_p.h"

#include "qemptysequence_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr EmptySequence::create(const Expression *const replacementFor,
                                      const StaticContext::Ptr &context)
{
    Q_ASSERT(replacementFor);
    Q_ASSERT(context);

    const Expression::Ptr retval(new EmptySequence());
    context->wrapExpressionWith(replacementFor, retval);
    return retval;
}

Item::Iterator::Ptr EmptySequence::evaluateSequence(const DynamicContext::Ptr &) const
{
    return CommonValues::emptyIterator;
}

Item EmptySequence::evaluateSingleton(const DynamicContext::Ptr &) const
{
    return Item();
}

void EmptySequence::evaluateToSequenceReceiver(const DynamicContext::Ptr &) const
{
}

ItemType::Ptr EmptySequence::type() const
{
    return CommonSequenceTypes::Empty;
}

SequenceType::Ptr EmptySequence::staticType() const
{
    return CommonSequenceTypes::Empty;
}

bool EmptySequence::evaluateEBV(const DynamicContext::Ptr &) const
{
    return false;
}

QString EmptySequence::stringValue() const
{
    return QString();
}

ExpressionVisitorResult::Ptr EmptySequence::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID EmptySequence::id() const
{
    return IDEmptySequence;
}

Expression::Properties EmptySequence::properties() const
{
    return IsEvaluated;
}

QT_END_NAMESPACE
