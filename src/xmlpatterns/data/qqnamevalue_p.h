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

#ifndef Patternist_QNameValue_H
#define Patternist_QNameValue_H

#include "qitem_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the value instance of the @c xs:QName type.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_xdm
     * @see QNameConstructor::expandQName()
     * @see XPathHelper::isQName()
     * @see QXmlUtils
     */
    class QNameValue : public AtomicValue
    {
    public:
        friend class CommonValues;
        friend class QNameComparator;

        typedef QExplicitlySharedDataPointer<QNameValue> Ptr;

        /**
         * Constructs a QNameValue that represents @p name.
         *
         * @param name the QName. May not be @c null.
         * @param np the NamePool.
         * @see QNameConstructor::expandQName()
         * @see XPathHelper::isQName()
         * @see QXmlUtils
         */
        static QNameValue::Ptr fromValue(const NamePool::Ptr &np, const QXmlName name);

        virtual QString stringValue() const;

        virtual ItemType::Ptr type() const;

        inline QXmlName qName() const
        {
            return m_qName;
        }

    private:
        QNameValue(const NamePool::Ptr &np, const QXmlName name);

        const QXmlName         m_qName;
        const NamePool::Ptr m_namePool;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
