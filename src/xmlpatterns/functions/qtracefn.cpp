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

#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qitemmappingiterator_p.h"
#include "qpatternistlocale_p.h"

#include "qtracefn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

namespace QPatternist
{
    /**
     * @short TraceCallback is a MappingCallback and takes care of
     * the tracing of each individual item.
     *
     * Because Patternist must be thread safe, TraceFN creates a TraceCallback
     * each time the function is evaluated. In other words, TraceFN, which is
     * an Expression sub class, can't modify its members, but MappingCallback
     * does not have this limitation since it's created on a per evaluation basis.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class TraceCallback : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<TraceCallback> Ptr;

        inline TraceCallback(const QString &msg) : m_position(0),
                                                   m_msg(msg)
        {
        }

        /**
         * Performs the actual tracing.
         */
        Item mapToItem(const Item &item,
                            const DynamicContext::Ptr &context)
        {
            QTextStream out(stderr);
            ++m_position;
            if(m_position == 1)
            {
                if(item)
                {
                    out << qPrintable(m_msg)
                        << " : "
                        << qPrintable(item.stringValue());
                }
                else
                {
                    out << qPrintable(m_msg)
                        << " : ("
                        << qPrintable(formatType(context->namePool(), CommonSequenceTypes::Empty))
                        << ")\n";
                    return Item();
                }
            }
            else
            {
                out << qPrintable(item.stringValue())
                    << '['
                    << m_position
                    << "]\n";
            }

            return item;
        }

    private:
        xsInteger m_position;
        const QString m_msg;
    };
}

Item::Iterator::Ptr TraceFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const QString msg(m_operands.last()->evaluateSingleton(context).stringValue());

    return makeItemMappingIterator<Item>(TraceCallback::Ptr(new TraceCallback(msg)),
                                              m_operands.first()->evaluateSequence(context),
                                              context);
}

Item TraceFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QString msg(m_operands.last()->evaluateSingleton(context).stringValue());
    const Item item(m_operands.first()->evaluateSingleton(context));

    return TraceCallback::Ptr(new TraceCallback(msg))->mapToItem(item, context);
}

SequenceType::Ptr TraceFN::staticType() const
{
    return m_operands.first()->staticType();
}

QT_END_NAMESPACE
