/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qmime.h"

QT_BEGIN_NAMESPACE

/*!
    \class QMimeSource
    \brief The QMimeSource class is an abstraction of objects that
           provided formatted data of a certain MIME type.

    \obsolete

    The preferred approach to drag and drop is to use QDrag in
    conjunction with QMimeData. See \l{Drag and Drop} for details.

    \sa QMimeData, QDrag
*/

/*!
    Destroys the MIME source.
*/
QMimeSource::~QMimeSource()
{
}

/*!
    \fn const char *QMimeSource::format(int i) const

    Returns the (\a i - 1)-th supported MIME format, or 0.
*/

/*!
    \fn QByteArray QMimeSource::encodedData(const char *format) const

    Returns the encoded data of this object in the specified MIME
    \a format.
*/

/*!
    Returns true if the object can provide the data in format \a
    mimeType; otherwise returns false.

    If you inherit from QMimeSource, for consistency reasons it is
    better to implement the more abstract canDecode() functions such
    as QTextDrag::canDecode() and QImageDrag::canDecode().
*/
bool QMimeSource::provides(const char* mimeType) const
{
    const char* fmt;
    for (int i=0; (fmt = format(i)); i++) {
        if (!qstricmp(mimeType,fmt))
            return true;
    }
    return false;
}

QT_END_NAMESPACE
