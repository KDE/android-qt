/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
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

#include "grammar_p.h"

const char *const grammar::spell [] = {
  "end of file", "identifier", "string literal", "%decl", "%expect", "%expect-lr", "%impl", "%left", "%merged_output", "%nonassoc", 
  "%parser", "%prec", "%right", "%start", "%token", "%token_prefix", ":", "|", ";", 0, 
  0, 0};

const int grammar::lhs [] = {
  22, 23, 23, 29, 25, 28, 28, 28, 28, 28, 
  28, 28, 24, 24, 31, 32, 32, 33, 33, 34, 
  34, 34, 31, 35, 35, 36, 37, 37, 38, 38, 
  30, 30, 26, 26, 40, 39, 41, 41, 44, 43, 
  43, 42, 42, 27, 45};

const int grammar:: rhs[] = {
  4, 1, 2, 2, 2, 2, 2, 2, 2, 2, 
  2, 2, 1, 2, 2, 1, 2, 2, 2, 1, 
  1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 
  0, 1, 1, 2, 2, 4, 3, 6, 0, 0, 
  2, 1, 2, 0, 2};

const int grammar::action_default [] = {
  44, 2, 44, 0, 0, 0, 0, 13, 0, 0, 
  3, 0, 0, 0, 8, 10, 11, 9, 7, 6, 
  12, 20, 22, 0, 21, 0, 44, 31, 0, 14, 
  26, 24, 23, 25, 4, 33, 1, 0, 34, 44, 
  35, 42, 39, 40, 0, 31, 44, 40, 43, 0, 
  31, 41, 29, 27, 28, 32, 38, 30, 36, 31, 
  37, 5, 44, 16, 15, 18, 19, 17, 45};

const int grammar::goto_default [] = {
  3, 2, 13, 26, 36, 41, 10, 27, 61, 29, 
  64, 63, 23, 32, 31, 52, 55, 38, 39, 42, 
  43, 59, 44, 0};

const int grammar::action_index [] = {
  -22, -22, 54, 1, 5, 15, 20, -22, -1, 6, 
  -22, 3, 2, 35, -22, -22, -22, -22, -22, -22, 
  -22, -22, -22, 10, -22, 7, -22, 14, 9, -22, 
  -22, -22, 8, -22, -22, -22, 11, -2, -22, -22, 
  -22, -22, -3, 16, 13, 14, -22, 17, -22, 4, 
  14, -22, -22, -22, -22, 14, -22, -22, -22, 14, 
  -22, -22, 0, -22, 12, -22, -22, -22, -22, 

  2, -24, -2, -24, -24, -24, -24, -24, -24, -24, 
  -24, -24, -24, -24, -24, -24, -24, -24, -24, -24, 
  -24, -24, -24, -24, -24, -24, -4, -24, -24, -24, 
  -24, -24, -14, -24, -24, -24, -24, -24, -24, -24, 
  -24, -24, -24, -24, -24, 0, -16, -15, -24, -24, 
  15, -24, -24, -24, -24, -10, -24, -24, -24, 1, 
  -24, -24, -3, -24, -1, -24, -24, -24, -24};

const int grammar::action_info [] = {
  17, 68, 66, 20, 19, 51, 14, 18, 34, 30, 
  62, 30, 37, 62, 40, 45, 15, 48, 48, 0, 
  0, 16, 0, 0, 0, 0, 0, 49, 49, 0, 
  46, 0, 0, 53, 54, 0, 0, 0, 0, 0, 
  0, 0, 21, 0, 22, 0, 0, 24, 25, 28, 
  0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 
  8, 0, 9, 0, 11, 0, 0, 0, 0, 12, 
  0, 0, 0, 0, 0, 0, 

  33, 35, 65, 7, 47, 57, 50, 1, 58, 60, 
  67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0};

const int grammar::action_check [] = {
  1, 0, 2, 1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 16, 18, 1, 1, 1, -1, 
  -1, 1, -1, -1, -1, -1, -1, 11, 11, -1, 
  17, -1, -1, 19, 20, -1, -1, -1, -1, -1, 
  -1, -1, 7, -1, 9, -1, -1, 12, 13, 14, 
  -1, -1, -1, -1, -1, -1, -1, 3, 4, 5, 
  6, -1, 8, -1, 10, -1, -1, -1, -1, 15, 
  -1, -1, -1, -1, -1, -1, 

  14, 5, 5, 5, 20, 15, 21, 5, 8, 8, 
  11, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
  -1, -1, -1, 8, -1, -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, -1, -1, -1, -1};

