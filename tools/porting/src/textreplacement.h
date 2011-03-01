/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef TEXTREPLACEMENT_H
#define TEXTREPLACEMENT_H

#include <QByteArray>
#include <QList>
#include <QtAlgorithms>

QT_BEGIN_NAMESPACE

class TextReplacement
{
public:
    QByteArray newText;
    int insertPosition;
    int currentLenght;    //length of the text that is going to be replaced.
    bool operator<(const TextReplacement &other) const
    {
        return  (insertPosition < other.insertPosition);
    }
};

class TextReplacements
{
public:
    /*
        creates a TextReplacement that inserts newText at insertPosition. currentLength bytes
        are overwritten in the original text. If there already is an insert at insertPosition,
        the insert will not be performed.

        insert maintains the TextReplacement list in sorted order.

        Returns true if the insert was successful, false otherwise;
    */
    bool insert(QByteArray newText, int insertPosition, int currentLenght);
    void clear();
    QList<TextReplacement> replacements() const
    {
        return textReplacementList;
    }
    QByteArray apply(QByteArray current);

    TextReplacements &operator+=(const TextReplacements &other);

private:
    QList<TextReplacement> textReplacementList;
};

QT_END_NAMESPACE

#endif
