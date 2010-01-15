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
  cpptoqsconverter.h
*/

#ifndef CPPTOQSCONVERTER_H
#define CPPTOQSCONVERTER_H

#include <qregexp.h>

#include "tree.h"

QT_BEGIN_NAMESPACE

class CppToQsConverter
{
public:
    CppToQsConverter() { }

    ClassNode *findClassNode( Tree *qsTree, const QString& qtName );
    QString convertedDataType( Tree *qsTree, const QString& leftType,
			       const QString& rightType = "" );
    QString convertedCode( Tree *qsTree, const QString& code,
			   const QSet<QString>& classesWithNoQ );

    static void initialize( const Config& config );
    static void terminate();

private:
    void clearState();
    QString convertCodeLine( Tree *qsTree, const QStringList& program,
			     const QString& code,
			     const QSet<QString>& classesWithNoQ );
    QString convertComment( Tree *qsTree, const QString& comment,
			    const QSet<QString>& classesWithNoQ );
    QString convertExpr( Tree *qsTree, const QString& expr,
			 const QSet<QString>& classesWithNoQ );
    void updateDelimDepths( const QString& code );

    static QRegExp qClassRegExp;
    static QRegExp addressOperatorRegExp;
    static QRegExp gulbrandsenRegExp;
    static int tabSize;
};

QT_END_NAMESPACE

#endif
