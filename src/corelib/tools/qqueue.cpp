/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

/*!
    \class QQueue
    \brief The QQueue class is a generic container that provides a queue.

    \ingroup tools
    \ingroup shared

    \reentrant

    QQueue\<T\> is one of Qt's generic \l{container classes}. It
    implements a queue data structure for items of a same type.

    A queue is a first in, first out (FIFO) structure. Items are
    added to the tail of the queue using enqueue() and retrieved from
    the head using dequeue(). The head() function provides access to
    the head item without removing it.

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qqueue.cpp 0

    The example will output 1, 2, 3 in that order.

    QQueue inherits from QList. All of QList's functionality also
    applies to QQueue. For example, you can use isEmpty() to test
    whether the queue is empty, and you can traverse a QQueue using
    QList's iterator classes (for example, QListIterator). But in
    addition, QQueue provides three convenience functions that make
    it easy to implement FIFO semantics: enqueue(), dequeue(), and
    head().

    QQueue's value type must be an \l{assignable data type}. This
    covers most data types that are commonly used, but the compiler
    won't let you, for example, store a QWidget as a value. Use
    QWidget* instead.

    \sa QList, QStack
*/

/*!
    \fn QQueue::QQueue()

    Constructs an empty queue.
*/

/*!
    \fn QQueue::~QQueue()

    Destroys the queue. References to the values in the queue, and all
    iterators over this queue, become invalid.
*/

/*!
    \fn void QQueue::enqueue(const T& t)

    Adds value \a t to the tail of the queue.

    This is the same as QList::append().

    \sa dequeue(), head()
*/

/*!
    \fn T &QQueue::head()

    Returns a reference to the queue's head item. This function
    assumes that the queue isn't empty.

    This is the same as QList::first().

    \sa dequeue(), enqueue(), isEmpty()
*/

/*!
    \fn const T &QQueue::head() const

    \overload
*/

/*!
    \fn T QQueue::dequeue()

    Removes the head item in the queue and returns it. This function
    assumes that the queue isn't empty.

    This is the same as QList::takeFirst().

    \sa head(), enqueue(), isEmpty()
*/
