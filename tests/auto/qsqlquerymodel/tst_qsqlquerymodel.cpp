/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>
#include <QtGui>

#include <qsqldriver.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>

#include <qsqlquerymodel.h>
#include <qsortfilterproxymodel.h>

#include "../qsqldatabase/tst_databases.h"

//TESTED_CLASS=
//TESTED_FILES=

Q_DECLARE_METATYPE(Qt::Orientation)

class tst_QSqlQueryModel : public QObject
{
    Q_OBJECT

public:
    tst_QSqlQueryModel();
    virtual ~tst_QSqlQueryModel();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void insertColumn_data() { generic_data(); }
    void insertColumn();
    void removeColumn_data() { generic_data(); }
    void removeColumn();
    void record_data() { generic_data(); }
    void record();
    void setHeaderData_data() { generic_data(); }
    void setHeaderData();
    void fetchMore_data() { generic_data(); }
    void fetchMore();

    //problem specific tests
    void withSortFilterProxyModel_data() { generic_data(); }
    void withSortFilterProxyModel();
    void setQuerySignalEmission_data() { generic_data(); }
    void setQuerySignalEmission();
    void setQueryWithNoRowsInResultSet_data() { generic_data(); }
    void setQueryWithNoRowsInResultSet();

    void task_180617();
    void task_180617_data() { generic_data(); }
    void task_QTBUG_4963_setHeaderDataWithProxyModel();

private:
    void generic_data(const QString &engine=QString());
    void dropTestTables(QSqlDatabase db);
    void createTestTables(QSqlDatabase db);
    void populateTestTables(QSqlDatabase db);
    tst_Databases dbs;
};

/* Stupid class that makes protected members public for testing */
class DBTestModel: public QSqlQueryModel
{
public:
    DBTestModel(QObject *parent = 0): QSqlQueryModel(parent) {}
    QModelIndex indexInQuery(const QModelIndex &item) const { return QSqlQueryModel::indexInQuery(item); }
};

tst_QSqlQueryModel::tst_QSqlQueryModel()
{
}

tst_QSqlQueryModel::~tst_QSqlQueryModel()
{
}

void tst_QSqlQueryModel::initTestCase()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    dbs.open();
    for (QStringList::ConstIterator it = dbs.dbNames.begin(); it != dbs.dbNames.end(); ++it) {
	QSqlDatabase db = QSqlDatabase::database((*it));
	CHECK_DATABASE(db);
	dropTestTables(db); //in case of leftovers
	createTestTables(db);
	populateTestTables(db);
    }
}

void tst_QSqlQueryModel::cleanupTestCase()
{
    for (QStringList::ConstIterator it = dbs.dbNames.begin(); it != dbs.dbNames.end(); ++it) {
	QSqlDatabase db = QSqlDatabase::database((*it));
	CHECK_DATABASE(db);
	dropTestTables(db);
    }
    dbs.close();
}

void tst_QSqlQueryModel::dropTestTables(QSqlDatabase db)
{
    QStringList tableNames;
    tableNames << qTableName("test")
            << qTableName("test2")
            << qTableName("test3")
            << qTableName("many");
    tst_Databases::safeDropTables(db, tableNames);
}

void tst_QSqlQueryModel::createTestTables(QSqlDatabase db)
{
    dropTestTables(db);
    QSqlQuery q(db);
    if(tst_Databases::isPostgreSQL(db))
        QVERIFY_SQL( q, exec("set client_min_messages='warning'"));
    QVERIFY_SQL( q, exec("create table " + qTableName("test") + "(id integer not null, name varchar(20), title integer, primary key (id))"));
    QVERIFY_SQL( q, exec("create table " + qTableName("test2") + "(id integer not null, title varchar(20), primary key (id))"));
    QVERIFY_SQL( q, exec("create table " + qTableName("test3") + "(id integer not null, primary key (id))"));
    QVERIFY_SQL( q, exec("create table " + qTableName("many") + "(id integer not null, name varchar(20), primary key (id))"));
}

