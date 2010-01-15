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
/* possible connection parameters */

#ifndef TST_DATABASES_H
#define TST_DATABASES_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QRegExp>
#include <QDir>
#include <QVariant>
#include <QDebug>

#include <QtTest/QtTest>

#if defined (Q_OS_WIN) || defined (Q_OS_WIN32)
#  include <qt_windows.h>
#  if defined (Q_OS_WINCE)
#    include <winsock2.h>
#  endif
#else
#include <unistd.h>
#endif

#define CHECK_DATABASE( db ) \
    if ( !db.isValid() ) { qFatal( "db is Invalid" ); }

#define QVERIFY_SQL(q, stmt) QVERIFY2((q).stmt, tst_Databases::printError((q).lastError(), db))
#define QFAIL_SQL(q, stmt) QVERIFY2(!(q).stmt, tst_Databases::printError((q).lastError(), db))

#define DBMS_SPECIFIC(db, driver) \
    if (!db.driverName().startsWith(driver)) { QSKIP(driver " specific test", SkipSingle); return; }

// ### use QSystem::hostName if it is integrated in qtest/main
static QString qGetHostName()
{
    static QString hostname;

    if ( !hostname.isEmpty() )
        return hostname;

    char hn[257];

    if ( gethostname( hn, 255 ) == 0 ) {
        hn[256] = '\0';
        hostname = QString::fromLatin1( hn );
        hostname.replace( QLatin1Char( '.' ), QLatin1Char( '_' ) );
        hostname.replace( QLatin1Char( '-' ), QLatin1Char( '_' ) );
    }

    return hostname;
}

// to prevent nameclashes on our database server, each machine
// will use its own set of table names. Call this function to get
// "tablename_hostname"
inline static QString qTableName( const QString& prefix, QSqlDriver* driver = 0 )
{
    if ( !driver )
        return prefix + "_" + qGetHostName().replace( "-", "_" );
    else
        return driver->escapeIdentifier( prefix + "_" + qGetHostName(), QSqlDriver::TableName );
}

inline static bool testWhiteSpaceNames( const QString &name )
{
/*    return name.startsWith( "QPSQL" )
           || name.startsWith( "QODBC" )
           || name.startsWith( "QSQLITE" )
           || name.startsWith( "QMYSQL" );*/
    return name != QLatin1String("QSQLITE2");
}

inline static QString toHex( const QString& binary )
{
    QString str;
    static char const hexchars[] = "0123456789ABCDEF";

    for ( int i = 0; i < binary.size(); i++ ) {
        ushort code = binary.at(i).unicode();
        str += (QChar)(hexchars[ (code >> 12) & 0x0F ]);
        str += (QChar)(hexchars[ (code >> 8) & 0x0F ]);
        str += (QChar)(hexchars[ (code >> 4) & 0x0F ]);
        str += (QChar)(hexchars[ code & 0x0F ]);
    }

    return str;
}


class tst_Databases
{

public:
    tst_Databases(): counter( 0 )
    {
    }

    ~tst_Databases()
    {
        close();
    }

    // returns a testtable consisting of the names of all database connections if
    // driverPrefix is empty, otherwise only those that start with driverPrefix.
    int fillTestTable( const QString& driverPrefix = QString() ) const
    {
        QTest::addColumn<QString>( "dbName" );
        int count = 0;

        for ( int i = 0; i < dbNames.count(); ++i ) {
            QSqlDatabase db = QSqlDatabase::database( dbNames.at( i ) );

            if ( !db.isValid() )
                continue;

            if ( driverPrefix.isEmpty() || db.driverName().startsWith( driverPrefix ) ) {
                QTest::newRow( dbNames.at( i ).toLatin1() ) << dbNames.at( i );
                ++count;
            }
        }

        return count;
    }

    void addDb( const QString& driver, const QString& dbName,
                const QString& user = QString(), const QString& passwd = QString(),
                const QString& host = QString(), int port = -1, const QString params = QString() )
    {
        QSqlDatabase db;

        if ( !QSqlDatabase::drivers().contains( driver ) ) {
            qWarning() <<  "Driver" << driver << "is not installed";
            return;
        }

        // construct a stupid unique name
        QString cName = QString::number( counter++ ) + "_" + driver + "@";

        cName += host.isEmpty() ? dbName : host;

        if ( port > 0 )
            cName += ":" + QString::number( port );

        db = QSqlDatabase::addDatabase( driver, cName );

        if ( !db.isValid() ) {
            qWarning( "Could not create database object" );
            return;
        }

        db.setDatabaseName( dbName );

        db.setUserName( user );
        db.setPassword( passwd );
        db.setHostName( host );
        db.setPort( port );
        db.setConnectOptions( params );
        dbNames.append( cName );
    }

