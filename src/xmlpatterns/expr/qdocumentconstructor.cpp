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
#include "qdocumentcontentvalidator_p.h"
#include "qnodebuilder_p.h"

#include "qdocumentconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DocumentConstructor::DocumentConstructor(const Expression::Ptr &op) : SingleContainer(op)
{
}

Item DocumentConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(m_staticBaseURI));

    DocumentContentValidator validator(nodeBuilder.data(), context, ConstPtr(this));
    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    validator.startDocument();
    m_operand->evaluateToSequenceReceiver(receiverContext);
    validator.endDocument();

    const QAbstractXmlNodeModel::Ptr nm(nodeBuilder->builtDocument());
    context->addNodeModel(nm);

    return nm->root(QXmlNodeModelIndex());
}

void DocumentConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    QAbstractXmlReceiver *const receiver = context->outputReceiver();

    DocumentContentValidator validator(receiver, context, ConstPtr(this));

    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    validator.startDocument();
    m_operand->evaluateToSequenceReceiver(receiverContext);
    validator.endDocument();
}

Expression::Ptr DocumentConstructor::typeCheck(const StaticContext::Ptr &context,
                                               const SequenceType::Ptr &reqType)
{
    m_staticBaseURI = context->baseURI();
    return SingleContainer::typeCheck(context, reqType);
}

SequenceType::Ptr DocumentConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneDocumentNode;
}

SequenceType::List DocumentConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

Expression::Properties DocumentConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr
DocumentConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
