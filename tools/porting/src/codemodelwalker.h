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

#ifndef CODEMODELWALKER_H
#define CODEMODELWALKER_H

#include "codemodel.h"

QT_BEGIN_NAMESPACE

class CodeModelWalker
{
public:
    virtual ~CodeModelWalker(){};
    virtual void parseScope(CodeModel::Scope *scope);
    virtual void parseClassScope(CodeModel::ClassScope *){};
    virtual void parseNamespaceScope(CodeModel::NamespaceScope *){};
    virtual void parseBlockScope(CodeModel::BlockScope *){};

    virtual void parseType(CodeModel::Type *type);
    virtual void parseEnumType(CodeModel::EnumType *){};
    virtual void parseClassType(CodeModel::ClassType *){};
    virtual void parseUnknownType(CodeModel::UnknownType *){};
    virtual void parseBuiltinType(CodeModel::BuiltinType *){};
    virtual void parsePointerType(CodeModel::PointerType *){};
    virtual void parseReferenceType(CodeModel::ReferenceType *){};
    virtual void parseGenericType(CodeModel::GenericType *){};
    virtual void parseAliasType(CodeModel::AliasType *){};

    virtual void parseMember(CodeModel::Member *member);
    virtual void parseFunctionMember(CodeModel::FunctionMember *);
    virtual void parseVariableMember(CodeModel::VariableMember *){};
    virtual void parseUsingDeclarationMember(CodeModel::UsingDeclarationMember *){};
    virtual void parseTypeMember(CodeModel::TypeMember *){};

    virtual void parseArgument(CodeModel::Argument *){};
    virtual void parseNameUse(CodeModel::NameUse *){};
};

QT_END_NAMESPACE

#endif
