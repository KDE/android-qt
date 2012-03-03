/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  qmlcodeparser.h
*/

#ifndef QMLCODEPARSER_H
#define QMLCODEPARSER_H

#include <QSet>
#include "declarativeparser/qdeclarativejsengine_p.h"
#include "declarativeparser/qdeclarativejslexer_p.h"
#include "declarativeparser/qdeclarativejsparser_p.h"

#include "codeparser.h"
#include "location.h"

QT_BEGIN_NAMESPACE

class Config;
class Node;
class QString;
class Tree;

class QmlCodeParser : public CodeParser
{
public:
    QmlCodeParser();
    virtual ~QmlCodeParser();

    virtual void initializeParser(const Config& config);
    virtual void terminateParser();
    virtual QString language();
    virtual QStringList sourceFileNameFilter();
    virtual void parseSourceFile(const Location& location,
                                 const QString& filePath, Tree *tree);
    virtual void doneParsingSourceFiles(Tree *tree);

    /* Copied from src/declarative/qml/qdeclarativescriptparser.cpp */
    void extractPragmas(QString &script);

protected:
    virtual QSet<QString> topicCommands();
    virtual QSet<QString> otherMetaCommands();

private:
    QDeclarativeJS::Engine engine;
    QDeclarativeJS::Lexer *lexer;
    QDeclarativeJS::Parser *parser;
};

QT_END_NAMESPACE

#endif
