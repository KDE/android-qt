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

#include <QUrl>

#include "qcommonsequencetypes_p.h"
#include "qnodebuilder_p.h"

#include "qtextnodeconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

TextNodeConstructor::TextNodeConstructor(const Expression::Ptr &op) : SingleContainer(op)
{
}

Item TextNodeConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item chars(m_operand->evaluateSingleton(context));

    if(!chars)
        return Item();

    const NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(QUrl()));
    const QString &v = chars.stringValue();
    nodeBuilder->characters(QStringRef(&v));

    const QAbstractXmlNodeModel::Ptr nm(nodeBuilder->builtDocument());
    context->addNodeModel(nm);

    return nm->root(QXmlNodeModelIndex());
}

void TextNodeConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    const Item item(m_operand->evaluateSingleton(context));

    QAbstractXmlReceiver *const receiver = context->outputReceiver();

    if(item)
    {
        const QString &v = item.stringValue();
        receiver->characters(QStringRef(&v));
    }
    else
        receiver->characters(QStringRef());
}

SequenceType::Ptr TextNodeConstructor::staticType() const
{
    if(m_operand->staticType()->cardinality().allowsEmpty())
        return CommonSequenceTypes::ZeroOrOneTextNode;
    else
        return CommonSequenceTypes::ExactlyOneTextNode;
}

SequenceType::List TextNodeConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrOneString);
    return result;
}

Expression::Properties TextNodeConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr
TextNodeConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
