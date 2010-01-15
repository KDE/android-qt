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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QApplicationArgument_H
#define QApplicationArgument_H

#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QString;
class QApplicationArgumentPrivate;

class QApplicationArgument
{
public:
    QApplicationArgument();
    QApplicationArgument(const QApplicationArgument &other);
    QApplicationArgument(const QString &name,
                 const QString &description,
                 int aType = QVariant::Invalid);
    ~QApplicationArgument();
    QApplicationArgument &operator=(const QApplicationArgument &other);
    bool operator==(const QApplicationArgument &other) const;

    void setName(const QString &newName);
    QString name() const;
    void setDescription(const QString &newDescription);
    QString description() const;

    int type() const;
    void setType(int newType);
    void setDefaultValue(const QVariant &value);
    QVariant defaultValue() const;

    void setMinimumOccurrence(int minimum);
    int minimumOccurrence() const;
    void setMaximumOccurrence(int maximum);
    int maximumOccurrence() const;
    void setNameless(bool value);
    bool isNameless() const;

private:
    QApplicationArgumentPrivate *d_ptr;
};

uint qHash(const QApplicationArgument &argument);

QT_END_NAMESPACE
QT_END_HEADER
#endif

