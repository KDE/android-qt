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

#ifndef Patternist_CallTargetDescription_H
#define Patternist_CallTargetDescription_H

template<typename Key, typename Value> class QHash;
template<typename T> class QList;

#include <QSharedData>

#include "qexpression_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class CallSite;

    /**
     * @short Contains metadata for a callable component, such as a function or
     * template.
     *
     * CallTargetDescription can be used directly and is so for templates, but
     * can also be sub-classed which FunctionSignature do.
     *
     * @ingroup Patternist_expr
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class Q_AUTOTEST_EXPORT CallTargetDescription : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<CallTargetDescription> Ptr;
        typedef QList<Ptr> List;

        CallTargetDescription(const QXmlName &name);

        /**
         * The function's name. For example, the name of the signature
         * <tt>fn:string() as xs:string</tt> is <tt>fn:string</tt>.
         */
        QXmlName name() const;

        /**
         * Flags callsites to be aware of their recursion by calling
         * UserFunctionCallsite::configureRecursion(), if that is the case.
         *
         * @note We pass @p expr by value here intentionally.
         */
        static void checkCallsiteCircularity(CallTargetDescription::List &signList,
                                             const Expression::Ptr expr);
    private:
        /**
         * Helper function for checkCallsiteCircularity(). If C++ allowed it,
         * it would have been local to it.
         */
        static void checkArgumentsCircularity(CallTargetDescription::List &signList,
                                              const Expression::Ptr callsite);

        Q_DISABLE_COPY(CallTargetDescription)
        const QXmlName m_name;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

