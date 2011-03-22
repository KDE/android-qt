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

#ifndef TRANSLATIONUNIT_H
#define TRANSLATIONUNIT_H

#include "tokenengine.h"
#include "tokenstreamadapter.h"
#include "ast.h"
#include "codemodel.h"
#include "smallobject.h"
#include "cpplexer.h"
#include "parser.h"
#include "semantic.h"
#include <QSharedData>

QT_BEGIN_NAMESPACE

class TranslationUnitData : public QSharedData
{
public:
    TranslationUnitData(const TokenEngine::TokenSectionSequence &t)
    :tokens(t),  globalScope(0) {};
    TokenEngine::TokenSectionSequence tokens;
    CodeModel::NamespaceScope *globalScope;
    TypedPool<CodeModel::Item> codeModelMemoryPool;
};

class TranslationUnit
{
public:
    TranslationUnit();
    TranslationUnit(const TokenEngine::TokenSectionSequence &tokens);
    TokenEngine::TokenSectionSequence tokens() const;
    CodeModel::NamespaceScope *codeModel();
    TypedPool<CodeModel::Item> *codeModelMemoryPool();
private:
    friend class TranslationUnitAnalyzer;
    void setCodeModel(CodeModel::NamespaceScope *globalScope);
    QExplicitlySharedDataPointer<TranslationUnitData> d;
};

class TranslationUnitAnalyzer
{
public:
    TranslationUnit analyze
            (const TokenEngine::TokenSectionSequence &translationUnitTokens, int targetMaxASTNodes = 10000);
private:
    CppLexer lexer;
    Parser parser;
};

QT_END_NAMESPACE

#endif
