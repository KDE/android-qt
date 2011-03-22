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

#ifndef GENERATOR_H
#define GENERATOR_H

#include "moc.h"

QT_BEGIN_NAMESPACE

class Generator
{
    FILE *out;
    ClassDef *cdef;
    QVector<uint> meta_data;
public:
    Generator(ClassDef *classDef, const QList<QByteArray> &metaTypes, FILE *outfile = 0);
    void generateCode();
    QMetaObject *generateMetaObject(bool ignoreProperties);
private:
    void generateClassInfos();
    void generateFunctions(QList<FunctionDef> &list, const char *functype, int type);
    void generateEnums(int index);
    void generateProperties();
    void generateMetacall();
    void generateStaticMetacall(const QByteArray &prefix);
    void generateSignal(FunctionDef *def, int index);

    // used by binary QMetaObject generator
    void _generateClassInfos();
    void _generateFunctions(QList<FunctionDef> &list, int type);
    void _generateEnums(int index);
    void _generateProperties();

    int strreg(const char *); // registers a string and returns its id
    QList<QByteArray> strings;
    QByteArray purestSuperClass;
    QList<QByteArray> metaTypes;
};

QT_END_NAMESPACE

#endif // GENERATOR_H
