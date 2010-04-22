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
#include "../qsqldatabase/tst_databases.h"
#include <QtSql>

const QString test(qTableName("test", __FILE__)),
                   test2(qTableName("test2", __FILE__)),
                   test3(qTableName("test3", __FILE__));

//TESTED_CLASS=
//TESTED_FILES=

Q_DECLARE_METATYPE(QModelIndex)

class tst_QSqlTableModel : public QObject
{
    Q_OBJECT

public:
    tst_QSqlTableModel();
    virtual ~tst_QSqlTableModel();


    void dropTestTables();
    void createTestTables();
    void recreateTestTables();
    void repopulateTestTables();

    tst_Databases dbs;

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:

    void select_data() { generic_data(); }
    void select();
    void submitAll_data() { generic_data(); }
    void submitAll();
    void setRecord_data()  { generic_data(); }
    void setRecord();
    void insertRow_data() { generic_data(); }
    void insertRow();
    void insertRecord_data() { generic_data(); }
    void insertRecord();
    void insertMultiRecords_data() { generic_data(); }
    void insertMultiRecords();
    void removeRow_data() { generic_data(); }
    void removeRow();
    void removeRows_data() { generic_data(); }
    void removeRows();
    void removeInsertedRow_data() { generic_data(); }
    void removeInsertedRow();
    void setFilter_data() { generic_data(); }
    void setFilter();
    void setInvalidFilter_data() { generic_data(); }
    void setInvalidFilter();

    void emptyTable_data() { generic_data(); }
    void emptyTable();
    void tablesAndSchemas_data() { generic_data("QPSQL"); }
    void tablesAndSchemas();
    void whitespaceInIdentifiers_data() { generic_data(); }
    void whitespaceInIdentifiers();
    void primaryKeyOrder_data() { generic_data("QSQLITE"); }
    void primaryKeyOrder();

    void sqlite_bigTable_data() { generic_data("QSQLITE"); }
    void sqlite_bigTable();

    // bug specific tests
    void insertRecordBeforeSelect_data() { generic_data(); }
    void insertRecordBeforeSelect();
    void submitAllOnInvalidTable_data() { generic_data(); }
    void submitAllOnInvalidTable();
    void insertRecordsInLoop_data() { generic_data(); }
    void insertRecordsInLoop();
    void sqlite_attachedDatabase_data() { generic_data("QSQLITE"); }
    void sqlite_attachedDatabase(); // For task 130799
    void tableModifyWithBlank_data() { generic_data(); }
    void tableModifyWithBlank(); // For mail task

    void removeColumnAndRow_data() { generic_data(); }
    void removeColumnAndRow(); // task 256032

    void insertBeforeDelete_data() { generic_data(); }
    void insertBeforeDelete();
private:
    void generic_data(const QString& engine=QString());
};

tst_QSqlTableModel::tst_QSqlTableModel()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    dbs.open();
}

tst_QSqlTableModel::~tst_QSqlTableModel()
{
}

void tst_QSqlTableModel::dropTestTables()
{
    for (int i = 0; i < dbs.dbNames.count(); ++i) {
        QSqlDatabase db = QSqlDatabase::database(dbs.dbNames.at(i));
        QSqlQuery q(db);
        if(tst_Databases::isPostgreSQL(db))
            QVERIFY_SQL( q, exec("set client_min_messages='warning'"));

        QStringList tableNames;
        tableNames << test
                   << test2
                   << test3
                   << qTableName("test4", __FILE__)
                   << qTableName("emptytable", __FILE__)
                   << qTableName("bigtable", __FILE__)
                   << qTableName("foo", __FILE__);
        if (testWhiteSpaceNames(db.driverName()))
            tableNames << qTableName("qtestw hitespace", db.driver());

        tst_Databases::safeDropTables(db, tableNames);

        if (db.driverName().startsWith("QPSQL")) {
            q.exec("DROP SCHEMA " + qTableName("testschema", __FILE__) + " CASCADE");
        }
    }
}

