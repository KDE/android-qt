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

#include <qmousedriverplugin_qws.h>
#include <qmousetslib_qws.h>

QT_BEGIN_NAMESPACE

class TslibMouseDriver : public QMouseDriverPlugin
{
public:
    TslibMouseDriver();

    QStringList keys() const;
    QWSMouseHandler* create(const QString &driver, const QString &device);
};

TslibMouseDriver::TslibMouseDriver()
    : QMouseDriverPlugin()
{
}

QStringList TslibMouseDriver::keys() const
{
    return (QStringList() << "tslib");
}

QWSMouseHandler* TslibMouseDriver::create(const QString &driver,
                                          const QString &device)
{
    if (driver.toLower() != "tslib")
        return 0;
    return new QWSTslibMouseHandler(driver, device);
}

Q_EXPORT_STATIC_PLUGIN(TslibMouseDriver)
Q_EXPORT_PLUGIN2(qwstslibmousehandler, TslibMouseDriver)

QT_END_NAMESPACE