void tst_QSqlQueryModel::populateTestTables(QSqlDatabase db)
{
    qWarning() << "Populating test tables, this can take quite a while... ZZZzzz...";
    bool hasTransactions = db.driver()->hasFeature(QSqlDriver::Transactions);

    QSqlQuery q(db), q2(db);

    tst_Databases::safeDropTables(db, QStringList() << qTableName("manytmp") << qTableName("test3tmp"));
    QVERIFY_SQL(q, exec("create table " + qTableName("manytmp") + "(id integer not null, name varchar(20), primary key (id))"));
    QVERIFY_SQL(q, exec("create table " + qTableName("test3tmp") + "(id integer not null, primary key (id))"));

    if (hasTransactions) QVERIFY_SQL(db, transaction());

    QVERIFY_SQL(q, exec("insert into " + qTableName("test") + " values(1, 'harry', 1)"));
    QVERIFY_SQL(q, exec("insert into " + qTableName("test") + " values(2, 'trond', 2)"));
    QVERIFY_SQL(q, exec("insert into " + qTableName("test2") + " values(1, 'herr')"));
    QVERIFY_SQL(q, exec("insert into " + qTableName("test2") + " values(2, 'mister')"));

    QVERIFY_SQL(q, exec(QString("insert into " + qTableName("test3") + " values(0)")));
    QVERIFY_SQL(q, prepare("insert into "+qTableName("test3")+"(id) select id + ? from "+qTableName("test3tmp")));
    for (int i=1; i<260; i*=2) {
        q2.exec("delete from "+qTableName("test3tmp"));
        QVERIFY_SQL(q2, exec("insert into "+qTableName("test3tmp")+"(id) select id from "+qTableName("test3")));
        q.bindValue(0, i);
        QVERIFY_SQL(q, exec());
    }

    QVERIFY_SQL(q, exec(QString("insert into " + qTableName("many") + "(id, name) values (0, \'harry\')")));
    QVERIFY_SQL(q, prepare("insert into "+qTableName("many")+"(id, name) select id + ?, name from "+qTableName("manytmp")));
    for (int i=1; i < 2048; i*=2) {
        q2.exec("delete from "+qTableName("manytmp"));
        QVERIFY_SQL(q2, exec("insert into "+qTableName("manytmp")+"(id, name) select id, name from "+qTableName("many")));
        q.bindValue(0, i);
        QVERIFY_SQL(q, exec());
    }

    if (hasTransactions) QVERIFY_SQL(db, commit());

    tst_Databases::safeDropTables(db, QStringList() << qTableName("manytmp") << qTableName("test3tmp"));
}

void tst_QSqlQueryModel::generic_data(const QString& engine)
{
    if ( dbs.fillTestTable(engine) == 0 ) {
        if(engine.isEmpty())
           QSKIP( "No database drivers are available in this Qt configuration", SkipAll );
        else
           QSKIP( (QString("No database drivers of type %1 are available in this Qt configuration").arg(engine)).toLocal8Bit(), SkipAll );
    }
}

void tst_QSqlQueryModel::init()
{
}

void tst_QSqlQueryModel::cleanup()
{
}

void tst_QSqlQueryModel::removeColumn()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    DBTestModel model;
    model.setQuery(QSqlQuery("select * from " + qTableName("test"), db));
    model.fetchMore();
    QSignalSpy spy(&model, SIGNAL(columnsAboutToBeRemoved(QModelIndex, int, int)));

    QCOMPARE(model.columnCount(), 3);
    QVERIFY(model.removeColumn(0));
    QCOMPARE(spy.count(), 1);
    QVERIFY(*(QModelIndex *)spy.at(0).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(0).at(1).toInt(), 0);
    QCOMPARE(spy.at(0).at(2).toInt(), 0);

    QCOMPARE(model.columnCount(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), -1);

    QVERIFY(model.insertColumn(1));
    QCOMPARE(model.columnCount(), 3);
    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), -1);

    QCOMPARE(model.data(model.index(0, 0)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 1)), QVariant());
    QCOMPARE(model.data(model.index(0, 2)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 3)), QVariant());

    QVERIFY(!model.removeColumn(42));
    QVERIFY(!model.removeColumn(3));
    QVERIFY(!model.removeColumn(1, model.index(1, 2)));
    QCOMPARE(model.columnCount(), 3);

    QVERIFY(model.removeColumn(2));

    QCOMPARE(spy.count(), 2);
    QVERIFY(*(QModelIndex *)spy.at(1).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(1).at(1).toInt(), 2);
    QCOMPARE(spy.at(1).at(2).toInt(), 2);

    QCOMPARE(model.columnCount(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), -1);

    QVERIFY(model.removeColumn(1));

    QCOMPARE(spy.count(), 3);
    QVERIFY(*(QModelIndex *)spy.at(2).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(2).at(1).toInt(), 1);
    QCOMPARE(spy.at(2).at(2).toInt(), 1);

    QCOMPARE(model.columnCount(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), -1);
    QCOMPARE(model.data(model.index(0, 0)).toString(), QString("harry"));

    QVERIFY(model.removeColumn(0));

    QCOMPARE(spy.count(), 4);
    QVERIFY(*(QModelIndex *)spy.at(3).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(3).at(1).toInt(), 0);
    QCOMPARE(spy.at(3).at(2).toInt(), 0);

    QCOMPARE(model.columnCount(), 0);
    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), -1);
}

