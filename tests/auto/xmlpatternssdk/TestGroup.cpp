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

#include <QColor>
#include <QPair>
#include <QVariant>

#include "TestGroup.h"

using namespace QPatternistSDK;

TestGroup::TestGroup(TreeItem *p) : m_parent(p)
{
}

QVariant TestGroup::data(const Qt::ItemDataRole role, int column) const
{
    if(role != Qt::DisplayRole && role != Qt::BackgroundRole && role != Qt::ToolTipRole)
        return QVariant();

    /* In ResultSummary, the first is the amount of passes and the second is the total. */
    const ResultSummary sum(resultSummary());
    const int failures = sum.second - sum.first;

    switch(role)
    {
        case Qt::DisplayRole:
        {

            switch(column)
            {
                case 0:
                    return title();
                case 1:
                    /* Passes. */
                    return QString::number(sum.first);
                case 2:
                    /* Failures. */
                    return QString::number(failures);
                case 3:
                    /* Total. */
                    return QString::number(sum.second);
                default:
                {
                    Q_ASSERT(false);
                    return QString();
                }
            }
        }
        case Qt::BackgroundRole:
        {
            switch(column)
            {
                case 1:
                {
                    if(sum.first)
                    {
                        /* Pass. */
                        return Qt::green;
                    }
                    else
                        return QVariant();
                }
                case 2:
                {
                    if(failures)
                    {
                        /* Failure. */
                        return Qt::red;
                    }
                    else
                        return QVariant();
                }
                default:
                    return QVariant();
            }
        }
        case Qt::ToolTipRole:
        {
            return description();
        }
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "This shouldn't be reached");
            return QVariant();
        }
    }
}

void TestGroup::setNote(const QString &n)
{
    m_note = n;
}

QString TestGroup::note() const
{
    return m_note;
}

TreeItem *TestGroup::parent() const
{
    return m_parent;
}

// vim: et:ts=4:sw=4:sts=4