    void addDbs()
    {
//         addDb( "QOCI8", "//horsehead.nokia.troll.no:1521/pony.troll.no", "scott", "tiger" ); // Oracle 9i on horsehead
//         addDb( "QOCI8", "//horsehead.nokia.troll.no:1521/ustest.troll.no", "scott", "tiger", "" ); // Oracle 9i on horsehead
//         addDb( "QOCI8", "//iceblink.nokia.troll.no:1521/ice.troll.no", "scott", "tiger", "" ); // Oracle 8 on iceblink (not currently working)
//         addDb( "QOCI", "//silence.nokia.troll.no:1521/testdb", "scott", "tiger" ); // Oracle 10g on silence
//         addDb( "QOCI", "//oracle10g-nokia.trolltech.com.au:1521/XE", "scott", "tiger" ); // Oracle 10gexpress on xen

//      This requires a local ODBC data source to be configured( pointing to a MySql database )
//         addDb( "QODBC", "mysqlodbc", "troll", "trond" );
//         addDb( "QODBC", "SqlServer", "troll", "trond" );
//         addDb( "QTDS7", "testdb", "troll", "trondk", "horsehead" );
//         addDb( "QODBC", "silencetestdb", "troll", "trond", "silence" );
//         addDb( "QODBC", "horseheadtestdb", "troll", "trondk", "horsehead" );

//         addDb( "QMYSQL3", "testdb", "troll", "trond", "horsehead.nokia.troll.no" );
//         addDb( "QMYSQL3", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 3307 );
//         addDb( "QMYSQL3", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 3308, "CLIENT_COMPRESS=1;CLIENT_SSL=1" ); // MySQL 4.1.1
//         addDb( "QMYSQL3", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 3309, "CLIENT_COMPRESS=1;CLIENT_SSL=1" ); // MySQL 5.0.18 Linux
//         addDb( "QMYSQL3", "testdb", "troll", "trond", "silence.nokia.troll.no" ); // MySQL 5.1.36 Windows
//         addDb( "QMYSQL3", "testdb", "testuser", "Ee4Gabf6_", "mysql4-nokia.trolltech.com.au" ); // MySQL 4.1.22-2.el4  linux
//         addDb( "QMYSQL3", "testdb", "testuser", "Ee4Gabf6_", "bq-mysql50.apac.nokia.com" ); // MySQL 5.0.45-7.el5 linux
//         addDb( "QMYSQL3", "testdb", "testuser", "Ee4Gabf6_", "bq-mysql51.apac.nokia.com" ); // MySQL 5.1.36-6.7.2.i586 linux

//         addDb( "QPSQL7", "testdb", "troll", "trond", "horsehead.nokia.troll.no" ); // V7.2 NOT SUPPORTED!
//         addDb( "QPSQL7", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 5434 ); // V7.2 NOT SUPPORTED! Multi-byte
//         addDb( "QPSQL7", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 5435 ); // V7.3
//         addDb( "QPSQL7", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 5436 ); // V7.4
//         addDb( "QPSQL7", "testdb", "troll", "trond", "horsehead.nokia.troll.no", 5437 ); // V8.0.3
//         addDb( "QPSQL7", "testdb", "troll", "trond", "silence.nokia.troll.no" );         // V8.2.1, UTF-8
//         addDb( "QPSQL7", "testdb", "testuser", "Ee4Gabf6_", "postgres74-nokia.trolltech.com.au" );         // Version 7.4.19-1.el4_6.1
//         addDb( "QPSQL7", "testdb", "testuser", "Ee4Gabf6_", "bq-pgsql81.apac.nokia.com" );         // Version 8.1.11-1.el5_1.1
//         addDb( "QPSQL7", "testdb", "testuser", "Ee4Gabf6_", "bq-pgsql84.apac.nokia.com" );         // Version 8.4.1-2.1.i586

//         addDb( "QDB2", "testdb", "troll", "trond", "silence.nokia.troll.no" ); // DB2 v9.1 on silence

//      yes - interbase really wants the physical path on the host machine.
//         addDb( "QIBASE", "/opt/interbase/qttest.gdb", "SYSDBA", "masterkey", "horsehead.nokia.troll.no" );
//         addDb( "QIBASE", "silence.troll.no:c:\\ibase\\testdb", "SYSDBA", "masterkey", "" ); // InterBase 7.5 on silence
//         addDb( "QIBASE", "silence.troll.no:c:\\ibase\\testdb_ascii", "SYSDBA", "masterkey", "" ); // InterBase 7.5 on silence
//         addDb( "QIBASE", "/opt/firebird/databases/testdb.fdb", "testuser", "Ee4Gabf6_", "firebird1-nokia.trolltech.com.au" ); // Firebird 1.5.5
//         addDb( "QIBASE", "/opt/firebird/databases/testdb.fdb", "testuser", "Ee4Gabf6_", "firebird2-nokia.trolltech.com.au" ); // Firebird 2.1.1

//      use in-memory database to prevent local files
//         addDb("QSQLITE", ":memory:");
         addDb( "QSQLITE", QDir::toNativeSeparators(QDir::tempPath()+"/foo.db") );
//         addDb( "QSQLITE2", QDir::toNativeSeparators(QDir::tempPath()+"/foo2.db") );
//         addDb( "QODBC3", "DRIVER={SQL SERVER};SERVER=iceblink.nokia.troll.no\\ICEBLINK", "troll", "trond", "" );
//         addDb( "QODBC3", "DRIVER={SQL Native Client};SERVER=silence.nokia.troll.no\\SQLEXPRESS", "troll", "trond", "" );

//         addDb( "QODBC", "DRIVER={MySQL ODBC 5.1 Driver};SERVER=mysql5-nokia.trolltech.com.au;DATABASE=testdb", "testuser", "Ee4Gabf6_", "" );
//         addDb( "QODBC", "DRIVER={MySQL ODBC 5.1 Driver};SERVER=mysql4-nokia.trolltech.com.au;DATABASE=testdb", "testuser", "Ee4Gabf6_", "" );
//         addDb( "QODBC", "DRIVER={FreeTDS};SERVER=horsehead.nokia.troll.no;DATABASE=testdb;PORT=4101;UID=troll;PWD=trondk", "troll", "trondk", "" );
//         addDb( "QODBC", "DRIVER={FreeTDS};SERVER=silence.nokia.troll.no;DATABASE=testdb;PORT=2392;UID=troll;PWD=trond", "troll", "trond", "" );
//         addDb( "QODBC", "DRIVER={FreeTDS};SERVER=bq-winserv2003-x86-01.apac.nokia.com;DATABASE=testdb;PORT=1433;UID=testuser;PWD=Ee4Gabf6_;TDS_Version=8.0", "", "", "" );
//         addDb( "QODBC", "DRIVER={FreeTDS};SERVER=bq-winserv2008-x86-01.apac.nokia.com;DATABASE=testdb;PORT=1433;UID=testuser;PWD=Ee4Gabf6_;TDS_Version=8.0", "", "", "" );
//         addDb( "QTDS7", "testdb", "testuser", "Ee4Gabf6_", "bq-winserv2003" );
//         addDb( "QTDS7", "testdb", "testuser", "Ee4Gabf6_", "bq-winserv2008" );
//         addDb( "QODBC3", "DRIVER={SQL SERVER};SERVER=bq-winserv2003-x86-01.apac.nokia.com;DATABASE=testdb;PORT=1433", "testuser", "Ee4Gabf6_", "" );
//         addDb( "QODBC3", "DRIVER={SQL SERVER};SERVER=bq-winserv2008-x86-01.apac.nokia.com;DATABASE=testdb;PORT=1433", "testuser", "Ee4Gabf6_", "" );
//         addDb( "QODBC", "DRIVER={Microsoft Access Driver (*.mdb)};DBQ=c:\\dbs\\access\\testdb.mdb", "", "", "" );
//         addDb( "QODBC", "DRIVER={Postgresql};SERVER=postgres81-nokia.trolltech.com.au;DATABASE=testdb", "testuser", "Ee4Gabf6_", "" );
    }

