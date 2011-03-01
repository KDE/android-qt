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
  location.h
*/

#ifndef LOCATION_H
#define LOCATION_H

#include <qstack.h>

#include "tr.h"

#define QDOC_QML

QT_BEGIN_NAMESPACE

class Config;
class QRegExp;

class Location
{
 public:
    Location();
    Location(const QString& filePath);
    Location(const Location& other);
    ~Location() { delete stk; }

    Location& operator=(const Location& other);

    void start();
    void advance(QChar ch);
    void advanceLines(int n) { stkTop->lineNo += n; stkTop->columnNo = 1; }

    void push(const QString& filePath);
    void pop();
    void setEtc(bool etc) { etcetera = etc; }
    void setLineNo(int no) { stkTop->lineNo = no; }
    void setColumnNo(int no) { stkTop->columnNo = no; }

    bool isEmpty() const { return stkDepth == 0; }
    int depth() const { return stkDepth; }
    const QString& filePath() const { return stkTop->filePath; }
    QString fileName() const;
    int lineNo() const { return stkTop->lineNo; }
    int columnNo() const { return stkTop->columnNo; }
    bool etc() const { return etcetera; }
    void warning(const QString& message, 
                 const QString& details = QString()) const;
    void error(const QString& message, 
               const QString& details = QString()) const;
    void fatal(const QString& message, 
               const QString& details = QString()) const;

    QT_STATIC_CONST Location null;

    static void initialize(const Config& config);
    static void terminate();
    static void information(const QString& message);
    static void internalError(const QString& hint);

 private:
    enum MessageType { Warning, Error };

    struct StackEntry
    {
	QString filePath;
	int lineNo;
	int columnNo;
    };

    void emitMessage(MessageType type, 
                     const QString& message,
                     const QString& details) const;
    QString toString() const;
    QString top() const;

 private:
    StackEntry stkBottom;
    QStack<StackEntry> *stk;
    StackEntry *stkTop;
    int stkDepth;
    bool etcetera;

    static int tabSize;
    static QString programName;
    static QRegExp *spuriousRegExp;
};

QT_END_NAMESPACE

#endif
