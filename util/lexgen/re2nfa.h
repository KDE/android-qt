/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
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

#ifndef RE2NFA_H
#define RE2NFA_H

#include "nfa.h"
#include <QSet>

class RE2NFA
{
public:
    RE2NFA(const QMap<QString, NFA> &macros, const QSet<InputType> &maxInputSet, Qt::CaseSensitivity cs);

    NFA parse(const QString &expression, int *errorColumn = 0);

private:
    NFA parseExpr();
    NFA parseBranch();
    NFA parsePiece();
    NFA parseAtom();
    NFA parseMaybeQuantifier(const NFA &nfa);
    NFA parseSet();
    NFA parseSet2();

    NFA createCharNFA();

private:
    friend class RegExpTokenizer;

    enum Token {
        TOK_INVALID,
        TOK_STRING,
        TOK_LBRACE,   // {
        TOK_RBRACE,   // }
        TOK_LBRACKET, // [
        TOK_RBRACKET, // ]
        TOK_LPAREN,   // (
        TOK_RPAREN,   // )
        TOK_COMMA,
        TOK_STAR,
        TOK_OR,
        TOK_QUESTION,
        TOK_DOT,
        TOK_PLUS,
        TOK_SEQUENCE,
        TOK_QUOTED_STRING
    };

    struct Symbol
    {
        inline Symbol() : token(TOK_INVALID), column(-1) {}
        inline Symbol(Token t, const QString &l = QString()) : token(t), lexem(l), column(-1) {}
        Token token;
        QString lexem;
        int column;
    };

    inline bool hasNext() const { return index < symbols.count(); }
    inline Token next() { return symbols.at(index++).token; }
    bool next(Token t);
    bool test(Token t);
    inline void prev() { index--; }
    inline const Symbol &symbol() const { return symbols.at(index - 1); }
    QString lexemUntil(Token t);

    void tokenize(const QString &input);

    QMap<QString, NFA> macros;
    QVector<Symbol> symbols;
    int index;
    int errorColumn;
    const QSet<InputType> maxInputSet;
    Qt::CaseSensitivity caseSensitivity;
};

#endif // RE2NFA_H

