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

#include "qanyuri_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qpatternistlocale_p.h"
#include "private/qxmlutils_p.h"

#include "qcomputednamespaceconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ComputedNamespaceConstructor::ComputedNamespaceConstructor(const Expression::Ptr &prefix,
                                                           const Expression::Ptr &namespaceURI) : PairContainer(prefix, namespaceURI)
{
}

void ComputedNamespaceConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    const Item prefixItem(m_operand1->evaluateSingleton(context));
    const QString prefix(prefixItem ? prefixItem.stringValue() : QString());

    const Item namespaceItem(m_operand2->evaluateSingleton(context));
    const QString namespaceURI(namespaceItem ? namespaceItem.stringValue() : QString());

    if(namespaceURI.isEmpty())
    {
        context->error(QtXmlPatterns::tr("In a namespace constructor, the value for a namespace cannot be an empty string."),
                       ReportContext::XTDE0930,
                       this);
    }

    /* One optimization could be to store a pointer to
     * the name pool as a member in order to avoid the virtual call(s). */
    const NamePool::Ptr np(context->namePool());

    if(!prefix.isEmpty() && !QXmlUtils::isNCName(prefix))
    {
        context->error(QtXmlPatterns::tr("The prefix must be a valid %1, which %2 is not.")
                                        .arg(formatType(np, BuiltinTypes::xsNCName),
                                             formatKeyword(prefix)),
                       ReportContext::XTDE0920,
                       this);
    }
    const QXmlName binding(np->allocateBinding(prefix, namespaceURI));

    AnyURI::toQUrl<ReportContext::XTDE0905, DynamicContext::Ptr>(namespaceURI,
                                                                 context,
                                                                 this);

    if(binding.prefix() == StandardPrefixes::xmlns)
    {
        context->error(QtXmlPatterns::tr("The prefix %1 cannot be bound.")
                                        .arg(formatKeyword(prefix)),
                       ReportContext::XTDE0920,
                       this);
    }

    if((binding.prefix() == StandardPrefixes::xml && binding.namespaceURI() != StandardNamespaces::xml)
       ||
       (binding.prefix() != StandardPrefixes::xml && binding.namespaceURI() == StandardNamespaces::xml))
    {
        context->error(QtXmlPatterns::tr("Only the prefix %1 can be bound to %2 and vice versa.")
                                        .arg(formatKeyword(prefix), formatKeyword(namespaceURI)),
                       ReportContext::XTDE0925,
                       this);
    }

     context->outputReceiver()->namespaceBinding(binding);
}

SequenceType::Ptr ComputedNamespaceConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneAttribute;
}

SequenceType::List ComputedNamespaceConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrOneString);
    result.append(CommonSequenceTypes::ZeroOrOneString);
    return result;
}

Expression::Properties ComputedNamespaceConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr ComputedNamespaceConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
