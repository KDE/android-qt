/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <qgenericplugin_qpa.h>
#include "qtslib.h"

QT_BEGIN_NAMESPACE

class QTsLibPlugin : public QGenericPlugin
{
public:
    QTsLibPlugin();

    QStringList keys() const;
    QObject* create(const QString &key, const QString &specification);
};

QTsLibPlugin::QTsLibPlugin()
    : QGenericPlugin()
{
}

QStringList QTsLibPlugin::keys() const
{
    return (QStringList()
            << QLatin1String("Tslib")
            << QLatin1String("TslibRaw"));
}

QObject* QTsLibPlugin::create(const QString &key,
                                                 const QString &specification)
{
    if (!key.compare(QLatin1String("Tslib"), Qt::CaseInsensitive) || !key.compare(QLatin1String("TslibRaw"), Qt::CaseInsensitive))
        return new QTsLibMouseHandler(key, specification);
    return 0;
    }

Q_EXPORT_PLUGIN2(qtslibplugin, QTsLibPlugin)

QT_END_NAMESPACE
