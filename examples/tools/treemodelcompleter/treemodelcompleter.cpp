/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "treemodelcompleter.h"
#include <QStringList>

//! [0]
TreeModelCompleter::TreeModelCompleter(QObject *parent)
    : QCompleter(parent)
{
}
//! [0]

//! [1]
TreeModelCompleter::TreeModelCompleter(QAbstractItemModel *model, QObject *parent)
    : QCompleter(model, parent)
{
}
//! [1]

void TreeModelCompleter::setSeparator(const QString &separator)
{
    sep = separator;
}

//! [2]
QString TreeModelCompleter::separator() const
{
    return sep;
}
//! [2]

//! [3]
QStringList TreeModelCompleter::splitPath(const QString &path) const
{
    if (sep.isNull()) {
        return QCompleter::splitPath(path);
    }

    return path.split(sep);
}
//! [3]

//! [4]
QString TreeModelCompleter::pathFromIndex(const QModelIndex &index) const
{
    if (sep.isNull()) {
        return QCompleter::pathFromIndex(index);
    }

    // navigate up and accumulate data
    QStringList dataList;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        dataList.prepend(model()->data(i, completionRole()).toString());
    }

    return dataList.join(sep);
}
//! [4]

