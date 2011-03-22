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
#include "qqnameconstructor_p.h"

#include "qsystempropertyfn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item SystemPropertyFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QString lexQName(m_operands.first()->evaluateSingleton(context).stringValue());

    const QXmlName name
        (QNameConstructor::expandQName<DynamicContext::Ptr,
                                       ReportContext::XTDE1390,
                                       ReportContext::XTDE1390>(lexQName,
                                                                context,
                                                                staticNamespaces(), this));

    return AtomicString::fromValue(retrieveProperty(name));
}

QString SystemPropertyFN::retrieveProperty(const QXmlName name)
{
    if(name.namespaceURI() != StandardNamespaces::xslt)
        return QString();

    switch(name.localName())
    {
        case StandardLocalNames::version:
            /*
             * The supported XSL-T version.
             *
             * @see <a href="http://www.w3.org/TR/xslt20/#system-property">The Note paragraph
             * at the very end of XSL Transformations (XSLT) Version 2.0,
             * 16.6.5 system-property</a>
             */
            return QString::number(1.20);
        case StandardLocalNames::vendor:
            return QLatin1String("Nokia Corporation and/or its subsidiary(-ies), a Nokia Company");
        case StandardLocalNames::vendor_url:
            return QLatin1String("http://qt.nokia.com/");
        case StandardLocalNames::product_name:
            return QLatin1String("QtXmlPatterns");
        case StandardLocalNames::product_version:
            return QLatin1String("0.1");
        case StandardLocalNames::is_schema_aware:
        /* Fallthrough. */
        case StandardLocalNames::supports_backwards_compatibility:
        /* Fallthrough. */
        case StandardLocalNames::supports_serialization:
        /* Fallthrough. */
            return QLatin1String("no");
        default:
            return QString();
    }
}

QT_END_NAMESPACE
