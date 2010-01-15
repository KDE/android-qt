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

#include "qbuiltintypes_p.h"
#include "qitem_p.h"

#include "qschematime_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SchemaTime::SchemaTime(const QDateTime &dateTime) : AbstractDateTime(dateTime)
{
}

SchemaTime::Ptr SchemaTime::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable( // STATIC DATA
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                             /* Any preceding whitespace. */
                "(\\d{2})"                          /* Hour part */
                ":"                                 /* Delimiter. */
                "(\\d{2})"                          /* Minutes part */
                ":"                                 /* Delimiter. */
                "(\\d{2,})"                         /* Seconds part. */
                "(?:\\.(\\d+))?"                    /* Milli seconds part. */
                "(?:(\\+|-)(\\d{2}):(\\d{2})|(Z))?" /* The zone offset, "+08:24". */
                "\\s*$"                             /* Any terminating whitespace. */))),
        /*zoneOffsetSignP*/         5,
        /*zoneOffsetHourP*/         6,
        /*zoneOffsetMinuteP*/       7,
        /*zoneOffsetUTCSymbolP*/    8,
        /*yearP*/                   -1,
        /*monthP*/                  -1,
        /*dayP*/                    -1,
        /*hourP*/                   1,
        /*minutesP*/                2,
        /*secondsP*/                3,
        /*msecondsP*/               4);

    AtomicValue::Ptr err;
    const QDateTime retval(create(err, lexical, captureTable));

    return err ? err : SchemaTime::Ptr(new SchemaTime(retval));
}

SchemaTime::Ptr SchemaTime::fromDateTime(const QDateTime &dt)
{
    Q_ASSERT(dt.isValid());
    /* Singleton value, allocated once instead of each time it's needed. */
    // STATIC DATA
    static const QDate time_defaultDate(AbstractDateTime::DefaultYear,
                                        AbstractDateTime::DefaultMonth,
                                        AbstractDateTime::DefaultDay);

    QDateTime result;
    copyTimeSpec(dt, result);

    result.setDate(time_defaultDate);
    result.setTime(dt.time());

    return SchemaTime::Ptr(new SchemaTime(result));
}

Item SchemaTime::fromValue(const QDateTime &dt) const
{
    Q_ASSERT(dt.isValid());
    return fromDateTime(dt);
}

QString SchemaTime::stringValue() const
{
    return timeToString() + zoneOffsetToString();
}

ItemType::Ptr SchemaTime::type() const
{
    return BuiltinTypes::xsTime;
}

QT_END_NAMESPACE