void tst_QSqlTableModel::createTestTables()
{
    for (int i = 0; i < dbs.dbNames.count(); ++i) {
        QSqlDatabase db = QSqlDatabase::database(dbs.dbNames.at(i));
        QSqlQuery q(db);

        QVERIFY_SQL( q, exec("create table " + test + "(id int, name varchar(20), title int)"));

        QVERIFY_SQL( q, exec("create table " + test2 + "(id int, title varchar(20))"));

        QVERIFY_SQL( q, exec("create table " + test3 + "(id int, random varchar(20), randomtwo varchar(20))"));

        if(!tst_Databases::isSqlServer(db))
            QVERIFY_SQL( q, exec("create table " + qTableName("test4", __FILE__) + "(column1 varchar(50), column2 varchar(50), column3 varchar(50))"));
        else
            QVERIFY_SQL( q, exec("create table " + qTableName("test4", __FILE__) + "(column1 varchar(50), column2 varchar(50) NULL, column3 varchar(50))"));


        QVERIFY_SQL( q, exec("create table " + qTableName("emptytable", __FILE__) + "(id int)"));

        if (testWhiteSpaceNames(db.driverName())) {
            QString qry = "create table " + qTableName("qtestw hitespace", db.driver()) + " ("+ db.driver()->escapeIdentifier("a field", QSqlDriver::FieldName) + " int)";
            QVERIFY_SQL( q, exec(qry));
        }
    }
}

void tst_QSqlTableModel::repopulateTestTables()
{
    for (int i = 0; i < dbs.dbNames.count(); ++i) {
        QSqlDatabase db = QSqlDatabase::database(dbs.dbNames.at(i));
        QSqlQuery q(db);

        q.exec("delete from " + test);
        QVERIFY_SQL( q, exec("insert into " + test + " values(1, 'harry', 1)"));
        QVERIFY_SQL( q, exec("insert into " + test + " values(2, 'trond', 2)"));
        QVERIFY_SQL( q, exec("insert into " + test + " values(3, 'vohi', 3)"));

        q.exec("delete from " + test2);
        QVERIFY_SQL( q, exec("insert into " + test2 + " values(1, 'herr')"));
        QVERIFY_SQL( q, exec("insert into " + test2 + " values(2, 'mister')"));

        q.exec("delete from " + test3);
        QVERIFY_SQL( q, exec("insert into " + test3 + " values(1, 'foo', 'bar')"));
        QVERIFY_SQL( q, exec("insert into " + test3 + " values(2, 'baz', 'joe')"));
    }
}

void tst_QSqlTableModel::recreateTestTables()
{
    dropTestTables();
    createTestTables();
    repopulateTestTables();
}

void tst_QSqlTableModel::generic_data(const QString &engine)
{
    if ( dbs.fillTestTable(engine) == 0 ) {
        if(engine.isEmpty())
           QSKIP( "No database drivers are available in this Qt configuration", SkipAll );
        else
           QSKIP( (QString("No database drivers of type %1 are available in this Qt configuration").arg(engine)).toLocal8Bit(), SkipAll );
    }
}

void tst_QSqlTableModel::initTestCase()
{
    recreateTestTables();
}

void tst_QSqlTableModel::cleanupTestCase()
{
    dropTestTables();
    dbs.close();
}

void tst_QSqlTableModel::init()
{
}

void tst_QSqlTableModel::cleanup()
{
    repopulateTestTables();
}

void tst_QSqlTableModel::select()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    QVERIFY_SQL(model, select());

    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.columnCount(), 3);

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 2)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 3)), QVariant());

    QCOMPARE(model.data(model.index(1, 0)).toInt(), 2);
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond"));
    QCOMPARE(model.data(model.index(1, 2)).toInt(), 2);
    QCOMPARE(model.data(model.index(1, 3)), QVariant());

    QCOMPARE(model.data(model.index(2, 0)).toInt(), 3);
    QCOMPARE(model.data(model.index(2, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(2, 2)).toInt(), 3);
    QCOMPARE(model.data(model.index(2, 3)), QVariant());

    QCOMPARE(model.data(model.index(3, 0)), QVariant());
    QCOMPARE(model.data(model.index(3, 1)), QVariant());
    QCOMPARE(model.data(model.index(3, 2)), QVariant());
    QCOMPARE(model.data(model.index(3, 3)), QVariant());
}