void tst_QSqlQueryModel::insertColumn()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    DBTestModel model;
    model.setQuery(QSqlQuery("select * from " + qTableName("test"), db));
    model.fetchMore(); // necessary???

    QSignalSpy spy(&model, SIGNAL(columnsInserted(QModelIndex, int, int)));

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 2)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 3)), QVariant());

    QVERIFY(model.insertColumn(1));

    QCOMPARE(spy.count(), 1);
    QVERIFY(*(QModelIndex *)spy.at(0).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(0).at(1).toInt(), 1);
    QCOMPARE(spy.at(0).at(2).toInt(), 1);

    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), 0);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 4)).column(), -1);

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)), QVariant());
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 3)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 4)), QVariant());

    QVERIFY(!model.insertColumn(-1));
    QVERIFY(!model.insertColumn(100));
    QVERIFY(!model.insertColumn(1, model.index(1, 1)));

    QVERIFY(model.insertColumn(0));

    QCOMPARE(spy.count(), 2);
    QVERIFY(*(QModelIndex *)spy.at(1).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(1).at(1).toInt(), 0);
    QCOMPARE(spy.at(1).at(2).toInt(), 0);

    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), 0);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 4)).column(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 5)).column(), -1);

    QVERIFY(!model.insertColumn(6));
    QVERIFY(model.insertColumn(5));

    QCOMPARE(spy.count(), 3);
    QVERIFY(*(QModelIndex *)spy.at(2).at(0).constData() == QModelIndex());
    QCOMPARE(spy.at(2).at(1).toInt(), 5);
    QCOMPARE(spy.at(2).at(2).toInt(), 5);

    QCOMPARE(model.indexInQuery(model.index(0, 0)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 1)).column(), 0);
    QCOMPARE(model.indexInQuery(model.index(0, 2)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 3)).column(), 1);
    QCOMPARE(model.indexInQuery(model.index(0, 4)).column(), 2);
    QCOMPARE(model.indexInQuery(model.index(0, 5)).column(), -1);
    QCOMPARE(model.indexInQuery(model.index(0, 6)).column(), -1);

    bool isToUpper = db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2");
    QCOMPARE(model.record().field(0).name(), QString());
    QCOMPARE(model.record().field(1).name(), isToUpper ? QString("ID") : QString("id"));
    QCOMPARE(model.record().field(2).name(), QString());
    QCOMPARE(model.record().field(3).name(), isToUpper ? QString("NAME") : QString("name"));
    QCOMPARE(model.record().field(4).name(), isToUpper ? QString("TITLE") : QString("title"));
    QCOMPARE(model.record().field(5).name(), QString());
    QCOMPARE(model.record().field(6).name(), QString());
}

void tst_QSqlQueryModel::record()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQueryModel model;
    model.setQuery(QSqlQuery("select * from " + qTableName("test"), db));

    QSqlRecord rec = model.record();

    bool isToUpper = db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2");

    QCOMPARE(rec.count(), 3);
    QCOMPARE(rec.fieldName(0), isToUpper ? QString("ID") : QString("id"));
    QCOMPARE(rec.fieldName(1), isToUpper ? QString("NAME") : QString("name"));
    QCOMPARE(rec.fieldName(2), isToUpper ? QString("TITLE") : QString("title"));
    QCOMPARE(rec.value(0), QVariant());
    QCOMPARE(rec.value(1), QVariant());
    QCOMPARE(rec.value(2), QVariant());

    rec = model.record(0);
    QCOMPARE(rec.fieldName(0), isToUpper ? QString("ID") : QString("id"));
    QCOMPARE(rec.fieldName(1), isToUpper ? QString("NAME") : QString("name"));
    QCOMPARE(rec.fieldName(2), isToUpper ? QString("TITLE") : QString("title"));
    QCOMPARE(rec.value(0).toString(), QString("1"));
    QCOMPARE(rec.value(1), QVariant("harry"));
    QCOMPARE(rec.value(2), QVariant(1));
}

