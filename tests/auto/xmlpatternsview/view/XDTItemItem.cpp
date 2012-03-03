/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QList>
#include <QPointer>
#include <QVariant>

#include "Global.h"

#include "qitem_p.h"
#include "qreportcontext_p.h"

#include "XDTItemItem.h"

using namespace QPatternistSDK;

XDTItemItem::XDTItemItem(const QPatternist::Item &item,
                         XDTItemItem *p) : m_item(item),
                                           m_parent(p)
{
}

XDTItemItem::~XDTItemItem()
{
    qDeleteAll(m_children);
}

int XDTItemItem::columnCount() const
{
    return 3;
}

QVariant XDTItemItem::data(const Qt::ItemDataRole role, int column) const
{
    Q_ASSERT(m_item);
    if(role != Qt::DisplayRole)
        return QVariant();

    switch(column)
    {
        case 0:
            /* + 1: We don't want the index; the XDT counts from 1. */
            return row() + 1;
        case 1:
            return m_item.stringValue();
        case 2:
            return m_item.type()->displayName(Global::namePool());
        default:
        {
            Q_ASSERT(false);
            return QString();
        }
    }
}

TreeItem::List XDTItemItem::children() const
{
    return m_children;
}

void XDTItemItem::appendChild(TreeItem *item)
{
    m_children.append(item);
}

TreeItem *XDTItemItem::child(const unsigned int rowP) const
{
    return m_children.value(rowP);
}

unsigned int XDTItemItem::childCount() const
{
    return m_children.count();
}

TreeItem *XDTItemItem::parent() const
{
    return m_parent;
}

// vim: et:ts=4:sw=4:sts=4
