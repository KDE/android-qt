----------------------------------------------------------------------------
--
-- Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
-- All rights reserved.
-- Contact: Nokia Corporation (qt-info@nokia.com)
--
-- This file is part of the QtCore module of the Qt Toolkit.
--
-- $QT_BEGIN_LICENSE:LGPL$
-- No Commercial Usage
-- This file contains pre-release code and may not be distributed.
-- You may use this file in accordance with the terms and conditions
-- contained in the Technology Preview License Agreement accompanying
-- this package.
--
-- GNU Lesser General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU Lesser
-- General Public License version 2.1 as published by the Free Software
-- Foundation and appearing in the file LICENSE.LGPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU Lesser General Public License version 2.1 requirements
-- will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
--
-- In addition, as a special exception, Nokia gives you certain additional
-- rights.  These rights are described in the Nokia Qt LGPL Exception
-- version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
--
-- If you have questions regarding the use of this file, please contact
-- Nokia at qt-info@nokia.com.
--
--
--
--
--
--
--
--
-- $QT_END_LICENSE$
--
----------------------------------------------------------------------------

-- lambda calculus

%decl lambda.h

%token LPAREN
%token RPAREN
%token ID
%token FUN
%token DOT

%nonassoc SHIFT_THERE
%nonassoc LPAREN RPAREN ID FUN DOT
%nonassoc REDUCE_HERE

%start Expr

/:
enum {
:/


Expr ::= ID %prec SHIFT_THERE ;
/:  Symbol = $rule_number,
:/

Expr ::= LPAREN Expr RPAREN %prec SHIFT_THERE  ;
/:  SubExpression = $rule_number,
:/

Expr ::= Expr Expr %prec REDUCE_HERE ;
/:  Appl = $rule_number,
:/

Expr ::= FUN ID DOT Expr %prec SHIFT_THERE ;
/:  Abstr = $rule_number,
:/

/:};
:/