    void open()
    {
        addDbs();

        QStringList::Iterator it = dbNames.begin();

        while ( it != dbNames.end() ) {
            QSqlDatabase db = QSqlDatabase::database(( *it ), false );
            qDebug() << "Opening:" << (*it);

            if ( db.isValid() && !db.isOpen() ) {
                if ( !db.open() ) {
                    qWarning( "tst_Databases: Unable to open %s on %s:\n%s", qPrintable( db.driverName() ), qPrintable( *it ), qPrintable( db.lastError().databaseText() ) );
                    // well... opening failed, so we just ignore the server, maybe it is not running
                    it = dbNames.erase( it );
                } else {
                    ++it;
                }
            }
        }
    }

    void close()
    {
        for ( QStringList::Iterator it = dbNames.begin(); it != dbNames.end(); ++it ) {
            {
                QSqlDatabase db = QSqlDatabase::database(( *it ), false );

                if ( db.isValid() && db.isOpen() )
                    db.close();
            }

            QSqlDatabase::removeDatabase(( *it ) );
        }

        dbNames.clear();
    }

    // for debugging only: outputs the connection as string
    static QString dbToString( const QSqlDatabase db )
    {
        QString res = db.driverName() + "@";

        if ( db.driverName().startsWith( "QODBC" ) || db.driverName().startsWith( "QOCI" ) ) {
            res += db.databaseName();
        } else {
            res += db.hostName();
        }

        if ( db.port() > 0 ) {
            res += ":" + QString::number( db.port() );
        }

        return res;
    }

