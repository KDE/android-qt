/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "stationquery.h"

#include <QtCore/QStringList>
#include <QtXmlPatterns/QXmlQuery>

StationInformation::StationInformation()
{
}

StationInformation::StationInformation(const QString &id, const QString &name)
    : m_id(id), m_name(name)
{
}

QString StationInformation::id() const
{
    return m_id;
}

QString StationInformation::name() const
{
    return m_name;
}

//! [0]
StationInformation::List StationQuery::query(const QString &name)
{
    const QString stationIdQueryUrl = QString("doc(concat('http://wap.trafikanten.no/FromLink1.asp?fra=', $station))/wml/card/p/small/a[@title='Velg']/substring(@href,18)");
    const QString stationNameQueryUrl = QString("doc(concat('http://wap.trafikanten.no/FromLink1.asp?fra=', $station))/wml/card/p/small/a[@title='Velg']/string()");

    QStringList stationIds;
    QStringList stationNames;

    QXmlQuery query;

    query.bindVariable("station", QVariant(QString::fromLatin1(QUrl::toPercentEncoding(name))));
    query.setQuery(stationIdQueryUrl);
    query.evaluateTo(&stationIds);

    query.bindVariable("station", QVariant(QString::fromLatin1(QUrl::toPercentEncoding(name))));
    query.setQuery(stationNameQueryUrl);
    query.evaluateTo(&stationNames);

    if (stationIds.count() != stationNames.count()) // something went wrong...
        return StationInformation::List();

    StationInformation::List information;
    for (int i = 0; i < stationIds.count(); ++i)
        information.append(StationInformation(stationIds.at(i), stationNames.at(i)));

    return information;
}
//! [0]
