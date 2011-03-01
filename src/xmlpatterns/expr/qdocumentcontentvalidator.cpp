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

#include "qpatternistlocale_p.h"

#include "qdocumentcontentvalidator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DocumentContentValidator::
DocumentContentValidator(QAbstractXmlReceiver *const receiver,
                         const DynamicContext::Ptr &context,
                         const Expression::ConstPtr &expr) : m_receiver(receiver)
                                                           , m_context(context)
                                                           , m_expr(expr)
                                                           , m_elementDepth(0)
{
    Q_ASSERT(receiver);
    Q_ASSERT(m_expr);
    Q_ASSERT(context);
}

void DocumentContentValidator::namespaceBinding(const QXmlName &nb)
{
    m_receiver->namespaceBinding(nb);
}

void DocumentContentValidator::startElement(const QXmlName &name)
{
    ++m_elementDepth;
    m_receiver->startElement(name);
}

void DocumentContentValidator::endElement()
{
    Q_ASSERT(m_elementDepth > 0);
    --m_elementDepth;
    m_receiver->endElement();
}

void DocumentContentValidator::attribute(const QXmlName &name,
                                         const QStringRef &value)
{
    if(m_elementDepth == 0)
    {
        m_context->error(QtXmlPatterns::tr("An attribute node cannot be a "
                                           "child of a document node. "
                                           "Therefore, the attribute %1 "
                                           "is out of place.")
                         .arg(formatKeyword(m_context->namePool(), name)),
                         ReportContext::XPTY0004, m_expr.data());
    }
    else
        m_receiver->attribute(name, value);
}

void DocumentContentValidator::comment(const QString &value)
{
    m_receiver->comment(value);
}

void DocumentContentValidator::characters(const QStringRef &value)
{
    m_receiver->characters(value);
}

void DocumentContentValidator::processingInstruction(const QXmlName &name,
                                                     const QString &value)
{
    m_receiver->processingInstruction(name, value);
}

void DocumentContentValidator::item(const Item &outputItem)
{
    /* We can't send outputItem directly to m_receiver since its item() function
     * won't dispatch to this DocumentContentValidator, but to itself. We're not sub-classing here,
     * we're delegating. */

    if(outputItem.isNode())
        sendAsNode(outputItem);
    else
        m_receiver->item(outputItem);
}

void DocumentContentValidator::startDocument()
{
    m_receiver->startDocument();
}

void DocumentContentValidator::endDocument()
{
    m_receiver->endDocument();
}

void DocumentContentValidator::atomicValue(const QVariant &value)
{
    Q_UNUSED(value);
}

void DocumentContentValidator::startOfSequence()
{
}

void DocumentContentValidator::endOfSequence()
{
}

QT_END_NAMESPACE