void tst_QSqlQueryModel::setHeaderData()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQueryModel model;

    QVERIFY(!model.setHeaderData(5, Qt::Vertical, "foo"));
    QVERIFY(model.headerData(5, Qt::Vertical).isValid());

    model.setQuery(QSqlQuery("select * from " + qTableName("test"), db));

    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    QSignalSpy spy(&model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)));
    QVERIFY(model.setHeaderData(2, Qt::Horizontal, "bar"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("bar"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<Qt::Orientation>(spy.value(0).value(0)), Qt::Horizontal);
    QCOMPARE(spy.value(0).value(1).toInt(), 2);
    QCOMPARE(spy.value(0).value(2).toInt(), 2);

    QVERIFY(!model.setHeaderData(7, Qt::Horizontal, "foo", Qt::ToolTipRole));
    QVERIFY(!model.headerData(7, Qt::Horizontal, Qt::ToolTipRole).isValid());

    bool isToUpper = db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2");
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), isToUpper ? QString("ID") : QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), isToUpper ? QString("NAME") : QString("name"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("bar"));
    QVERIFY(model.headerData(3, Qt::Horizontal).isValid());
}

void tst_QSqlQueryModel::fetchMore()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQueryModel model;
    QSignalSpy spy(&model, SIGNAL(rowsInserted(QModelIndex, int, int)));

    model.setQuery(QSqlQuery("select * from " + qTableName("many"), db));
    int rowCount = model.rowCount();

    QCOMPARE(spy.value(0).value(1).toInt(), 0);
    QCOMPARE(spy.value(0).value(2).toInt(), rowCount - 1);

    // If the driver doesn't return the query size fetchMore() causes the
    // model to grow and new signals are emitted
    if (!db.driver()->hasFeature(QSqlDriver::QuerySize)) {
        spy.clear();
        model.fetchMore();
        int newRowCount = model.rowCount();
        QCOMPARE(spy.value(0).value(1).toInt(), rowCount);
        QCOMPARE(spy.value(0).value(2).toInt(), newRowCount - 1);
    }
}