void tst_QSqlTableModel::setRecord()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QList<QSqlTableModel::EditStrategy> policies = QList<QSqlTableModel::EditStrategy>() << QSqlTableModel::OnFieldChange << QSqlTableModel::OnRowChange << QSqlTableModel::OnManualSubmit;

    QString Xsuffix;
    foreach( QSqlTableModel::EditStrategy submitpolicy, policies) {

        QSqlTableModel model(0, db);
        model.setEditStrategy((QSqlTableModel::EditStrategy)submitpolicy);
        model.setTable(test3);
        model.setSort(0, Qt::AscendingOrder);
        QVERIFY_SQL(model, select());

        for (int i = 0; i < model.rowCount(); ++i) {
            QSignalSpy spy(&model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

            QSqlRecord rec = model.record(i);
            rec.setValue(1, rec.value(1).toString() + 'X');
            rec.setValue(2, rec.value(2).toString() + 'X');
            QVERIFY(model.setRecord(i, rec));

            if ((QSqlTableModel::EditStrategy)submitpolicy == QSqlTableModel::OnManualSubmit)
                QVERIFY(model.submitAll());
            else if ((QSqlTableModel::EditStrategy)submitpolicy == QSqlTableModel::OnRowChange && i == model.rowCount() -1)
                model.submit();
            else {
                // dataChanged() is not emitted when submitAll() is called
                QCOMPARE(spy.count(), 2);
                QCOMPARE(spy.at(0).count(), 2);
                QCOMPARE(qvariant_cast<QModelIndex>(spy.at(0).at(0)), model.index(i, 1));
                QCOMPARE(qvariant_cast<QModelIndex>(spy.at(0).at(1)), model.index(i, 1));
            }
        }

        Xsuffix.append('X');

        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("foo").append(Xsuffix));
        QCOMPARE(model.data(model.index(0, 2)).toString(), QString("bar").append(Xsuffix));
        QCOMPARE(model.data(model.index(1, 1)).toString(), QString("baz").append(Xsuffix));
        QCOMPARE(model.data(model.index(1, 2)).toString(), QString("joe").append(Xsuffix));
    }
}

void tst_QSqlTableModel::insertRow()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setEditStrategy(QSqlTableModel::OnRowChange);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    QVERIFY_SQL(model, select());

    QVERIFY(model.insertRow(2));
    QSqlRecord rec = model.record(1);
    rec.setValue(0, 42);
    rec.setValue(1, QString("vohi"));
    QVERIFY(model.setRecord(2, rec));

    QCOMPARE(model.data(model.index(2, 0)).toInt(), 42);
    QCOMPARE(model.data(model.index(2, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(2, 2)).toInt(), 2);

    QVERIFY(model.submitAll());
}

void tst_QSqlTableModel::insertRecord()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    QVERIFY_SQL(model, select());

    QSqlRecord rec = model.record();
    rec.setValue(0, 42);
    rec.setValue(1, QString("vohi"));
    rec.setValue(2, 1);
    QVERIFY(model.insertRecord(1, rec));
    QCOMPARE(model.rowCount(), 4);

    QCOMPARE(model.data(model.index(1, 0)).toInt(), 42);
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(1, 2)).toInt(), 1);

    model.revertAll();
    model.setEditStrategy(QSqlTableModel::OnRowChange);

    QVERIFY(model.insertRecord(-1, rec));

    QCOMPARE(model.data(model.index(3, 0)).toInt(), 42);
    QCOMPARE(model.data(model.index(3, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(3, 2)).toInt(), 1);
}