    // drop a table only if it exists to prevent warnings
    static void safeDropTables( QSqlDatabase db, const QStringList& tableNames )
    {
        bool wasDropped;
        QSqlQuery q( db );
        QStringList dbtables=db.tables();

        foreach(const QString &tableName, tableNames)
        {
            wasDropped = true;
            QString table=tableName;
            if ( db.driver()->isIdentifierEscaped(table, QSqlDriver::TableName))
                table = db.driver()->stripDelimiters(table, QSqlDriver::TableName);

            if ( dbtables.contains( table, Qt::CaseInsensitive ) ) {
                foreach(const QString &table2, dbtables.filter(table, Qt::CaseInsensitive)) {
                    if(table2.compare(table.section('.', -1, -1), Qt::CaseInsensitive) == 0) {
                        table=db.driver()->escapeIdentifier(table2, QSqlDriver::TableName);
                        if(db.driverName().startsWith( "QPSQL" ))
                            wasDropped = q.exec( "drop table " + table + " cascade");
                        else
                            wasDropped = q.exec( "drop table " + table);
                        dbtables.removeAll(table2);
                    }
                }
            }
            if ( !wasDropped ) {
                qWarning() << dbToString(db) << "unable to drop table" << tableName << ':' << q.lastError();
//              qWarning() << "last query:" << q.lastQuery();
//              qWarning() << "dbtables:" << dbtables;
//              qWarning() << "db.tables():" << db.tables();
            }
        }
    }

    static void safeDropTable( QSqlDatabase db, const QString& tableName )
    {
        safeDropTables(db, QStringList() << tableName);
    }

    static void safeDropViews( QSqlDatabase db, const QStringList &viewNames )
    {
        if ( isMSAccess( db ) ) // Access is sooo stupid.
            safeDropTables( db, viewNames );

        bool wasDropped;
        QSqlQuery q( db );
        QStringList dbtables=db.tables(QSql::Views);

        foreach(QString viewName, viewNames)
        {
            wasDropped = true;
            QString view=viewName;
            if ( db.driver()->isIdentifierEscaped(view, QSqlDriver::TableName))
                view = db.driver()->stripDelimiters(view, QSqlDriver::TableName);

            if ( dbtables.contains( view, Qt::CaseInsensitive ) ) {
                foreach(const QString &view2, dbtables.filter(view, Qt::CaseInsensitive)) {
                    if(view2.compare(view.section('.', -1, -1), Qt::CaseInsensitive) == 0) {
                        view=db.driver()->escapeIdentifier(view2, QSqlDriver::TableName);
                        wasDropped = q.exec( "drop view " + view);
                        dbtables.removeAll(view);
                    }
                }
            }

            if ( !wasDropped )
                qWarning() << dbToString(db) << "unable to drop view" << viewName << ':' << q.lastError();
//                  << "\nlast query:" << q.lastQuery()
//                  << "\ndbtables:" << dbtables
//                  << "\ndb.tables(QSql::Views):" << db.tables(QSql::Views);
        }
    }

    static void safeDropView( QSqlDatabase db, const QString& tableName )
    {
        safeDropViews(db, QStringList() << tableName);
    }

