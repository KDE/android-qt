/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3MEMBUF_P_H
#define Q3MEMBUF_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qbytearray.h"
#include "QtCore/qlist.h"

QT_BEGIN_NAMESPACE

class Q_COMPAT_EXPORT Q3Membuf
{
public:
    Q3Membuf();
    ~Q3Membuf();

    void append(QByteArray *ba);
    void clear();

    bool consumeBytes(Q_ULONG nbytes, char *sink);
    QByteArray readAll();
    bool scanNewline(QByteArray *store);
    bool canReadLine() const;

    int ungetch(int ch);

    qint64 size() const;

private:

    QList<QByteArray *> buf;
    qint64 _size;
    qint64 _index;
};

inline void Q3Membuf::append(QByteArray *ba)
{ buf.append(ba); _size += ba->size(); }

inline void Q3Membuf::clear()
{ qDeleteAll(buf); buf.clear(); _size=0; _index=0; }

inline QByteArray Q3Membuf::readAll()
{ QByteArray ba; ba.resize(_size); consumeBytes(_size,ba.data()); return ba; }

inline bool Q3Membuf::canReadLine() const
{ return const_cast<Q3Membuf*>(this)->scanNewline(0); }

inline qint64 Q3Membuf::size() const
{ return _size; }

QT_END_NAMESPACE

#endif // Q3MEMBUF_P_H
