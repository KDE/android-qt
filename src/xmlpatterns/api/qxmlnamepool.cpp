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

#include "qnamepool_p.h"
#include "qxmlnamepool.h"

QT_BEGIN_NAMESPACE

/*!
  \class QXmlNamePool
  \brief The QXmlNamePool class is a table of shared strings referenced by instances of QXmlName.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlNamePool is used to optimize operations on instances of
  QXmlName. An instance of QXmlName represents an XML name in
  a way that allows the XML name to be compared and passed around
  efficiently. The efficiency is achieved by storing the strings
  comprising the XML names in an instance of QXmlNamePool, where
  they are mapped to binary identifiers, which are then packed
  into a key which is stored in the QXmlName.

  This means that each instance of QXmlName is tied to the name
  pool it was created with, and that name pool should be kept in
  scope and used to create all instances of QXmlName that might
  be compared. Note also that the name pool is required if you
  must reconstitute the QXmlName as text, or if you must access
  any of its component strings, so although instances of
  QXmlName can be compared without reference to a name pool, the
  name pool must be kept in scope if the name's strings must be
  accessed later.

  \sa QXmlName
  \sa QXmlQuery::namePool()
 */

/*!
  Constructs an empty name pool.
 */
QXmlNamePool::QXmlNamePool() : d(new QPatternist::NamePool())
{
}

/*!
  Constructs a copy of the \a other name pool.
 */
QXmlNamePool::QXmlNamePool(const QXmlNamePool &other) : d(other.d)
{
}

/*!
  Destroys the name pool. Instances of QXmlName constructed
  with this name pool can still be compared after this destructor
  is called, but their text strings cannot be accessed.
 */
QXmlNamePool::~QXmlNamePool()
{
}

QXmlNamePool::QXmlNamePool(QPatternist::NamePool *namePool) : d(QExplicitlySharedDataPointer<QPatternist::NamePool>(namePool))
{
}

/*!
  Assigns the \a other name pool to this one.
 */
QXmlNamePool &QXmlNamePool::operator=(const QXmlNamePool &other)
{
    d = other.d;
    return *this;
}

QT_END_NAMESPACE

