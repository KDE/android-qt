/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef UTILS_H
#define UTILS_H

#include "ui4.h"
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

inline bool toBool(const QString &str)
{ return str.toLower() == QLatin1String("true"); }

inline QString toString(const DomString *str)
{ return str ? str->text() : QString(); }

inline QString fixString(const QString &str, const QString &indent)
{
    QString cursegment;
    QStringList result;
    const QByteArray utf8 = str.toUtf8();
    const int utf8Length = utf8.length();

    for (int i = 0; i < utf8Length; ++i) {
        const uchar cbyte = utf8.at(i);
        if (cbyte >= 0x80) {
            cursegment += QLatin1Char('\\');
            cursegment += QString::number(cbyte, 8);
        } else {
            switch(cbyte) {
            case '\\':
                cursegment += QLatin1String("\\\\"); break;
            case '\"':
                cursegment += QLatin1String("\\\""); break;
            case '\r':
                break;
            case '\n':
                cursegment += QLatin1String("\\n\"\n\""); break;
            default:
                cursegment += QLatin1Char(cbyte);
            }
        }

        if (cursegment.length() > 1024) {
            result << cursegment;
            cursegment.clear();
        }
    }

    if (!cursegment.isEmpty())
        result << cursegment;


    QString joinstr = QLatin1String("\"\n");
    joinstr += indent;
    joinstr += indent;
    joinstr += QLatin1Char('"');

    QString rc(QLatin1Char('"'));
    rc += result.join(joinstr);
    rc += QLatin1Char('"');
    return rc;
}

inline QHash<QString, DomProperty *> propertyMap(const QList<DomProperty *> &properties)
{
    QHash<QString, DomProperty *> map;

    for (int i=0; i<properties.size(); ++i) {
        DomProperty *p = properties.at(i);
        map.insert(p->attributeName(), p);
    }

    return map;
}

inline QStringList unique(const QStringList &lst)
{
    QHash<QString, bool> h;
    for (int i=0; i<lst.size(); ++i)
        h.insert(lst.at(i), true);
    return h.keys();
}

QT_END_NAMESPACE

#endif // UTILS_H
