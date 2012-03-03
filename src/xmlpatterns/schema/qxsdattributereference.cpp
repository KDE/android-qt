/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qxsdattributereference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool XsdAttributeReference::isAttributeUse() const
{
    return false;
}

bool XsdAttributeReference::isReference() const
{
    return true;
}

void XsdAttributeReference::setType(Type type)
{
    m_type = type;
}

XsdAttributeReference::Type XsdAttributeReference::type() const
{
    return m_type;
}

void XsdAttributeReference::setReferenceName(const QXmlName &referenceName)
{
    m_referenceName = referenceName;
}

QXmlName XsdAttributeReference::referenceName() const
{
    return m_referenceName;
}

void XsdAttributeReference::setSourceLocation(const QSourceLocation &location)
{
    m_sourceLocation = location;
}

QSourceLocation XsdAttributeReference::sourceLocation() const
{
    return m_sourceLocation;
}

QT_END_NAMESPACE
