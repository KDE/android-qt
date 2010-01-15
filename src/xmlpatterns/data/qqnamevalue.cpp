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

#include "qanyuri_p.h"
#include "qbuiltintypes_p.h"
#include "qxpathhelper_p.h"

#include "qqnamevalue_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QNameValue::QNameValue(const NamePool::Ptr &np, const QXmlName name) : m_qName(name)
                                                                     , m_namePool(np)
{
    Q_ASSERT(!name.isNull());
    Q_ASSERT(m_namePool);
}

QNameValue::Ptr QNameValue::fromValue(const NamePool::Ptr &np, const QXmlName name)
{
    Q_ASSERT(!name.isNull());
    return QNameValue::Ptr(new QNameValue(np, name));
}

QString QNameValue::stringValue() const
{
    return m_namePool->toLexical(m_qName);
}

ItemType::Ptr QNameValue::type() const
{
    return BuiltinTypes::xsQName;
}

QT_END_NAMESPACE
