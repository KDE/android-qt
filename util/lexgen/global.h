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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QHash>
#include <QDataStream>
#include <QSet>

#include "configfile.h"

#if 1
typedef int InputType;

enum SpecialInputType {
    DigitInput,
    SpaceInput,
    Letter
};

#else

enum SpecialInputType {
    NoSpecialInput = 0,
    DigitInput,
    SpaceInput,
    LetterOrNumberInput
};

struct InputType
{
    inline InputType() : val(0), specialInput(NoSpecialInput) {}
    inline InputType(const int &val) : val(val), specialInput(NoSpecialInput) {}

    inline operator int() const { return val; }

    inline bool operator==(const InputType &other) const
    { return val == other.val; }
    inline bool operator!=(const InputType &other) const
    { return val != other.val; }

    int val;
    SpecialInputType specialInput;
};

inline int qHash(const InputType &t) { return qHash(t.val); }

inline QDataStream &operator<<(QDataStream &stream, const InputType &i)
{
    return stream << i;
}

inline QDataStream &operator>>(QDataStream &stream, InputType &i)
{
    return stream >> i;
}

#endif

const InputType Epsilon = -1;

struct Config
{
    inline Config() : caseSensitivity(Qt::CaseSensitive), debug(false), cache(false) {}
    QSet<InputType> maxInputSet;
    Qt::CaseSensitivity caseSensitivity;
    QString className;
    bool debug;
    bool cache;
    QString ruleFile;
    ConfigFile::SectionMap configSections;
};

#endif // GLOBAL_H
