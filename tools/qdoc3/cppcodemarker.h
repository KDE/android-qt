/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
  cppcodemarker.h
*/

#ifndef CPPCODEMARKER_H
#define CPPCODEMARKER_H

#include "codemarker.h"

QT_BEGIN_NAMESPACE

class CppCodeMarker : public CodeMarker
{
 public:
    CppCodeMarker();
    ~CppCodeMarker();

    virtual bool recognizeCode(const QString& code);
    virtual bool recognizeExtension(const QString& ext);
    virtual bool recognizeLanguage(const QString& lang);
    virtual Atom::Type atomType() const;
    virtual QString plainName(const Node *node);
    virtual QString plainFullName(const Node *node, const Node *relative);
    virtual QString markedUpCode(const QString& code, 
                                 const Node *relative, 
                                 const Location &location);
    virtual QString markedUpSynopsis(const Node *node, 
                                     const Node *relative,
                                     SynopsisStyle style);
#ifdef QDOC_QML
    virtual QString markedUpQmlItem(const Node *node, bool summary);
#endif
    virtual QString markedUpName(const Node *node);
    virtual QString markedUpFullName(const Node *node, const Node *relative);
    virtual QString markedUpEnumValue(const QString &enumValue, const Node *relative);
    virtual QString markedUpIncludes(const QStringList& includes);
    virtual QString functionBeginRegExp(const QString& funcName);
    virtual QString functionEndRegExp(const QString& funcName);
    virtual QList<Section> sections(const InnerNode *innerNode, 
                                    SynopsisStyle style, 
                                    Status status);
    virtual QList<Section> qmlSections(const QmlClassNode* qmlClassNode,
                                       SynopsisStyle style,
                                       const Tree* tree);
    virtual const Node* resolveTarget(const QString& target, 
                                      const Tree* tree, 
                                      const Node* relative,
                                      const Node* self = 0);

private:
    QString addMarkUp(const QString& protectedCode, 
                      const Node *relative, 
                      const Location &location);
};

QT_END_NAMESPACE

#endif
