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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_AnyNodeType_H
#define Patternist_AnyNodeType_H

#include "qatomictype_p.h"
#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Represents the <tt>node()</tt> item type.
     *
     * @ingroup Patternist_types
     * @see BuiltinNodeType
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class AnyNodeType : public ItemType
    {
    public:

        typedef QExplicitlySharedDataPointer<AnyNodeType> Ptr;

        virtual bool xdtTypeMatches(const ItemType::Ptr &other) const;
        virtual bool itemMatches(const Item &item) const;
        virtual QString displayName(const NamePool::Ptr &np) const;

        virtual ItemType::Ptr xdtSuperType() const;

        virtual bool isNodeType() const;
        virtual bool isAtomicType() const;

        /**
         * @see <a href="http://www.w3.org/TR/xpath-datamodel/#acc-summ-typed-value">XQuery 1.0
         * and XPath 2.0 Data Model, G.15 dm:typed-value Accessor</a>
         */
        virtual ItemType::Ptr atomizedType() const;

        /**
         * @returns the node kind this node ItemType tests for. If it matches any node, zero is returned.
         */
        virtual QXmlNodeModelIndex::NodeKind nodeKind() const;

        virtual PatternPriority patternPriority() const;

    protected:
        friend class BuiltinTypes;

        inline AnyNodeType()
        {
        }

    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
