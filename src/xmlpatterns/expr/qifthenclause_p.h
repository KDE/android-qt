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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_IfThenClause_H
#define Patternist_IfThenClause_H

#include "qtriplecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements XPath 2.0's conditional expression <tt>if([expr]) then [expr] else [expr]</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-conditionals">XML Path Language (XPath) 2.0,
     * 3.8 Conditional Expressions</a>
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     */
    class IfThenClause : public TripleContainer
    {
    public:
        IfThenClause(const Expression::Ptr &test,
                     const Expression::Ptr &then,
                     const Expression::Ptr &el);

        virtual bool evaluateEBV(const DynamicContext::Ptr &context) const;
        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;

        virtual SequenceType::List expectedOperandTypes() const;
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        virtual SequenceType::Ptr staticType() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        /**
         * @returns IDIfThenClause
         */
        virtual ID id() const;
        virtual QList<QExplicitlySharedDataPointer<OptimizationPass> > optimizationPasses() const;
        //virtual Properties properties() const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
