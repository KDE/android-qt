/****************************************************************************
**
** Copyright (C) 2001-2004 Roberto Raggi
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "tokenengine.h"
#include <QVector>
#include <QByteArray>

QT_BEGIN_NAMESPACE

class Tokenizer
{
public:
    Tokenizer();
    ~Tokenizer();
    typedef void (Tokenizer::*scan_fun_ptr)(int *kind);
    QVector<TokenEngine::Token> tokenize(QByteArray text);
private:
    bool nextToken(TokenEngine::Token &tok);

    void scanChar(int *kind);
    void scanUnicodeChar(int *kind);
    void scanNewline(int *kind);
    void scanWhiteSpaces(int *kind);
    void scanCharLiteral(int *kind);
    void scanStringLiteral(int *kind);
    void scanNumberLiteral(int *kind);
    void scanIdentifier(int *kind);
    void scanComment(int *kind);
    void scanPreprocessor(int *kind);
    void scanOperator(int *kind);

    void setupScanTable();
private:
    static bool s_initialized;
    static scan_fun_ptr s_scan_table[];
    static int s_attr_table[];

    const char *m_buffer;
    int m_ptr;

    QVector<TokenEngine::Token> m_tokens;
};

QT_END_NAMESPACE

#endif
