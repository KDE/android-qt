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

#include "textreplacement.h"

QT_BEGIN_NAMESPACE

bool TextReplacements::insert(QByteArray newText, int insertPosition, int currentLenght)
{
    //bubble sort the new replacement into the list
    int i;
    for(i=0; i<textReplacementList.size(); ++i) {
        if (insertPosition == textReplacementList.at(i).insertPosition)
            return false; // multiple replacements on the same insertPosition is not allowed.
        if(insertPosition < textReplacementList.at(i).insertPosition)
            break;  //we found the right position
    }
    //++i;
  //  cout << "inserting new text " << newText.constData() << endl;
    // %s at %d overwriting %d bytes at list pos %d\n", newText.constData(), insertPosition, currentLenght, i);
    TextReplacement rep;
    rep.newText=newText;
    rep.insertPosition=insertPosition;
    rep.currentLenght=currentLenght;

    textReplacementList.insert(i, rep);
    return true;
}

void TextReplacements::clear()
{
    textReplacementList.clear();
}

QByteArray TextReplacements::apply(QByteArray current)
{
    QByteArray newData=current;
    int i;
    int replacementOffset=0;

    for(i=0; i<textReplacementList.size(); ++i) {
        TextReplacement rep=textReplacementList.at(i);
        //printf("applying new text %s insert at %d overwriting %d bytes \n", rep.newText.constData(), rep.insertPosition, rep.currentLenght);
        newData.remove(rep.insertPosition+replacementOffset, rep.currentLenght);
        newData.insert(rep.insertPosition+replacementOffset, rep.newText);

        //modify all remaining replacements if we change the document length
        replacementOffset+=(rep.newText.size() - rep.currentLenght);
    }

    return newData;
}

TextReplacements &TextReplacements::operator+=(const TextReplacements &other)
{
    foreach(TextReplacement rep, other.replacements()) {
        insert(rep.newText, rep.insertPosition, rep.currentLenght);
    }
    return *this;
}

QT_END_NAMESPACE
