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

#include "qinteger_p.h"

#include "qrangeiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

RangeIterator::RangeIterator(const xsInteger start,
                             const Direction direction,
                             const xsInteger end)
                             : m_start(start),
                               m_end(end),
                               m_position(0),
                               m_count(start),
                               m_direction(direction),
                               m_increment(m_direction == Forward ? 1 : -1)
{
    Q_ASSERT(m_start < m_end);
    Q_ASSERT(m_direction == Backward || m_direction == Forward);

    if(m_direction == Backward)
    {
        qSwap(m_start, m_end);
        m_count = m_start;
    }
}

Item RangeIterator::next()
{
    if(m_position == -1)
        return Item();
    else if((m_direction == Forward && m_count > m_end) ||
            (m_direction == Backward && m_count < m_end))
    {
        m_position = -1;
        m_current.reset();
        return Item();
    }
    else
    {
        m_current = Integer::fromValue(m_count);
        m_count += m_increment;
        ++m_position;
        return m_current;
    }
}

xsInteger RangeIterator::count()
{
    /* This complication is for handling that m_start & m_end may be reversed. */
    xsInteger ret;

    if(m_start < m_end)
        ret = m_end - m_start;
    else
        ret = m_start - m_end;

    return ret + 1;
}

Item::Iterator::Ptr RangeIterator::toReversed()
{
    return Item::Iterator::Ptr(new RangeIterator(m_start, Backward, m_end));
}

Item RangeIterator::current() const
{
    return m_current;
}

xsInteger RangeIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr RangeIterator::copy() const
{
    if(m_direction == Backward)
        return Item::Iterator::Ptr(new RangeIterator(m_end, Backward, m_start));
    else
        return Item::Iterator::Ptr(new RangeIterator(m_start, Forward, m_end));
}

QT_END_NAMESPACE
