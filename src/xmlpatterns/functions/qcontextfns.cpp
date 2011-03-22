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

#include "qanyuri_p.h"
#include "qdate_p.h"
#include "qschemadatetime_p.h"
#include "qdaytimeduration_p.h"
#include "qinteger_p.h"
#include "qliteral_p.h"
#include "qatomicstring_p.h"
#include "qschematime_p.h"

#include "qcontextfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item PositionFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(context);
    return Integer::fromValue(context->contextPosition());
}

Item LastFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(context);
    return Integer::fromValue(context->contextSize());
}

Item ImplicitTimezoneFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(context->implicitTimezone());
}

Item CurrentDateTimeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(DateTime::fromDateTime(context->currentDateTime()));
}

Item CurrentDateFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(Date::fromDateTime(context->currentDateTime()));
}

Item CurrentTimeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(SchemaTime::fromDateTime(context->currentDateTime()));
}

Expression::Ptr StaticBaseURIFN::typeCheck(const StaticContext::Ptr &context,
                                           const SequenceType::Ptr &reqType)
{
    /* Our base URI can never be undefined. */
    return wrapLiteral(toItem(AnyURI::fromValue(context->baseURI())), context, this)->typeCheck(context, reqType);
}

Expression::Ptr DefaultCollationFN::typeCheck(const StaticContext::Ptr &context,
                                              const SequenceType::Ptr &reqType)
{
    return wrapLiteral(AtomicString::fromValue(context->defaultCollation().toString()), context, this)->typeCheck(context, reqType);
}

QT_END_NAMESPACE
