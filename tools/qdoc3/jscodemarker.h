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
  jscodemarker.h
*/

#ifndef JSCODEMARKER_H
#define JSCODEMARKER_H

#include "qmlcodemarker.h"

QT_BEGIN_NAMESPACE

class JsCodeMarker : public QmlCodeMarker
{
public:
    JsCodeMarker();
    ~JsCodeMarker();

    virtual bool recognizeCode(const QString &code);
    virtual bool recognizeExtension(const QString &ext);
    virtual bool recognizeLanguage(const QString &language);
    virtual Atom::Type atomType() const;

    virtual QString markedUpCode(const QString &code, 
                                 const Node *relative, 
                                 const Location &location);

private:
    QString addMarkUp(const QString &code, const Node *relative,
                      const Location &location);
};

QT_END_NAMESPACE

#endif