    // returns the type name of the blob datatype for the database db.
    // blobSize is only used if the db doesn't have a generic blob type
    static QString blobTypeName( QSqlDatabase db, int blobSize = 10000 )
    {
        if ( db.driverName().startsWith( "QMYSQL" ) )
            return "longblob";

        if ( db.driverName().startsWith( "QPSQL" ) )
            return "bytea";

        if ( db.driverName().startsWith( "QTDS" )
                || isSqlServer( db )
                || isMSAccess( db ) )
            return "image";

        if ( db.driverName().startsWith( "QDB2" ) )
            return QString( "blob(%1)" ).arg( blobSize );

        if ( db.driverName().startsWith( "QIBASE" ) )
            return QString( "blob sub_type 0 segment size 4096" );

        if ( db.driverName().startsWith( "QOCI" )
                || db.driverName().startsWith( "QSQLITE" ) )
            return "blob";

        qDebug() <<  "tst_Databases::blobTypeName: Don't know the blob type for" << dbToString( db );

        return "blob";
    }

    static QString autoFieldName( QSqlDatabase db )
    {
        if ( db.driverName().startsWith( "QMYSQL" ) )
            return "AUTO_INCREMENT";
        if ( db.driverName().startsWith( "QTDS" ) )
            return "IDENTITY";
/*        if ( db.driverName().startsWith( "QPSQL" ) )
            return "SERIAL";*/
//        if ( db.driverName().startsWith( "QDB2" ) )
//            return "GENERATED BY DEFAULT AS IDENTITY";

        return QString();
    }

    static QByteArray printError( const QSqlError& err )
    {
        QString result;
        if(err.number() > 0)
            result += '(' + QString::number(err.number()) + ") ";
        result += '\'';
        if(!err.driverText().isEmpty())
            result += err.driverText() + "' || '";
        result += err.databaseText() + "'";
        return result.toLocal8Bit();
    }

    static QByteArray printError( const QSqlError& err, const QSqlDatabase& db )
    {
        QString result(dbToString(db) + ": ");
        if(err.number() > 0)
            result += '(' + QString::number(err.number()) + ") ";
        result += '\'';
        if(!err.driverText().isEmpty())
            result += err.driverText() + "' || '";
        result += err.databaseText() + "'";
        return result.toLocal8Bit();
    }

    static bool isSqlServer( QSqlDatabase db )
    {
        return db.databaseName().contains( "sql server", Qt::CaseInsensitive )
               || db.databaseName().contains( "sqlserver", Qt::CaseInsensitive )
               || db.databaseName().contains( "sql native client", Qt::CaseInsensitive )
               || db.databaseName().contains( "bq-winserv", Qt::CaseInsensitive )
               || db.hostName().contains( "bq-winserv", Qt::CaseInsensitive );
    }

    static bool isMSAccess( QSqlDatabase db )
    {
        return db.databaseName().contains( "Access Driver", Qt::CaseInsensitive );
    }

    static bool isPostgreSQL( QSqlDatabase db )
    {
        return db.driverName().startsWith("QPSQL") || (db.driverName().startsWith("QODBC") && db.databaseName().contains("PostgreSQL") );
    }

    static bool isMySQL( QSqlDatabase db )
    {
        return db.driverName().startsWith("QMYSQL") || (db.driverName().startsWith("QODBC") && db.databaseName().contains("MySQL") );
    }
    static bool isDB2( QSqlDatabase db )
    {
        return db.driverName().startsWith("QDB2") || (db.driverName().startsWith("QODBC") && db.databaseName().contains("db2") );
    }

    // -1 on fail, else Oracle version
    static int getOraVersion( QSqlDatabase db )
    {
        int ver = -1;
        QSqlQuery q( "SELECT banner FROM v$version", db );
        q.next();

        QRegExp vers( "([0-9]+)\\.[0-9\\.]+[0-9]" );

        if ( vers.indexIn( q.value( 0 ).toString() ) ) {
            bool ok;
            ver = vers.cap( 1 ).toInt( &ok );

            if ( !ok )
                ver = -1;
        }

        return ver;
    }

    static QString getMySqlVersion( const QSqlDatabase &db )
    {
        QSqlQuery q(db);
        q.exec( "select version()" );
        if(q.next())
            return q.value( 0 ).toString();
        else
            return QString();
    }

    static QString getPSQLVersion( const QSqlDatabase &db )
    {
        QSqlQuery q(db);
        q.exec( "select version()" );
        if(q.next())
            return q.value( 0 ).toString();
        else
            return QString();
    }

    QStringList     dbNames;
    int      counter;
};

#endif

