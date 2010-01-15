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
#include <QtSql/QtSql>

#include "../qsqldatabase/tst_databases.h"



//TESTED_CLASS=
//TESTED_FILES=

class tst_QSqlDriver : public QObject
{
    Q_OBJECT

public:
    void recreateTestTables(QSqlDatabase);

    tst_Databases dbs;

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void record();
    void primaryIndex();
};


void tst_QSqlDriver::initTestCase_data()
{
    dbs.open();
    if (dbs.fillTestTable() == 0) {
        qWarning("NO DATABASES");
        QSKIP("No database drivers are available in this Qt configuration", SkipAll);
    }
}

void tst_QSqlDriver::recreateTestTables(QSqlDatabase db)
{
    QSqlQuery q(db);

    if(tst_Databases::isPostgreSQL(db))
        QVERIFY_SQL( q, exec("set client_min_messages='warning'"));

    tst_Databases::safeDropTable( db, qTableName( "relTEST1" ) );

    QVERIFY_SQL( q, exec("create table " + qTableName("relTEST1") +
            " (id int not null primary key, name varchar(20), title_key int, another_title_key int)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("relTEST1") + " values(1, 'harry', 1, 2)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("relTEST1") + " values(2, 'trond', 2, 1)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("relTEST1") + " values(3, 'vohi', 1, 2)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("relTEST1") + " values(4, 'boris', 2, 2)"));
}

void tst_QSqlDriver::initTestCase()
{
    foreach (const QString &dbname, dbs.dbNames)
        recreateTestTables(QSqlDatabase::database(dbname));
}

void tst_QSqlDriver::cleanupTestCase()
{
    foreach (const QString &dbName, dbs.dbNames) {
        QSqlDatabase db = QSqlDatabase::database(dbName);
        tst_Databases::safeDropTable( db, qTableName( "relTEST1" ) );
    }
    dbs.close();
}

void tst_QSqlDriver::init()
{
}

void tst_QSqlDriver::cleanup()
{
}

void tst_QSqlDriver::record()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QString tablename =  qTableName("relTEST1");
    QStringList fields;
    fields << "id" << "name" << "title_key" << "another_title_key";

    //check we can get records using an unquoted mixed case table name
    QSqlRecord rec = db.driver()->record(tablename);
    QCOMPARE(rec.count(), 4);

    if (db.driverName().startsWith("QIBASE")|| db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        for(int i = 0; i < fields.count(); ++i)
            fields[i] = fields[i].toUpper();

    for (int i = 0; i < fields.count(); ++i)
        QCOMPARE(rec.fieldName(i), fields[i]);

    if (db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        tablename = tablename.toUpper();
    else if (db.driverName().startsWith("QPSQL"))
        tablename = tablename.toLower();

    if(!db.driverName().startsWith("QODBC") && !db.databaseName().contains("PostgreSql")) {
        //check we can get records using a properly quoted table name
        rec = db.driver()->record(db.driver()->escapeIdentifier(tablename,QSqlDriver::TableName));
        QCOMPARE(rec.count(), 4);
    }

    for (int i = 0; i < fields.count(); ++i)
        QCOMPARE(rec.fieldName(i), fields[i]);

    if( db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        tablename = tablename.toLower();
    else if (db.driverName().startsWith("QPSQL"))
        tablename = tablename.toUpper();

    //check that we can't get records using incorrect tablename casing that's been quoted
    rec = db.driver()->record(db.driver()->escapeIdentifier(tablename,QSqlDriver::TableName));
    if (tst_Databases::isMySQL(db)
      || db.driverName().startsWith("QSQLITE")
      || db.driverName().startsWith("QTDS")
      || tst_Databases::isSqlServer(db)
      || tst_Databases::isMSAccess(db))
        QCOMPARE(rec.count(), 4); //mysql, sqlite and tds will match
    else
        QCOMPARE(rec.count(), 0);

}

void tst_QSqlDriver::primaryIndex()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QString tablename =  qTableName("relTEST1");
    //check that we can get primary index using unquoted mixed case table name
    QSqlIndex index = db.driver()->primaryIndex(tablename);
    QCOMPARE(index.count(), 1);

    if( db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        QCOMPARE(index.fieldName(0), QString::fromLatin1("ID"));
    else
        QCOMPARE(index.fieldName(0), QString::fromLatin1("id"));


    //check that we can get the primary index using a quoted tablename
    if( db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        tablename = tablename.toUpper();
    else if (db.driverName().startsWith("QPSQL"))
        tablename = tablename.toLower();

    if(!db.driverName().startsWith("QODBC") && !db.databaseName().contains("PostgreSql")) {
        index = db.driver()->primaryIndex(db.driver()->escapeIdentifier(tablename, QSqlDriver::TableName));
        QCOMPARE(index.count(), 1);
    }
    if( db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        QCOMPARE(index.fieldName(0), QString::fromLatin1("ID"));
    else
        QCOMPARE(index.fieldName(0), QString::fromLatin1("id"));



    //check that we can not get the primary index using a quoted but incorrect table name casing
    if( db.driverName().startsWith("QIBASE") || db.driverName().startsWith("QOCI") || db.driverName().startsWith("QDB2"))
        tablename = tablename.toLower();
    else if (db.driverName().startsWith("QPSQL"))
        tablename = tablename.toUpper();

    index = db.driver()->primaryIndex(db.driver()->escapeIdentifier(tablename, QSqlDriver::TableName));
    if (tst_Databases::isMySQL(db)
      || db.driverName().startsWith("QSQLITE")
      || db.driverName().startsWith("QTDS")
      || tst_Databases::isSqlServer(db)
      || tst_Databases::isMSAccess(db))
        QCOMPARE(index.count(), 1); //mysql will always find the table name regardless of casing
    else
        QCOMPARE(index.count(), 0);
}

QTEST_MAIN(tst_QSqlDriver)
#include "tst_qsqldriver.moc"
