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


#include "qinsertioniterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

InsertionIterator::InsertionIterator(const Item::Iterator::Ptr &target,
                                     const xsInteger pos,
                                     const Item::Iterator::Ptr &inserts)
                                    : m_target(target),
                                      m_insertPos(pos),
                                      m_inserts(inserts),
                                      m_position(0),
                                      m_isInserting(pos == 1)
{
    Q_ASSERT(target);
    Q_ASSERT(inserts);
    Q_ASSERT(m_insertPos >= 1);
}

Item InsertionIterator::next()
{
    if(m_isInserting)
    {
        m_current = m_inserts->next();

        if(m_current)
        {
            ++m_position;
            return m_current;
        }
    }
    else if(m_position == (m_insertPos - 1) && !m_isInserting)
    { /* Entered only the first time insertion starts. */
        m_isInserting = true;
        return next();
    }

    ++m_position;
    m_current = m_target->next();

    if(m_current)
        return m_current;
    else if(m_inserts->position() == -1) /* We're at the end of the both iterators. */
    {
        m_position = -1;
        m_current.reset();
        return Item();
    }

    /* Insert the insertion iterator, since it's still left. */
    Q_ASSERT(m_target->position() < m_insertPos);
    m_isInserting = true;
    m_current = m_inserts->next();

    if(m_current)
        return m_current;
    else
    {
        /* m_current is already null, so no need to reset it. */
        m_position = -1;
        return Item();
    }
}

xsInteger InsertionIterator::count()
{
    return m_target->count() + m_inserts->count();
}

Item InsertionIterator::current() const
{
    return m_current;
}

xsInteger InsertionIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr InsertionIterator::copy() const
{
    return Item::Iterator::Ptr(new InsertionIterator(m_target->copy(), m_insertPos, m_inserts->copy()));
}

QT_END_NAMESPACE
