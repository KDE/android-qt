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

#ifndef Patternist_XsdSchemaTypesFactory_H
#define Patternist_XsdSchemaTypesFactory_H

#include <QtCore/QHash>
#include "qschematypefactory_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Factory for creating schema types for the types defined in XSD.
     *
     * @ingroup Patternist_types
     * @author Tobias Koenig <tobias.koenig@nokia.com>
     */
    class XsdSchemaTypesFactory : public SchemaTypeFactory
    {
        public:
            /**
             * Creates a new schema type factory.
             *
             * @param namePool The name pool all type names belong to.
             */
            XsdSchemaTypesFactory(const NamePool::Ptr &namePool);

            /**
             * Creates a primitive type for @p name. If @p name is not supported,
             * @c null is returned.
             *
             * @note This does not handle user defined types, only builtin types.
             */
            virtual SchemaType::Ptr createSchemaType(const QXmlName) const;

            /**
             * Returns a hash of all available types.
             */
            virtual SchemaType::Hash types() const;

        private:
            /**
             * A dictonary of all predefined schema types.
             */
            SchemaType::Hash               m_types;

            NamePool::Ptr                  m_namePool;
            mutable SchemaTypeFactory::Ptr m_basicTypesFactory;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
