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

#ifndef Patternist_LocalNameTest_H
#define Patternist_LocalNameTest_H

#include "qabstractnodetest_p.h"

template<typename Key, typename Value> class QHash;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A name test that is of the type <tt>*:local-name</tt>.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class LocalNameTest : public AbstractNodeTest
    {
    public:
        typedef QHash<QString, ItemType::Ptr> Hash;

        static ItemType::Ptr create(const ItemType::Ptr &primaryType, const QXmlName::LocalNameCode localName);

        /**
         * @note This function assumes that @p item is a QXmlNodeModelIndex.
         */
        virtual bool itemMatches(const Item &item) const;

        virtual QString displayName(const NamePool::Ptr &np) const;

        virtual bool operator==(const ItemType &other) const;
        virtual PatternPriority patternPriority() const;

    protected:
        virtual InstanceOf instanceOf() const;

    private:
        LocalNameTest(const ItemType::Ptr &primaryType, const QXmlName::LocalNameCode &ncName);

        const QXmlName::LocalNameCode m_ncName;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
