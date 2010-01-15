/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include "employee.h"

//! [0]
EmployeeData::EmployeeData()
{
    id = -1;
    name = 0;
}
//! [0]

//! [1]
EmployeeData::EmployeeData(const EmployeeData &other)
//! [1] //! [2]
    : QSharedData(other)
{
    id = other.id;
    if (other.name) {
        name = new QString(*other.name);
    } else {
        name = 0;
    }
}
//! [2]

//! [3]
EmployeeData::~EmployeeData()
//! [3] //! [4]
{
    delete name;
}
//! [4]

//! [5]
Employee::Employee()
//! [5] //! [6]
{
    d = new EmployeeData;
}
//! [6]

//! [7]
Employee::Employee(int id, const QString &name)
//! [7] //! [8]
{
    d = new EmployeeData;
    setId(id);
    setName(name);
}
//! [8]

//! [9]
void Employee::setName(const QString &name)
//! [9] //! [10]
{
    if (!d->name)
        d->name = new QString;
    *d->name = name;
}
//! [10]

//! [11]
QString Employee::name() const
//! [11] //! [12]
{
    if (!d->name)
        return QString();
    return *d->name;
}
//! [12]