// For task 149491: When used with QSortFilterProxyModel, a view and a
// database that doesn't support the QuerySize feature, blank rows was
// appended if the query returned more than 256 rows and setQuery()
// was called more than once. This because an insertion of rows was
// triggered at the same time as the model was being cleared.
void tst_QSqlQueryModel::withSortFilterProxyModel()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    if (db.driver()->hasFeature(QSqlDriver::QuerySize))
        QSKIP("Test applies only for drivers not reporting the query size.", SkipSingle);

    QSqlQueryModel model;
    model.setQuery(QSqlQuery("SELECT * FROM " + qTableName("test3"), db));
    QSortFilterProxyModel proxy;
    proxy.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxy);

    QSignalSpy modelRowsRemovedSpy(&model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    QSignalSpy modelRowsInsertedSpy(&model, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    model.setQuery(QSqlQuery("SELECT * FROM " + qTableName("test3"), db));
    view.scrollToBottom();

    QTestEventLoop::instance().enterLoop(1);

    QCOMPARE(proxy.rowCount(), 511);

    // The second call to setQuery() clears the model by removing it's rows.
    // Only 256 rows because that is what was cached.
    QCOMPARE(modelRowsRemovedSpy.count(), 1);
    QCOMPARE(modelRowsRemovedSpy.value(0).value(1).toInt(), 0);
    QCOMPARE(modelRowsRemovedSpy.value(0).value(2).toInt(), 255);

    // The call to scrollToBottom() forces the model to fetch all rows,
    // which will be done in two steps.
    QCOMPARE(modelRowsInsertedSpy.count(), 2);
    QCOMPARE(modelRowsInsertedSpy.value(0).value(1).toInt(), 0);
    QCOMPARE(modelRowsInsertedSpy.value(0).value(2).toInt(), 255);
    QCOMPARE(modelRowsInsertedSpy.value(1).value(1).toInt(), 256);
    QCOMPARE(modelRowsInsertedSpy.value(1).value(2).toInt(), 510);
}

// For task 155402: When the model is already empty when setQuery() is called
// no rows have to be removed and rowsAboutToBeRemoved and rowsRemoved should
// not be emitted.
void tst_QSqlQueryModel::setQuerySignalEmission()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQueryModel model;
    QSignalSpy modelRowsAboutToBeRemovedSpy(&model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
    QSignalSpy modelRowsRemovedSpy(&model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));

    // First select, the model was empty and no rows had to be removed!
    model.setQuery(QSqlQuery("SELECT * FROM " + qTableName("test"), db));
    QCOMPARE(modelRowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(modelRowsRemovedSpy.count(), 0);

    // Second select, the model wasn't empty and two rows had to be removed!
    model.setQuery(QSqlQuery("SELECT * FROM " + qTableName("test"), db));
    QCOMPARE(modelRowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(modelRowsAboutToBeRemovedSpy.value(0).value(1).toInt(), 0);
    QCOMPARE(modelRowsAboutToBeRemovedSpy.value(0).value(2).toInt(), 1);
    QCOMPARE(modelRowsRemovedSpy.count(), 1);
    QCOMPARE(modelRowsRemovedSpy.value(0).value(1).toInt(), 0);
    QCOMPARE(modelRowsRemovedSpy.value(0).value(2).toInt(), 1);
}

// For task 170783: When the query's result set is empty no rows should be inserted,
// i.e. no rowsAboutToBeInserted or rowsInserted signals should be emitted.
void tst_QSqlQueryModel::setQueryWithNoRowsInResultSet()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQueryModel model;
    QSignalSpy modelRowsAboutToBeInsertedSpy(&model, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)));
    QSignalSpy modelRowsInsertedSpy(&model, SIGNAL(rowsInserted(const QModelIndex &, int, int)));

    // The query's result set will be empty so no signals should be emitted!
    QSqlQuery query(db);
    QVERIFY_SQL(query, exec("SELECT * FROM " + qTableName("test") + " where 0 = 1"));
    model.setQuery(query);
    QCOMPARE(modelRowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(modelRowsInsertedSpy.count(), 0);
}

// For task 180617
// According to the task, several specific duplicate SQL queries would cause
// multiple empty grid lines to be visible in the view
void tst_QSqlQueryModel::task_180617()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QTableView view;
    QCOMPARE(view.columnAt(0), -1);
    QCOMPARE(view.rowAt(0), -1);

    QSqlQueryModel model;
    model.setQuery( "SELECT TOP 0 * FROM " + qTableName("test3"), db );
    view.setModel(&model);

    bool error = false;
    // Usually a syntax error
    if (model.lastError().isValid())    // usually a syntax error
        error = true;

    QCOMPARE(view.columnAt(0), (error)?-1:0 );
    QCOMPARE(view.rowAt(0), -1);

    model.setQuery( "SELECT TOP 0 * FROM " + qTableName("test3"), db );
    model.setQuery( "SELECT TOP 0 * FROM " + qTableName("test3"), db );
    model.setQuery( "SELECT TOP 0 * FROM " + qTableName("test3"), db );
    model.setQuery( "SELECT TOP 0 * FROM " + qTableName("test3"), db );

    QCOMPARE(view.columnAt(0),  (error)?-1:0 );
    QCOMPARE(view.rowAt(0), -1);
}

void tst_QSqlQueryModel::task_QTBUG_4963_setHeaderDataWithProxyModel()
{
    QSqlQueryModel plainModel;
    QSortFilterProxyModel proxyModel;
    proxyModel.setSourceModel(&plainModel);
    QVERIFY(!plainModel.setHeaderData(0, Qt::Horizontal, QObject::tr("ID")));
    // And it should not crash.
}

QTEST_MAIN(tst_QSqlQueryModel)
#include "tst_qsqlquerymodel.moc"
