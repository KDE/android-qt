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

#include "qdynamiccontext_p.h"
#include "qvalidationerror_p.h"
#include "qitem_p.h"

#include "qanyuri_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AnyURI::AnyURI(const QString &s) : AtomicString(s)
{
}

AnyURI::Ptr AnyURI::fromValue(const QString &value)
{
    return AnyURI::Ptr(new AnyURI(value));
}

AnyURI::Ptr AnyURI::fromValue(const QUrl &uri)
{
    return AnyURI::Ptr(new AnyURI(uri.toString()));
}

AnyURI::Ptr AnyURI::resolveURI(const QString &relative,
                               const QString &base)
{
    const QUrl urlBase(base);
    return AnyURI::fromValue(urlBase.resolved(relative).toString());
}

ItemType::Ptr AnyURI::type() const
{
    return BuiltinTypes::xsAnyURI;
}

AnyURI::Ptr AnyURI::fromLexical(const QString &value)
{
    bool isValid;
    /* The error code doesn't matter, because we never raise error. */
    const QUrl retval(toQUrl<ReportContext::FORG0001>(value,
                                                      DynamicContext::Ptr(),
                                                      0,
                                                      &isValid,
                                                      false));
    if(isValid)
        return fromValue(retval);
    else
        return ValidationError::createError();
}

bool AnyURI::isValid(const QString &candidate)
{
    bool isOk = false;
    /* The error code doesn't matter, because we never raise error. */
    toQUrl<ReportContext::FORG0001>(candidate,
                                    ReportContext::Ptr(),
                                    0,
                                    &isOk,
                                    false);
    return isOk;
}
QT_END_NAMESPACE