void tst_QSqlTableModel::insertMultiRecords()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    QVERIFY_SQL(model, select());

    QCOMPARE(model.rowCount(), 3);

    QVERIFY(model.insertRow(2));

    QCOMPARE(model.data(model.index(2, 0)), QVariant());
    QCOMPARE(model.data(model.index(2, 1)), QVariant());
    QCOMPARE(model.data(model.index(2, 2)), QVariant());

    QVERIFY(model.insertRow(3));
    QVERIFY(model.insertRow(0));

    QCOMPARE(model.data(model.index(5, 0)).toInt(), 3);
    QCOMPARE(model.data(model.index(5, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(5, 2)).toInt(), 3);

    QVERIFY(model.setData(model.index(0, 0), QVariant(42)));
    QVERIFY(model.setData(model.index(3, 0), QVariant(43)));
    QVERIFY(model.setData(model.index(4, 0), QVariant(44)));
    QVERIFY(model.setData(model.index(4, 1), QVariant(QLatin1String("gunnar"))));
    QVERIFY(model.setData(model.index(4, 2), QVariant(1)));

    QVERIFY(model.submitAll());
    model.clear();
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    QVERIFY_SQL(model, select());

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(1, 0)).toInt(), 2);
    QCOMPARE(model.data(model.index(2, 0)).toInt(), 3);
    QCOMPARE(model.data(model.index(3, 0)).toInt(), 42);
    QCOMPARE(model.data(model.index(4, 0)).toInt(), 43);
    QCOMPARE(model.data(model.index(5, 0)).toInt(), 44);
    QCOMPARE(model.data(model.index(5, 1)).toString(), QString("gunnar"));
    QCOMPARE(model.data(model.index(5, 2)).toInt(), 1);
}

void tst_QSqlTableModel::submitAll()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QVERIFY_SQL(model, select());

    QVERIFY(model.setData(model.index(0, 1), "harry2", Qt::EditRole));
    QVERIFY(model.setData(model.index(1, 1), "trond2", Qt::EditRole));

    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry2"));
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond2"));

    QVERIFY_SQL(model, submitAll());

    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry2"));
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond2"));

    QVERIFY(model.setData(model.index(0, 1), "harry", Qt::EditRole));
    QVERIFY(model.setData(model.index(1, 1), "trond", Qt::EditRole));

    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond"));

    QVERIFY_SQL(model, submitAll());

    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond"));
}

void tst_QSqlTableModel::removeRow()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 3);

    // headerDataChanged must be emitted by the model when the edit strategy is OnManualSubmit,
    // when OnFieldChange or OnRowChange it's not needed because the model will re-select.
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    QSignalSpy headerDataChangedSpy(&model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)));

    QVERIFY(model.removeRow(1));
    QCOMPARE(headerDataChangedSpy.count(), 1);
    QCOMPARE(*static_cast<const Qt::Orientation *>(headerDataChangedSpy.at(0).value(0).constData()), Qt::Vertical);
    QCOMPARE(headerDataChangedSpy.at(0).at(1).toInt(), 1);
    QCOMPARE(headerDataChangedSpy.at(0).at(2).toInt(), 1);
    QVERIFY(model.submitAll());
    QCOMPARE(model.rowCount(), 2);

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(1, 0)).toInt(), 3);
    model.clear();

    recreateTestTables();

    model.setTable(test);
    model.setEditStrategy(QSqlTableModel::OnRowChange);
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 3);

    headerDataChangedSpy.clear();
    QVERIFY(model.removeRow(1));
    QCOMPARE(headerDataChangedSpy.count(), 0);
    QCOMPARE(model.rowCount(), 2);

    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("vohi"));
}

void tst_QSqlTableModel::removeRows()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setSort(0, Qt::AscendingOrder);
    model.setEditStrategy(QSqlTableModel::OnFieldChange);
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 3);

    QSignalSpy beforeDeleteSpy(&model, SIGNAL(beforeDelete(int)));
    QVERIFY_SQL(model, removeRows(0, 2));
    QVERIFY(beforeDeleteSpy.count() == 2);
    QVERIFY(beforeDeleteSpy.at(0).at(0).toInt() == 0);
    QVERIFY(beforeDeleteSpy.at(1).at(0).toInt() == 1);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("vohi"));
    model.clear();

    recreateTestTables();
    model.setTable(test);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 3);
    beforeDeleteSpy.clear();

    // When the edit strategy is OnManualSubmit the beforeDelete() signal
    // isn't emitted until submitAll() is called.
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    QSignalSpy headerDataChangedSpy(&model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)));
    QVERIFY(model.removeRows(0, 2, QModelIndex()));
    QCOMPARE(headerDataChangedSpy.count(), 2);
    QCOMPARE(headerDataChangedSpy.at(0).at(1).toInt(), 0);
    QCOMPARE(headerDataChangedSpy.at(0).at(2).toInt(), 0);
    QCOMPARE(headerDataChangedSpy.at(1).at(1).toInt(), 1);
    QCOMPARE(headerDataChangedSpy.at(1).at(2).toInt(), 1);
    QCOMPARE(model.rowCount(), 3);
    QVERIFY(beforeDeleteSpy.count() == 0);
    QVERIFY(model.submitAll());
    QVERIFY(beforeDeleteSpy.count() == 2);
    QVERIFY(beforeDeleteSpy.at(0).at(0).toInt() == 0);
    QVERIFY(beforeDeleteSpy.at(1).at(0).toInt() == 1);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("vohi"));
}

