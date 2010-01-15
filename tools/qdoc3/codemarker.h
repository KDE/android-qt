/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
  codemarker.h
*/

#ifndef CODEMARKER_H
#define CODEMARKER_H

#include <qpair.h>

#include "node.h"

QT_BEGIN_NAMESPACE

class Config;
class Tree;

struct Section
{
    QString name;
    QString singularMember;
    QString pluralMember;
    NodeList members;
    NodeList reimpMembers;
    QList<QPair<ClassNode *, int> > inherited;

    Section() { }
    Section(const QString& name0, 
            const QString& singularMember0, 
            const QString& pluralMember0) 
        : name(name0), 
          singularMember(singularMember0),
	  pluralMember(pluralMember0) { }
    void appendMember(Node* node) { members.append(node); }
    void appendReimpMember(Node* node) { reimpMembers.append(node); }
};

struct FastSection
{
    const InnerNode *innerNode;
    QString name;
    QString singularMember;
    QString pluralMember;
    QMap<QString, Node *> memberMap;
    QMap<QString, Node *> reimpMemberMap;
    QList<QPair<ClassNode *, int> > inherited;

    FastSection(const InnerNode *innerNode0, 
                const QString& name0 = "",
                const QString& singularMember0 = "member",
                const QString& pluralMember0 = "members")
	: innerNode(innerNode0), 
          name(name0), 
          singularMember(singularMember0),
	  pluralMember(pluralMember0) { }
    bool isEmpty() const { 
        return (memberMap.isEmpty() && inherited.isEmpty() && 
                reimpMemberMap.isEmpty()); 
    }

};

class CodeMarker
{
 public:
    enum SynopsisStyle { Summary, Detailed, SeparateList, Accessors };
    enum Status { Compat, Obsolete, Okay };

    CodeMarker();
    virtual ~CodeMarker();

    virtual void initializeMarker(const Config& config);
    virtual void terminateMarker();
    virtual bool recognizeCode(const QString& code) = 0;
    virtual bool recognizeExtension(const QString& ext) = 0;
    virtual bool recognizeLanguage(const QString& lang) = 0;
    virtual QString plainName(const Node *node) = 0;
    virtual QString plainFullName(const Node *node, 
                                  const Node *relative = 0) = 0;
    virtual QString markedUpCode(const QString& code, 
                                 const Node *relative,
                                 const QString& dirPath) = 0;
    virtual QString markedUpSynopsis(const Node *node, 
                                     const Node *relative,
                                     SynopsisStyle style) = 0;
#ifdef QDOC_QML
    virtual QString markedUpQmlItem(const Node* , bool) { return QString(); }
#endif
    virtual QString markedUpName(const Node *node) = 0;
    virtual QString markedUpFullName(const Node *node,
                                     const Node *relative = 0) = 0;
    virtual QString markedUpEnumValue(const QString &enumValue, 
                                      const Node *relative) = 0;
    virtual QString markedUpIncludes(const QStringList& includes) = 0;
    virtual QString functionBeginRegExp(const QString& funcName) = 0;
    virtual QString functionEndRegExp(const QString& funcName) = 0;
    virtual QList<Section> sections(const InnerNode *inner, 
                                    SynopsisStyle style, 
                                    Status status) = 0;
#ifdef QDOC_QML
    virtual QList<Section> qmlSections(const QmlClassNode* qmlClassNode,
                                       SynopsisStyle style);
#endif
    virtual const Node *resolveTarget(const QString& target, 
                                      const Tree *tree,
		                      const Node *relative) = 0;
    virtual QStringList macRefsForNode(const Node *node);

    static void initialize(const Config& config);
    static void terminate();
    static CodeMarker *markerForCode(const QString& code);
    static CodeMarker *markerForFileName(const QString& fileName);
    static CodeMarker *markerForLanguage(const QString& lang);
    static const Node *nodeForString(const QString& string);
    static QString stringForNode(const Node *node);

 protected:
    bool hurryUp() const { return !slow; }

    virtual QString sortName(const Node *node);
    QString protect(const QString &string);
    QString typified(const QString &string);
    QString taggedNode(const Node* node);
#ifdef QDOC_QML
    QString taggedQmlNode(const Node* node);
#endif
    QString linkTag(const Node *node, const QString& body);
    void insert(FastSection &fastSection, 
                Node *node, 
                SynopsisStyle style, 
                Status status);
    bool insertReimpFunc(FastSection& fs, Node* node, Status status);
    void append(QList<Section>& sectionList, const FastSection& fastSection);

 private:
    QString macName(const Node *parent, const QString &name = QString());

    bool slow;

    static QString defaultLang;
    static QList<CodeMarker *> markers;
};

QT_END_NAMESPACE

#endif
