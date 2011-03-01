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

#include "qatomicstring_p.h"
#include "qcommonsequencetypes_p.h"

#include "qxsltsimplecontentconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XSLTSimpleContentConstructor::XSLTSimpleContentConstructor(const Expression::Ptr &source) : SimpleContentConstructor(source)
{
}

QString XSLTSimpleContentConstructor::processItem(const Item &item,
                                                  bool &discard,
                                                  bool &isText)
{
    if(item.isNode())
    {
        isText = (item.asNode().kind() == QXmlNodeModelIndex::Text);

        if(isText)
        {
            const QString value(item.stringValue());

            /* "1. Zero-length text nodes in the sequence are discarded." */
            discard = value.isEmpty();
            return value;
        }
        else
        {
            Item::Iterator::Ptr it(item.sequencedTypedValue()); /* Atomic values. */
            Item next(it->next());
            QString result;

            if(next)
                result = next.stringValue();

            next = it->next();

            while(next)
            {
                result += next.stringValue();
                result += QLatin1Char(' ');
                next = it->next();
            }

            return result;
        }
    }
    else
    {
        discard = false;
        isText = false;
        return item.stringValue();
    }
}

Item XSLTSimpleContentConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));

    Item next(it->next());
    QString result;

    bool previousIsText = false;
    bool discard = false;

    if(next)
    {
        const QString unit(processItem(next, discard, previousIsText));

        if(!discard)
            result = unit;

        next = it->next();
    }
    else
        return Item();

    while(next)
    {
        bool currentIsText = false;
        const QString unit(processItem(next, discard, currentIsText));

        if(!discard)
        {
            /* "Adjacent text nodes in the sequence are merged into a single text
             * node." */
            if(previousIsText && currentIsText)
                ;
            else
                result += QLatin1Char(' ');

            result += unit;
        }

        next = it->next();
        previousIsText = currentIsText;
    }

    return AtomicString::fromValue(result);
}

SequenceType::List XSLTSimpleContentConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr XSLTSimpleContentConstructor::staticType() const
{
    return CommonSequenceTypes::ZeroOrOneString;
}

QT_END_NAMESPACE