void tst_QSqlTableModel::removeInsertedRow()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    for (int i = 0; i <= 1; ++i) {

        QSqlTableModel model(0, db);
        model.setTable(test);
        model.setSort(0, Qt::AscendingOrder);

        model.setEditStrategy(i == 0
                ? QSqlTableModel::OnRowChange : QSqlTableModel::OnManualSubmit);
        QVERIFY_SQL(model, select());
        QCOMPARE(model.rowCount(), 3);

        QVERIFY(model.insertRow(1));
        QCOMPARE(model.rowCount(), 4);

        QVERIFY(model.removeRow(1));
        QCOMPARE(model.rowCount(), 3);

        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
        QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond"));
        QCOMPARE(model.data(model.index(2, 1)).toString(), QString("vohi"));
        model.clear();

        recreateTestTables();
    }
}

void tst_QSqlTableModel::emptyTable()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 0);

    model.setTable(qTableName("emptytable", __FILE__));
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 1);

    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 1);
}

void tst_QSqlTableModel::tablesAndSchemas()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQuery q(db);
    q.exec("DROP SCHEMA " + qTableName("testschema", __FILE__) + " CASCADE");
    QVERIFY_SQL( q, exec("create schema " + qTableName("testschema", __FILE__)));
    QString tableName = qTableName("testschema", __FILE__) + '.' + qTableName("testtable", __FILE__);
    QVERIFY_SQL( q, exec("create table " + tableName + "(id int)"));
    QVERIFY_SQL( q, exec("insert into " + tableName + " values(1)"));
    QVERIFY_SQL( q, exec("insert into " + tableName + " values(2)"));

    QSqlTableModel model(0, db);
    model.setTable(tableName);
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.columnCount(), 1);
}

void tst_QSqlTableModel::whitespaceInIdentifiers()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    if (!testWhiteSpaceNames(db.driverName()))
        QSKIP("DBMS doesn't support whitespaces in identifiers", SkipSingle);

    QString tableName = qTableName("qtestw hitespace", db.driver());

    QSqlTableModel model(0, db);
    model.setTable(tableName);
    QVERIFY_SQL(model, select());
}

void tst_QSqlTableModel::primaryKeyOrder()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQuery q(db);

    if(tst_Databases::isPostgreSQL(db))
        QVERIFY_SQL( q, exec("set client_min_messages='warning'"));

    QVERIFY_SQL( q, exec("create table "+qTableName("foo", __FILE__)+"(a varchar(20), id int not null primary key, b varchar(20))"));

    QSqlTableModel model(0, db);
    model.setTable(qTableName("foo", __FILE__));

    QSqlIndex pk = model.primaryKey();
    QCOMPARE(pk.count(), 1);
    QCOMPARE(pk.fieldName(0), QLatin1String("id"));

    QVERIFY(model.insertRow(0));
    QVERIFY(model.setData(model.index(0, 0), "hello"));
    QVERIFY(model.setData(model.index(0, 1), 42));
    QVERIFY(model.setData(model.index(0, 2), "blah"));
    QVERIFY_SQL(model, submitAll());

    QVERIFY(model.setData(model.index(0, 1), 43));
    QVERIFY_SQL(model, submitAll());

    QCOMPARE(model.data(model.index(0, 1)).toInt(), 43);
}

void tst_QSqlTableModel::setInvalidFilter()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    // set an invalid filter, make sure it fails
    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setFilter("blahfahsel");

    QCOMPARE(model.filter(), QString("blahfahsel"));
    QVERIFY(!model.select());

    // set a valid filter later, make sure if passes
    model.setFilter("id = 1");
    QVERIFY_SQL(model, select());
}

