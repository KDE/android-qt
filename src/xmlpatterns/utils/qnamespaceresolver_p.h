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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_NamespaceResolver_H
#define Patternist_NamespaceResolver_H

#include <QSharedData>
#include <QXmlName>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template<typename A, typename B> class QHash;

namespace QPatternist
{

    /**
     * @short Base class for namespace resolvers.
     *
     * @ingroup Patternist
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class NamespaceResolver : public QSharedData
    {
    public:
        enum Constants
        {
            NoBinding = -1
        };

        typedef QExplicitlySharedDataPointer<NamespaceResolver> Ptr;

        /**
         * A list of namespace bindings. The key is the prefix, and the value is
         * the namespace URI.
         */
        typedef QHash<QXmlName::PrefixCode, QXmlName::NamespaceCode> Bindings;

        NamespaceResolver();
        virtual ~NamespaceResolver();

        /**
         * Adds the mapping from @p prefix to @p namespaceURI to
         * this NamespaceResolver. If this NamespaceResolver already contains
         * a binding involving @p prefix, the old binding is replaced.
         */
        virtual void addBinding(const QXmlName nb) = 0;

        /**
         * Resolves the @p prefix to the corresponding namespace URI. If no binding
         * exists for @p prefix, NoBinding is returned.
         *
         * @returns the namespace corresponding to @p prefix.
         */
        virtual QXmlName::NamespaceCode lookupNamespaceURI(const QXmlName::PrefixCode prefix) const = 0;

        /**
         * @returns all bindings this NamespaceResolver handles.
         */
        virtual Bindings bindings() const = 0;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
