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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_OptimizationBlocks_H
#define Patternist_OptimizationBlocks_H

#include "qatomiccomparator_p.h"
#include "qexpression_p.h"
#include "qoptimizerframework_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains a set of common OptimizerPass instances.
     *
     * @author Frans englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     */
    namespace OptimizationPasses
    {
        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>count([expr]) ne 0</tt> into <tt>exists([expr])</tt>
         * - <tt>count([expr]) != 0</tt> into <tt>exists([expr])</tt>
         * - <tt>0 ne count([expr])</tt> into <tt>exists([expr])</tt>
         * - <tt>0 != count([expr])</tt> into <tt>exists([expr])</tt>
         * - <tt>count([expr]) eq 0</tt> into <tt>empty([expr])</tt>
         * - <tt>count([expr]) = 0</tt> into <tt>empty([expr])</tt>
         * - <tt>0 eq count([expr])</tt> into <tt>empty([expr])</tt>
         * - <tt>0 = count([expr])</tt> into <tt>empty([expr])</tt>
         * - <tt>count([expr]) ge 1</tt> into <tt>exists([expr])</tt>
         * - <tt>count([expr]) >= 1</tt> into <tt>exists([expr])</tt>
         */
        extern OptimizationPass::List comparisonPasses;

        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>for $var in [expr] return $var</tt> into <tt>[expr]</tt>
         */
        extern OptimizationPass::List forPasses;

        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>if([expr of type xs:boolean]) then true() else false()</tt>
         *   into <tt>[expr of type xs:boolean]</tt>
         */
        extern OptimizationPass::List ifThenPasses;

        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>fn:not(fn:exists([expr]))</tt> into <tt>fn:empty([expr])</tt>
         * - <tt>fn:not(fn:empty([expr]))</tt> into <tt>fn:exists([expr])</tt>
         */
        extern OptimizationPass::List notFN;

        /**
         * Initializes the data members in the OptimizationPasses namespace.
         *
         * This class is not supposed to be instantiated, but to be used via its init()
         * function. In fact, this class cannot be instantiated.
         *
         * @author Frans englich <frans.englich@nokia.com>
         */
        class Coordinator
        {
        public:
            /**
             * Initializes the members in the OptimizationPasses namespace.
             */
            static void init();

        private:
            Q_DISABLE_COPY(Coordinator)
            inline Coordinator();
        };
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