void tst_QSqlTableModel::setFilter()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setFilter("id = 1");
    QCOMPARE(model.filter(), QString("id = 1"));
    QVERIFY_SQL(model, select());

    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);

    QSignalSpy rowsRemovedSpy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));
    QSignalSpy rowsAboutToBeRemovedSpy(&model,
            SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsInsertedSpy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy rowsAboutToBeInsertedSpy(&model,
            SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    model.setFilter("id = 2");

    // check the signals
    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QList<QVariant> args = rowsAboutToBeRemovedSpy.takeFirst();
    QCOMPARE(args.count(), 3);
    QCOMPARE(qvariant_cast<QModelIndex>(args.at(0)), QModelIndex());
    QCOMPARE(args.at(1).toInt(), 0);
    QCOMPARE(args.at(2).toInt(), 0);
    args = rowsRemovedSpy.takeFirst();
    QCOMPARE(args.count(), 3);
    QCOMPARE(qvariant_cast<QModelIndex>(args.at(0)), QModelIndex());
    QCOMPARE(args.at(1).toInt(), 0);
    QCOMPARE(args.at(2).toInt(), 0);
    args = rowsInsertedSpy.takeFirst();
    QCOMPARE(args.count(), 3);
    QCOMPARE(qvariant_cast<QModelIndex>(args.at(0)), QModelIndex());
    QCOMPARE(args.at(1).toInt(), 0);
    QCOMPARE(args.at(2).toInt(), 0);
    args = rowsAboutToBeInsertedSpy.takeFirst();
    QCOMPARE(args.count(), 3);
    QCOMPARE(qvariant_cast<QModelIndex>(args.at(0)), QModelIndex());
    QCOMPARE(args.at(1).toInt(), 0);
    QCOMPARE(args.at(2).toInt(), 0);

    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0)).toInt(), 2);
}

void tst_QSqlTableModel::sqlite_bigTable()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);
    const QString bigtable(qTableName("bigtable", __FILE__));

    bool hasTransactions = db.driver()->hasFeature(QSqlDriver::Transactions);
    if (hasTransactions) QVERIFY(db.transaction());
    QSqlQuery q(db);
    QVERIFY_SQL( q, exec("create table "+bigtable+"(id int primary key, name varchar)"));
    QVERIFY_SQL( q, prepare("insert into "+bigtable+"(id, name) values (?, ?)"));
    QTime startTime;
    startTime.start();
    for (int i = 0; i < 10000; ++i) {
        q.addBindValue(i);
        q.addBindValue(QString::number(i));
        if(i%1000 == 0 && startTime.elapsed() > 5000)
            qDebug() << i << "records written";
        QVERIFY_SQL( q, exec());
    }
    q.clear();
    if (hasTransactions) QVERIFY(db.commit());

    QSqlTableModel model(0, db);
    model.setTable(bigtable);
    QVERIFY_SQL(model, select());

    QSqlRecord rec = model.record();
    rec.setValue("id", 424242);
    rec.setValue("name", "Guillaume");
    QVERIFY_SQL(model, insertRecord(-1, rec));

    model.clear();
}

// For task 118547: couldn't insert records unless select()
// had first been called.
void tst_QSqlTableModel::insertRecordBeforeSelect()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    QCOMPARE(model.lastError().type(), QSqlError::NoError);

    QSqlRecord buffer = model.record();
    buffer.setValue("id", 13);
    buffer.setValue("name", QString("The Lion King"));
    buffer.setValue("title", 0);
    QVERIFY_SQL(model, insertRecord(-1, buffer));

    buffer.setValue("id", 26);
    buffer.setValue("name", QString("T. Leary"));
    buffer.setValue("title", 0);
    QVERIFY_SQL(model, insertRecord(1, buffer));

    int rowCount = model.rowCount();
    model.clear();
    QCOMPARE(model.rowCount(), 0);

    QSqlTableModel model2(0, db);
    model2.setTable(test);
    QVERIFY_SQL(model2, select());
    QCOMPARE(model2.rowCount(), rowCount);
}

