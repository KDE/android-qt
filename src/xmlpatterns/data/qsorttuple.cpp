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

#include <QString>

#include "qbuiltintypes_p.h"
#include "qsorttuple_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool SortTuple::isAtomicValue() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "It makes no sense to call this function.");
    return false;
}

QString SortTuple::stringValue() const
{
    return QLatin1String("SortTuple");
}

bool SortTuple::isNode() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "It makes no sense to call this function.");
    return false;
}

bool SortTuple::hasError() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "It makes no sense to call this function.");
    return false;
}

Item::Iterator::Ptr SortTuple::typedValue() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "It makes no sense to call this function.");
    return Item::Iterator::Ptr();
}

ItemType::Ptr SortTuple::type() const
{
    return BuiltinTypes::xsAnyAtomicType;
}

QT_END_NAMESPACE
