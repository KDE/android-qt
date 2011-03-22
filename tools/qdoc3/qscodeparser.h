/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  qscodeparser.h
*/

#ifndef QSCODEPARSER_H
#define QSCODEPARSER_H

#include "cppcodeparser.h"
#include "cpptoqsconverter.h"

QT_BEGIN_NAMESPACE

class QsCodeParser : public CppCodeParser
{
 public:
    QsCodeParser(Tree *cppTree);
    ~QsCodeParser();

    virtual void initializeParser(const Config& config);
    virtual void terminateParser();
    virtual QString language();
    virtual QString headerFileNameFilter();
    virtual QString sourceFileNameFilter();
    virtual void parseHeaderFile(const Location& location,
                                 const QString& filePath, Tree *tree);
    virtual void parseSourceFile(const Location& location,
                                 const QString& filePath, Tree *tree);
    virtual void doneParsingHeaderFiles(Tree *tree);
    virtual void doneParsingSourceFiles(Tree *tree);

    FunctionNode *findFunctionNode(const QString& synopsis, Tree *tree);

 protected:
    virtual QSet<QString> topicCommands();
    virtual Node *processTopicCommand(const Doc& doc, const QString& command,
                                      const QString& arg);
    virtual QSet<QString> otherMetaCommands();
    virtual void processOtherMetaCommand(const Doc& doc,
                                         const QString& command,
                                         const QString& arg, Node *node);

 private:
    ClassNode *tryClass(const QString& className);
    FunctionNode *findKernelFunction(const QStringList& parentPath,
                                     const QString& name);
    void extractRegExp(const QRegExp& regExp, QString& source,
                       const Doc& doc);
    void extractTarget(const QString& target, QString& source,
                       const Doc& doc);
    void renameParameters(QString& source, const Doc& doc,
                          const QStringList& qtNames,
                          const QStringList& quickNames);
    void applyReplacementList(QString& source, const Doc& doc);
    void quickifyClass(ClassNode *quickClass);
    void quickifyEnum(ClassNode *quickClass, EnumNode *enume);
    void quickifyFunction(ClassNode *quickClass, ClassNode *qtClass,
                          FunctionNode *func, bool onBlackList);
    void quickifyProperty(ClassNode *quickClass, ClassNode *qtClass,
                          PropertyNode *property);
    QString quickifiedDoc(const QString& source);
    void setQtDoc(Node *quickNode, const Doc& doc);
    void setQuickDoc(Node *quickNode, const Doc& doc,
                     const QStringList& qtParams = QStringList(),
                     const QStringList& quickParams = QStringList());
    bool makeFunctionNode(const QString& synopsis, QStringList *parentPathPtr,
                          FunctionNode **funcPtr);

    static bool isWord(QChar ch);
    static bool leftWordBoundary(const QString& str, int pos);
    static bool rightWordBoundary(const QString& str, int pos);

    QMap<QString,Node::Type>    nodeTypeMap;
    QMap<QString,Node*>         classesWithNoQuickDoc;
    QList<QRegExp>              replaceBefores;
    QStringList                 replaceAfters;
    QSet<QString>               classesWithNoQ;
    Tree*                       cppTre;
    Tree*                       qsTre;
    QRegExp                     replaceRegExp;
    CppToQsConverter            cpp2qs;

    static int                  tabSize;
};

QT_END_NAMESPACE

#endif