// For task 118547: set errors if table doesn't exist and if records
// are inserted and submitted on a non-existing table.
void tst_QSqlTableModel::submitAllOnInvalidTable()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);

    // setTable returns a void, so the error can only be caught by
    // manually checking lastError(). ### Qt5: This should be changed!
    model.setTable(qTableName("invalidTable", __FILE__));
    QCOMPARE(model.lastError().type(), QSqlError::StatementError);

    // This will give us an empty record which is expected behavior
    QSqlRecord buffer = model.record();
    buffer.setValue("bogus", 1000);
    buffer.setValue("bogus2", QString("I will go nowhere!"));

    // Inserting the record into the *model* will work (OnManualSubmit)
    QVERIFY_SQL(model, insertRecord(-1, buffer));

    // The submit and select shall fail because the table doesn't exist
    QEXPECT_FAIL("", "The table doesn't exist: submitAll() shall fail",
        Continue);
    QVERIFY_SQL(model, submitAll());
    QEXPECT_FAIL("", "The table doesn't exist: select() shall fail",
        Continue);
    QVERIFY_SQL(model, select());
}

// For task 147575: the rowsRemoved signal emitted from the model was lying
void tst_QSqlTableModel::insertRecordsInLoop()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.select();

    QSqlRecord record = model.record();
    record.setValue(0, 10);
    record.setValue(1, "Testman");
    record.setValue(2, 1);

    QSignalSpy spyRowsRemoved(&model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    QSignalSpy spyRowsInserted(&model, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    for (int i = 0; i < 10; i++) {
        QVERIFY(model.insertRecord(model.rowCount(), record));
        QCOMPARE(spyRowsInserted.at(i).at(1).toInt(), i+3); // The table already contains three rows
        QCOMPARE(spyRowsInserted.at(i).at(2).toInt(), i+3);
    }
    model.submitAll(); // submitAll() calls select() which clears and repopulates the table

    int firstRowIndex = 0, lastRowIndex = 12;
    QCOMPARE(spyRowsRemoved.count(), 1);
    QCOMPARE(spyRowsRemoved.at(0).at(1).toInt(), firstRowIndex);
    QCOMPARE(spyRowsRemoved.at(0).at(2).toInt(), lastRowIndex);

    QCOMPARE(spyRowsInserted.at(10).at(1).toInt(), firstRowIndex);
    QCOMPARE(spyRowsInserted.at(10).at(2).toInt(), lastRowIndex);
    QCOMPARE(spyRowsInserted.count(), 11);

    QCOMPARE(model.rowCount(), 13);
    QCOMPARE(model.columnCount(), 3);
}

void tst_QSqlTableModel::sqlite_attachedDatabase()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);
    if(db.databaseName() == ":memory:")
        QSKIP(":memory: database, skipping test", SkipSingle);

    QSqlDatabase attachedDb = QSqlDatabase::cloneDatabase(db, db.driverName() + QLatin1String("attached"));
    attachedDb.setDatabaseName(db.databaseName()+QLatin1String("attached.dat"));
    QVERIFY_SQL(attachedDb, open());
    QSqlQuery q(attachedDb);
    tst_Databases::safeDropTables(attachedDb, QStringList() << "atest" << "atest2");
    QVERIFY_SQL( q, exec("CREATE TABLE atest(id int, text varchar(20))"));
    QVERIFY_SQL( q, exec("CREATE TABLE atest2(id int, text varchar(20))"));
    QVERIFY_SQL( q, exec("INSERT INTO atest VALUES(1, 'attached-atest')"));
    QVERIFY_SQL( q, exec("INSERT INTO atest2 VALUES(2, 'attached-atest2')"));

    QSqlQuery q2(db);
    tst_Databases::safeDropTable(db, "atest");
    QVERIFY_SQL(q2, exec("CREATE TABLE atest(id int, text varchar(20))"));
    QVERIFY_SQL(q2, exec("INSERT INTO atest VALUES(3, 'main')"));
    QVERIFY_SQL(q2, exec("ATTACH DATABASE \""+attachedDb.databaseName()+"\" as adb"));

    // This should query the table in the attached database (schema supplied)
    QSqlTableModel model(0, db);
    model.setTable("adb.atest");
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1), Qt::DisplayRole).toString(), QLatin1String("attached-atest"));

    // This should query the table in the attached database (unique tablename)
    model.setTable("atest2");
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toInt(), 2);
    QCOMPARE(model.data(model.index(0, 1), Qt::DisplayRole).toString(), QLatin1String("attached-atest2"));

    // This should query the table in the main database (tables in main db has 1st priority)
    model.setTable("atest");
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toInt(), 3);
    QCOMPARE(model.data(model.index(0, 1), Qt::DisplayRole).toString(), QLatin1String("main"));
}


