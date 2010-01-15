/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQLTABLEMODEL_P_H
#define QSQLTABLEMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qsql*model.h .  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qmap.h"
#include "private/qsqlquerymodel_p.h"

QT_BEGIN_NAMESPACE

class QSqlTableModelPrivate: public QSqlQueryModelPrivate
{
    Q_DECLARE_PUBLIC(QSqlTableModel)

public:
    QSqlTableModelPrivate()
        : editIndex(-1), insertIndex(-1), sortColumn(-1),
          sortOrder(Qt::AscendingOrder),
          strategy(QSqlTableModel::OnRowChange)
    {}
    void clear();
    QSqlRecord primaryValues(int index);
    virtual void clearEditBuffer();
    virtual void clearCache();
    QSqlRecord record(const QVector<QVariant> &values) const;

    bool exec(const QString &stmt, bool prepStatement,
              const QSqlRecord &rec, const QSqlRecord &whereValues = QSqlRecord());
    virtual void revertCachedRow(int row);
    void revertInsertedRow();
    bool setRecord(int row, const QSqlRecord &record);
    virtual int nameToIndex(const QString &name) const;
    void initRecordAndPrimaryIndex();

    QSqlDatabase db;
    int editIndex;
    int insertIndex;

    int sortColumn;
    Qt::SortOrder sortOrder;

    QSqlTableModel::EditStrategy strategy;

    QSqlQuery editQuery;
    QSqlIndex primaryIndex;
    QString tableName;
    QString filter;

    enum Op { None, Insert, Update, Delete };

    struct ModifiedRow
    {
        ModifiedRow(Op o = None, const QSqlRecord &r = QSqlRecord()): op(o), rec(r) {}
        ModifiedRow(const ModifiedRow &other): op(other.op), rec(other.rec), primaryValues(other.primaryValues) {}
        Op op;
        QSqlRecord rec;
		QSqlRecord primaryValues;
    };

    QSqlRecord editBuffer;

    typedef QMap<int, ModifiedRow> CacheMap;
    CacheMap cache;
};

QT_END_NAMESPACE

#endif // QSQLTABLEMODEL_P_H