void tst_QSqlTableModel::tableModifyWithBlank()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(qTableName("test4", __FILE__));
    model.select();

    //generate a time stamp for the test. Add one second to the current time to make sure
    //it is different than the QSqlQuery test.
    QString timeString=QDateTime::currentDateTime().addSecs(1).toString(Qt::ISODate);

    //insert a new row, with column0 being the timestamp.
    //Should be equivalent to QSqlQuery INSERT INTO... command)
    QVERIFY_SQL(model, insertRow(0));
    QVERIFY_SQL(model, setData(model.index(0,0),timeString));
    QVERIFY_SQL(model, submitAll());

    //set a filter on the table so the only record we get is the one we just made
    //I could just do another setData command, but I want to make sure the TableModel
    //matches exactly what is stored in the database
    model.setFilter("column1='"+timeString+"'"); //filter to get just the newly entered row
    QVERIFY_SQL(model, select());

    //Make sure we only get one record, and that it is the one we just made
    QCOMPARE(model.rowCount(), 1); //verify only one entry
    QCOMPARE(model.record(0).value(0).toString(), timeString); //verify correct record

    //At this point we know that the intial value (timestamp) was succsefully stored in the database
    //Attempt to modify the data in the new record
    //equivalent to query.exec("update test set column3="... command in direct test
    //set the data in the first column to "col1ModelData"
    QVERIFY_SQL(model, setData(model.index(0,1), "col1ModelData"));

    //do a quick check to make sure that the setData command properly set the value in the model
    QCOMPARE(model.record(0).value(1).toString(), QLatin1String("col1ModelData"));

    //submit the changed data to the database
    //This is where I have been getting errors.
    QVERIFY_SQL(model, submitAll());

    //make sure the model has the most current data for our record
    QVERIFY_SQL(model, select());

    //verify that our new record was the only record returned
    QCOMPARE(model.rowCount(), 1);

    //And that the record returned is, in fact, our test record.
    QCOMPARE(model.record(0).value(0).toString(), timeString);

    //Make sure the value of the first column matches what we set it to previously.
    QCOMPARE(model.record(0).value(1).toString(), QLatin1String("col1ModelData"));
}

void tst_QSqlTableModel::removeColumnAndRow()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.columnCount(), 3);

    QVERIFY(model.removeColumn(0));
    QVERIFY(model.removeRow(0));
    QVERIFY(model.submitAll());
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.columnCount(), 2);

    // check with another table because the model has been modified
    // but not the sql table
    QSqlTableModel model2(0, db);
    model2.setTable(test);
    QVERIFY_SQL(model2, select());
    QCOMPARE(model2.rowCount(), 2);
    QCOMPARE(model2.columnCount(), 3);
}

void tst_QSqlTableModel::insertBeforeDelete()
{
    QFETCH(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlQuery q(db);
    QVERIFY_SQL( q, exec("insert into " + test + " values(9, 'andrew', 9)"));
    QVERIFY_SQL( q, exec("insert into " + test + " values(10, 'justin', 10)"));

    QSqlTableModel model(0, db);
    model.setTable(test);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QVERIFY_SQL(model, select());

    QSqlRecord rec = model.record();
    rec.setValue(0, 4);
    rec.setValue(1, QString("bill"));
    rec.setValue(2, 4);
    QVERIFY_SQL(model, insertRecord(4, rec));

    QVERIFY_SQL(model, removeRow(5));
    QVERIFY_SQL(model, submitAll());
    QCOMPARE(model.rowCount(), 5);
}

QTEST_MAIN(tst_QSqlTableModel)
#include "tst_qsqltablemodel.moc"
