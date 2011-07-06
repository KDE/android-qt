/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>


#include <qfile.h>
#include <qdir.h>
#include <qcoreapplication.h>
#include <qlibrary.h>
#include <qtemporaryfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#ifdef Q_OS_UNIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#endif
#ifdef Q_OS_WIN
#define _WIN32_WINNT  0x500
#include <qt_windows.h>
#include <qlibrary.h>
#include <lm.h>
#endif
#include <qplatformdefs.h>
#include <qdebug.h>
#ifdef Q_OS_SYMBIAN
#include <f32file.h>
#include <private/qcore_symbian_p.h>
#endif
#include "../network-settings.h"
#include <private/qfileinfo_p.h>
#include "../../shared/filesystem.h"

#if defined(Q_OS_SYMBIAN)
# define SRCDIR ""
# define NO_SYMLINKS
#endif

QT_BEGIN_NAMESPACE
extern Q_AUTOTEST_EXPORT bool qIsLikelyToBeNfs(int /* handle */);
QT_END_NAMESPACE

//TESTED_CLASS=
//TESTED_FILES=

class tst_QFileInfo : public QObject
{
Q_OBJECT

public:
    tst_QFileInfo();
    ~tst_QFileInfo();

private slots:
    void getSetCheck();

    void copy();

    void isFile_data();
    void isFile();

    void isDir_data();
    void isDir();

    void isRoot_data();
    void isRoot();

    void exists_data();
    void exists();

    void absolutePath_data();
    void absolutePath();

    void absFilePath_data();
    void absFilePath();

    void canonicalPath();
    void canonicalFilePath();

    void fileName_data();
    void fileName();

    void bundleName_data();
    void bundleName();

    void dir_data();
    void dir();

    void suffix_data();
    void suffix();

    void completeSuffix_data();
    void completeSuffix();

    void baseName_data();
    void baseName();

    void completeBaseName_data();
    void completeBaseName();

    void permission_data();
    void permission();

    void size_data();
    void size();

    void systemFiles();

    void compare_data();
    void compare();

    void consistent_data();
    void consistent();

    void fileTimes_data();
    void fileTimes();
    void fileTimes_oldFile();

    void isSymLink_data();
    void isSymLink();

    void isHidden_data();
    void isHidden();
#if defined(Q_OS_MAC)
    void isHiddenFromFinder();
#endif

    void isBundle_data();
    void isBundle();

    void isLocalFs_data();
    void isLocalFs();

    void refresh();

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    void ntfsJunctionPointsAndSymlinks_data();
    void ntfsJunctionPointsAndSymlinks();
    void brokenShortcut();
#endif

    void isWritable();
    void isExecutable();
    void testDecomposedUnicodeNames_data();
    void testDecomposedUnicodeNames();

    void equalOperator() const;
    void equalOperatorWithDifferentSlashes() const;
    void notEqualOperator() const;

    void detachingOperations();

#if !defined(Q_OS_WINCE) && !defined(Q_OS_SYMBIAN)
    void owner();
#endif
    void group();

    void invalidState();
};

tst_QFileInfo::tst_QFileInfo()
{
}

tst_QFileInfo::~tst_QFileInfo()
{
    QFile::remove("brokenlink.lnk");
    QFile::remove("link.lnk");
    QFile::remove("file1");
    QFile::remove("dummyfile");
    QFile::remove("simplefile.txt");
    QFile::remove("longFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileNamelongFileName.txt");
#ifdef Q_OS_SYMBIAN
    QFile::remove("hidden.txt");
    QFile::remove("nothidden.txt");
#else
    QFile::remove("tempfile.txt");
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    QDir().rmdir("./.hidden-directory");
    QFile::remove("link_to_tst_qfileinfo");
#endif
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QDir().rmdir("./hidden-directory");
    QDir().rmdir("abs_symlink");
    QDir().rmdir("rel_symlink");
    QDir().rmdir("junction_pwd");
    QDir().rmdir("junction_root");
    QDir().rmdir("mountpoint");
    QFile::remove("abs_symlink.cpp");
    QFile::remove("rel_symlink.cpp");
#endif
}

// Testing get/set functions
void tst_QFileInfo::getSetCheck()
{
    QFileInfo obj1;
    // bool QFileInfo::caching()
    // void QFileInfo::setCaching(bool)
    obj1.setCaching(false);
    QCOMPARE(false, obj1.caching());
    obj1.setCaching(true);
    QCOMPARE(true, obj1.caching());
}

static QFileInfoPrivate* getPrivate(QFileInfo &info)
{
    return (*reinterpret_cast<QFileInfoPrivate**>(&info));
}

void tst_QFileInfo::copy()
{
    QTemporaryFile *t;
    t = new QTemporaryFile;
    t->open();
    QFileInfo info(t->fileName());
    QVERIFY(info.exists());

    //copy constructor
    QFileInfo info2(info);
    QFileInfoPrivate *privateInfo = getPrivate(info);
    QFileInfoPrivate *privateInfo2 = getPrivate(info2);
    QCOMPARE(privateInfo, privateInfo2);

    //operator =
    QFileInfo info3 = info;
    QFileInfoPrivate *privateInfo3 = getPrivate(info3);
    QCOMPARE(privateInfo, privateInfo3);
    QCOMPARE(privateInfo2, privateInfo3);

    //refreshing info3 will detach it
    QFile file(info.absoluteFilePath());
    QVERIFY(file.open(QFile::WriteOnly));
    QCOMPARE(file.write("JAJAJAA"), qint64(7));
    file.flush();

    QTest::qWait(250);
#if defined(Q_OS_WIN) || defined(Q_OS_WINCE)
    if (QSysInfo::windowsVersion() & QSysInfo::WV_VISTA ||
                QSysInfo::windowsVersion() & QSysInfo::WV_CE_based)
        file.close();
#endif
#if defined(Q_OS_WINCE)
    // On Windows CE we need to close the file.
    // Otherwise the content will be cached and not
    // flushed to the storage, although we flushed it
    // manually!!! CE has interim cache, we cannot influence.
    QTest::qWait(5000);
#endif
    info3.refresh();
    privateInfo3 = getPrivate(info3);
    QVERIFY(privateInfo != privateInfo3);
    QVERIFY(privateInfo2 != privateInfo3);
    QCOMPARE(privateInfo, privateInfo2);
}

void tst_QFileInfo::isFile_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << false;
    QTest::newRow("data1") << SRCDIR "tst_qfileinfo.cpp" << true;
    QTest::newRow("data2") << ":/tst_qfileinfo/resources/" << false;
    QTest::newRow("data3") << ":/tst_qfileinfo/resources/file1" << true;
    QTest::newRow("data4") << ":/tst_qfileinfo/resources/afilethatshouldnotexist" << false;
}

void tst_QFileInfo::isFile()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.isFile(), expected);
}


void tst_QFileInfo::isDir_data()
{
    // create a broken symlink
    QFile::remove("brokenlink.lnk");
    QFile::remove("dummyfile");
    QFile file3("dummyfile");
    file3.open(QIODevice::WriteOnly);
    if (file3.link("brokenlink.lnk")) {
        file3.remove();
        QFileInfo info3("brokenlink.lnk");
        QVERIFY( info3.isSymLink() );
    }

    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << true;
    QTest::newRow("data1") << SRCDIR "tst_qfileinfo.cpp" << false;
    QTest::newRow("data2") << ":/tst_qfileinfo/resources/" << true;
    QTest::newRow("data3") << ":/tst_qfileinfo/resources/file1" << false;
    QTest::newRow("data4") << ":/tst_qfileinfo/resources/afilethatshouldnotexist" << false;

    QTest::newRow("simple dir") << SRCDIR "resources" << true;
    QTest::newRow("simple dir with slash") << SRCDIR "resources/" << true;

    QTest::newRow("broken link") << "brokenlink.lnk" << false;

#if (defined(Q_OS_WIN) && !defined(Q_OS_WINCE)) || defined(Q_OS_SYMBIAN)
    QTest::newRow("drive 1") << "c:" << true;
    QTest::newRow("drive 2") << "c:/" << true;
    //QTest::newRow("drive 2") << "t:s" << false;
#endif
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("unc 1") << "//"  + QtNetworkSettings::winServerName() << true;
    QTest::newRow("unc 2") << "//"  + QtNetworkSettings::winServerName() + "/" << true;
    QTest::newRow("unc 3") << "//"  + QtNetworkSettings::winServerName() + "/testshare" << true;
    QTest::newRow("unc 4") << "//"  + QtNetworkSettings::winServerName() + "/testshare/" << true;
    QTest::newRow("unc 5") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp" << true;
    QTest::newRow("unc 6") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp/" << true;
    QTest::newRow("unc 7") << "//"  + QtNetworkSettings::winServerName() + "/testshare/adirthatshouldnotexist" << false;
#endif
}

void tst_QFileInfo::isDir()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.isDir(), expected);
}

void tst_QFileInfo::isRoot_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");
    QTest::newRow("data0") << QDir::currentPath() << false;
    QTest::newRow("data1") << "/" << true;
    QTest::newRow("data2") << "*" << false;
    QTest::newRow("data3") << "/*" << false;
    QTest::newRow("data4") << ":/tst_qfileinfo/resources/" << false;
    QTest::newRow("data5") << ":/" << true;

    QTest::newRow("simple dir") << SRCDIR "resources" << false;
    QTest::newRow("simple dir with slash") << SRCDIR "resources/" << false;
#if (defined(Q_OS_WIN) && !defined(Q_OS_WINCE)) || defined(Q_OS_SYMBIAN)
    QTest::newRow("drive 1") << "c:" << false;
    QTest::newRow("drive 2") << "c:/" << true;
    QTest::newRow("drive 3") << "p:/" << false;
#endif

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("unc 1") << "//"  + QtNetworkSettings::winServerName() << true;
    QTest::newRow("unc 2") << "//"  + QtNetworkSettings::winServerName() + "/" << true;
    QTest::newRow("unc 3") << "//"  + QtNetworkSettings::winServerName() + "/testshare" << false;
    QTest::newRow("unc 4") << "//"  + QtNetworkSettings::winServerName() + "/testshare/" << false;
    QTest::newRow("unc 7") << "//ahostthatshouldnotexist" << false;
#endif
}

void tst_QFileInfo::isRoot()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.isRoot(), expected);
}

void tst_QFileInfo::exists_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QDir::currentPath() << true;
    QTest::newRow("data1") << SRCDIR "tst_qfileinfo.cpp" << true;
    QTest::newRow("data2") << "/I/do_not_expect_this_path_to_exist/" << false;
    QTest::newRow("data3") << ":/tst_qfileinfo/resources/" << true;
    QTest::newRow("data4") << ":/tst_qfileinfo/resources/file1" << true;
    QTest::newRow("data5") << ":/I/do_not_expect_this_path_to_exist/" << false;
    QTest::newRow("data6") << SRCDIR "resources/*" << false;
    QTest::newRow("data7") << SRCDIR "resources/*.foo" << false;
    QTest::newRow("data8") << SRCDIR "resources/*.ext1" << false;
    QTest::newRow("data9") << SRCDIR "resources/file?.ext1" << false;
    QTest::newRow("data10") << "." << true;
    QTest::newRow("data11") << ". " << false;
    QTest::newRow("empty") << "" << false;

    QTest::newRow("simple dir") << SRCDIR "resources" << true;
    QTest::newRow("simple dir with slash") << SRCDIR "resources/" << true;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QTest::newRow("unc 1") << "//"  + QtNetworkSettings::winServerName() << true;
    QTest::newRow("unc 2") << "//"  + QtNetworkSettings::winServerName() + "/" << true;
    QTest::newRow("unc 3") << "//"  + QtNetworkSettings::winServerName() + "/testshare" << true;
    QTest::newRow("unc 4") << "//"  + QtNetworkSettings::winServerName() + "/testshare/" << true;
    QTest::newRow("unc 5") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp" << true;
    QTest::newRow("unc 6") << "//"  + QtNetworkSettings::winServerName() + "/testshare/tmp/" << true;
    QTest::newRow("unc 7") << "//"  + QtNetworkSettings::winServerName() + "/testshare/adirthatshouldnotexist" << false;
    QTest::newRow("unc 8") << "//"  + QtNetworkSettings::winServerName() + "/asharethatshouldnotexist" << false;
    QTest::newRow("unc 9") << "//ahostthatshouldnotexist" << false;
#endif
}

void tst_QFileInfo::exists()
{
    QFETCH(QString, path);
    QFETCH(bool, expected);

    QFileInfo fi(path);
    QCOMPARE(fi.exists(), expected);
}

void tst_QFileInfo::absolutePath_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("filename");

    QString drivePrefix;
#if (defined(Q_OS_WIN) && !defined(Q_OS_WINCE)) || defined(Q_OS_SYMBIAN)
    drivePrefix = QDir::currentPath().left(2);
    QString nonCurrentDrivePrefix =
        drivePrefix.left(1).compare("X", Qt::CaseInsensitive) == 0 ? QString("Y:") : QString("X:");

    // Make sure drive-relative paths return correct absolute paths (task 255326)
    QTest::newRow("<current drive>:my.dll") << drivePrefix + "my.dll" << QDir::currentPath() << "my.dll";
    QTest::newRow("<not current drive>:my.dll") << nonCurrentDrivePrefix + "my.dll"
                                                << nonCurrentDrivePrefix + "/"
                                                << "my.dll";
#endif
    QTest::newRow("0") << "/machine/share/dir1/" << drivePrefix + "/machine/share/dir1" << "";
    QTest::newRow("1") << "/machine/share/dir1" << drivePrefix + "/machine/share" << "dir1";
    QTest::newRow("2") << "/usr/local/bin" << drivePrefix + "/usr/local" << "bin";
    QTest::newRow("3") << "/usr/local/bin/" << drivePrefix + "/usr/local/bin" << "";
    QTest::newRow("/test") << "/test" << drivePrefix + "/" << "test";

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    // see task 102898
    QTest::newRow("c:\\autoexec.bat") << "c:\\autoexec.bat" << "C:/"
                                      << "autoexec.bat";
#endif
}

void tst_QFileInfo::absolutePath()
{
    QFETCH(QString, file);
    QFETCH(QString, path);
    QFETCH(QString, filename);

    QFileInfo fi(file);

    QCOMPARE(fi.absolutePath(), path);
    QCOMPARE(fi.fileName(), filename);
}

void tst_QFileInfo::absFilePath_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("relativeFile") << "tmp.txt" << QDir::currentPath() + "/tmp.txt";
    QTest::newRow("relativeFileInSubDir") << "temp/tmp.txt" << QDir::currentPath() + "/" + "temp/tmp.txt";
#if (defined(Q_OS_WIN) && !defined(Q_OS_WINCE)) || defined(Q_OS_SYMBIAN)
    QString curr = QDir::currentPath();

    curr.remove(0, 2);   // Make it a absolute path with no drive specifier: \depot\qt-4.2\tests\auto\qfileinfo
    QTest::newRow(".")            << curr << QDir::currentPath();
    QTest::newRow("absFilePath") << "c:\\home\\andy\\tmp.txt" << "C:/home/andy/tmp.txt";

    // Make sure drive-relative paths return correct absolute paths (task 255326)
    QString drivePrefix = QDir::currentPath().left(2);
    QString nonCurrentDrivePrefix =
        drivePrefix.left(1).compare("X", Qt::CaseInsensitive) == 0 ? QString("Y:") : QString("X:");

    QTest::newRow("<current drive>:my.dll") << drivePrefix + "temp/my.dll" << QDir::currentPath() + "/temp/my.dll";
    QTest::newRow("<not current drive>:my.dll") << nonCurrentDrivePrefix + "temp/my.dll"
                                                << nonCurrentDrivePrefix + "/temp/my.dll";
#else
    QTest::newRow("absFilePath") << "/home/andy/tmp.txt" << "/home/andy/tmp.txt";
#endif
}

void tst_QFileInfo::absFilePath()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
    QVERIFY(QString::compare(fi.absoluteFilePath(), expected, Qt::CaseInsensitive) == 0);
#else
    QCOMPARE(fi.absoluteFilePath(), expected);
#endif
}

void tst_QFileInfo::canonicalPath()
{
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(true);
    tempFile.open();
    QFileInfo fi(tempFile.fileName());
    QCOMPARE(fi.canonicalPath(), QFileInfo(QDir::tempPath()).canonicalFilePath());
}

void tst_QFileInfo::canonicalFilePath()
{
    const QString fileName("tmp.canon");
    QFile tempFile(fileName);
    QVERIFY(tempFile.open(QFile::WriteOnly));
    QFileInfo fi(tempFile.fileName());
    QCOMPARE(fi.canonicalFilePath(), QDir::currentPath() + "/" + fileName);
    tempFile.remove();

    // This used to crash on Mac, verify that it doesn't anymore.
    QFileInfo info("/tmp/../../../../../../../../../../../../../../../../../");
    info.canonicalFilePath();

#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    // This used to crash on Mac
    QFileInfo dontCrash(QLatin1String("/"));
    QCOMPARE(dontCrash.canonicalFilePath(), QLatin1String("/"));
#endif

#ifndef Q_OS_WIN
    // test symlinks
    QFile::remove("link.lnk");
    {
        QFile file(SRCDIR "tst_qfileinfo.cpp");
        if (file.link("link.lnk")) {
            QFileInfo info1(file);
            QFileInfo info2("link.lnk");
            QCOMPARE(info1.canonicalFilePath(), info2.canonicalFilePath());
        }
    }
#  if !defined(Q_OS_SYMBIAN)
    // Symbian doesn't support links to directories
    {
        const QString link(QDir::tempPath() + QDir::separator() + "tst_qfileinfo");
        QFile::remove(link);
        QFile file(QDir::currentPath());
        if (file.link(link)) {
            QFile tempfile("tempfile.txt");
            tempfile.open(QIODevice::ReadWrite);
            tempfile.write("This file is generated by the QFileInfo autotest.");
            QVERIFY(tempfile.flush());
            tempfile.close();

            QFileInfo info1("tempfile.txt");
            QFileInfo info2(link + QDir::separator() + "tempfile.txt");

            QVERIFY(info1.exists());
            QVERIFY(info2.exists());
            QCOMPARE(info1.canonicalFilePath(), info2.canonicalFilePath());

            QFileInfo info3(link + QDir::separator() + "link.lnk");
            QFileInfo info4(SRCDIR "tst_qfileinfo.cpp");
            QVERIFY(!info3.canonicalFilePath().isEmpty());
            QCOMPARE(info4.canonicalFilePath(), info3.canonicalFilePath());

            tempfile.remove();
        }
    }
    {
        QString link(QDir::tempPath() + QDir::separator() + "tst_qfileinfo"
                     + QDir::separator() + "link_to_tst_qfileinfo");
        QFile::remove(link);

        QFile file(QDir::tempPath() + QDir::separator() + "tst_qfileinfo"
                   + QDir::separator() + "tst_qfileinfo.cpp");
        if (file.link(link))
        {
            QFileInfo info1("tst_qfileinfo.cpp");
            QFileInfo info2(link);
            QCOMPARE(info1.canonicalFilePath(), info2.canonicalFilePath());
        }
    }
#  endif
#endif

#ifdef Q_OS_WIN
    typedef BOOL (WINAPI *PtrCreateSymbolicLink)(LPTSTR, LPTSTR, DWORD);
    PtrCreateSymbolicLink ptrCreateSymbolicLink =
            (PtrCreateSymbolicLink)QLibrary::resolve(QLatin1String("kernel32"), "CreateSymbolicLinkW");

    if (!ptrCreateSymbolicLink) {
        QSKIP("Symbolic links aren't supported by FS", SkipAll);
    } else {
        // CreateSymbolicLink can return TRUE & still fail to create the link,
        // the error code in that case is ERROR_PRIVILEGE_NOT_HELD (1314)
        SetLastError(0);
        BOOL ret = ptrCreateSymbolicLink((wchar_t*)QString("res").utf16(), (wchar_t*)QString("resources").utf16(), 1);
        DWORD dwErr = GetLastError();
        if (!ret)
            QSKIP("Symbolic links aren't supported by FS", SkipAll);
        QString currentPath = QDir::currentPath();
        bool is_res_Current = QDir::setCurrent("res");
        if (!is_res_Current && dwErr == 1314)
            QSKIP("Not enough privilages to create Symbolic links", SkipAll);
        QCOMPARE(is_res_Current, true);

        QCOMPARE(QFileInfo("file1").canonicalFilePath(), currentPath + "/resources/file1");

        QCOMPARE(QDir::setCurrent(currentPath), true);
        QDir::current().rmdir("res");
    }
#endif
}

void tst_QFileInfo::fileName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("relativeFile") << "tmp.txt" << "tmp.txt";
    QTest::newRow("relativeFileInSubDir") << "temp/tmp.txt" << "tmp.txt";
#if (defined(Q_OS_WIN) && !defined(Q_OS_WINCE)) || defined(Q_OS_SYMBIAN)
    QTest::newRow("absFilePath") << "c:\\home\\andy\\tmp.txt" << "tmp.txt";
#else
    QTest::newRow("absFilePath") << "/home/andy/tmp.txt" << "tmp.txt";
#endif
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "file1.ext1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "file1.ext1.ext2";

    QTest::newRow("ending slash [small]") << QString::fromLatin1("/a/") << QString::fromLatin1("");
    QTest::newRow("no ending slash [small]") << QString::fromLatin1("/a") << QString::fromLatin1("a");

    QTest::newRow("ending slash") << QString::fromLatin1("/somedir/") << QString::fromLatin1("");
    QTest::newRow("no ending slash") << QString::fromLatin1("/somedir") << QString::fromLatin1("somedir");
}

void tst_QFileInfo::fileName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.fileName(), expected);
}

void tst_QFileInfo::bundleName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("root") << "/" << "";
    QTest::newRow("etc") << "/etc" << "";
#ifdef Q_OS_MAC
    QTest::newRow("safari") << "/Applications/Safari.app" << "Safari";
#endif
}

void tst_QFileInfo::bundleName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.bundleName(), expected);
}

void tst_QFileInfo::dir_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<bool>("absPath");
    QTest::addColumn<QString>("expected");

    QTest::newRow("relativeFile") << "tmp.txt" << false << ".";
    QTest::newRow("relativeFileAbsPath") << "tmp.txt" << true << QDir::currentPath();
    QTest::newRow("relativeFileInSubDir") << "temp/tmp.txt" << false << "temp";
    QTest::newRow("relativeFileInSubDirAbsPath") << "temp/tmp.txt" << true << QDir::currentPath() + "/temp";
    QTest::newRow("absFilePath") << QDir::currentPath() + "/tmp.txt" << false << QDir::currentPath();
    QTest::newRow("absFilePathAbsPath") << QDir::currentPath() + "/tmp.txt" << true << QDir::currentPath();
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << true << ":/tst_qfileinfo/resources";
}

void tst_QFileInfo::dir()
{
    QFETCH(QString, file);
    QFETCH(bool, absPath);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    if (absPath) {
        QCOMPARE(fi.absolutePath(), expected);
        QCOMPARE(fi.absoluteDir().path(), expected);
#ifdef QT3_SUPPORT
        QCOMPARE(fi.dir(true).path(), expected);
#endif
    } else {
        QCOMPARE(fi.path(), expected);
        QCOMPARE(fi.dir().path(), expected);
#ifdef QT3_SUPPORT
        QCOMPARE(fi.dir(false).path(), expected);
#endif
    }
}


void tst_QFileInfo::suffix_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("noextension0") << "file" << "";
    QTest::newRow("noextension1") << "/path/to/file" << "";
    QTest::newRow("data0") << "file.tar" << "tar";
    QTest::newRow("data1") << "file.tar.gz" << "gz";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "gz";
    QTest::newRow("data3") << "/path/file.tar" << "tar";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "ext1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "ext2";
    QTest::newRow("hidden1") << ".ext1" << "ext1";
    QTest::newRow("hidden1") << ".ext" << "ext";
    QTest::newRow("hidden1") << ".ex" << "ex";
    QTest::newRow("hidden1") << ".e" << "e";
    QTest::newRow("hidden2") << ".ext1.ext2" << "ext2";
    QTest::newRow("hidden2") << ".ext.ext2" << "ext2";
    QTest::newRow("hidden2") << ".ex.ext2" << "ext2";
    QTest::newRow("hidden2") << ".e.ext2" << "ext2";
    QTest::newRow("hidden2") << "..ext2" << "ext2";
}

void tst_QFileInfo::suffix()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.suffix(), expected);
}


void tst_QFileInfo::completeSuffix_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("noextension0") << "file" << "";
    QTest::newRow("noextension1") << "/path/to/file" << "";
    QTest::newRow("data0") << "file.tar" << "tar";
    QTest::newRow("data1") << "file.tar.gz" << "tar.gz";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "tar.gz";
    QTest::newRow("data3") << "/path/file.tar" << "tar";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "ext1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "ext1.ext2";
}

void tst_QFileInfo::completeSuffix()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.completeSuffix(), expected);
}

void tst_QFileInfo::baseName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("data0") << "file.tar" << "file";
    QTest::newRow("data1") << "file.tar.gz" << "file";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "file";
    QTest::newRow("data3") << "/path/file.tar" << "file";
    QTest::newRow("data4") << "/path/file" << "file";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "file1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "file1";
}

void tst_QFileInfo::baseName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.baseName(), expected);
}

void tst_QFileInfo::completeBaseName_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

    QTest::newRow("data0") << "file.tar" << "file";
    QTest::newRow("data1") << "file.tar.gz" << "file.tar";
    QTest::newRow("data2") << "/path/file/file.tar.gz" << "file.tar";
    QTest::newRow("data3") << "/path/file.tar" << "file";
    QTest::newRow("data4") << "/path/file" << "file";
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << "file1";
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << "file1.ext1";
}

void tst_QFileInfo::completeBaseName()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.completeBaseName(), expected);
}

void tst_QFileInfo::permission_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<int>("perms");
    QTest::addColumn<bool>("expected");

    QTest::newRow("data0") << QCoreApplication::instance()->applicationFilePath() << int(QFile::ExeUser) << true;
    QTest::newRow("data1") << SRCDIR "tst_qfileinfo.cpp" << int(QFile::ReadUser) << true;
//    QTest::newRow("data2") << "tst_qfileinfo.cpp" << int(QFile::WriteUser) << false;
    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << int(QFile::ReadUser) << true;
    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1" << int(QFile::WriteUser) << false;
    QTest::newRow("resource3") << ":/tst_qfileinfo/resources/file1.ext1" << int(QFile::ExeUser) << false;
}

void tst_QFileInfo::permission()
{
    QFETCH(QString, file);
    QFETCH(int, perms);
    QFETCH(bool, expected);
#ifdef Q_OS_SYMBIAN
    QSKIP("No user based rights in Symbian OS - SOS needs platform security tests instead", SkipAll);
#endif
    QFileInfo fi(file);
    QCOMPARE(fi.permission(QFile::Permissions(perms)), expected);
}

void tst_QFileInfo::size_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<int>("size");

    QTest::newRow("resource1") << ":/tst_qfileinfo/resources/file1.ext1" << 0;
    QFile::remove("file1");
    QFile file("file1");
    QVERIFY(file.open(QFile::WriteOnly));
    QCOMPARE(file.write("JAJAJAA"), qint64(7));
    QTest::newRow("created-file") << "file1" << 7;

    QTest::newRow("resource2") << ":/tst_qfileinfo/resources/file1.ext1.ext2" << 0;
}

void tst_QFileInfo::size()
{
    QFETCH(QString, file);

    QFileInfo fi(file);
    (void)fi.permissions();     // see task 104198
    QTEST(int(fi.size()), "size");
}

void tst_QFileInfo::systemFiles()
{
#if !defined(Q_OS_WIN) || defined(Q_OS_WINCE)
    QSKIP("This is a Windows only test", SkipAll);
#endif
    QFileInfo fi("c:\\pagefile.sys");
    QVERIFY(fi.exists());      // task 167099
    QVERIFY(fi.size() > 0);    // task 189202
    QVERIFY(fi.lastModified().isValid());
}

void tst_QFileInfo::compare_data()
{
    QTest::addColumn<QString>("file1");
    QTest::addColumn<QString>("file2");
    QTest::addColumn<bool>("same");

#if defined(Q_OS_MAC)
    // Since 10.6 we use realpath() in qfsfileengine, and it properly handles
    // file system case sensitivity. However here in the autotest we don't
    // check if the file system is case sensitive, so to make it pass in the
    // default OS X installation we assume we are running on a case insensitive
    // file system if on 10.6 and on a case sensitive file system if on 10.5
    bool caseSensitiveOnMac = true;
    if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_6)
        caseSensitiveOnMac = false;
#endif

    QTest::newRow("data0")
        << QString::fromLatin1(SRCDIR "tst_qfileinfo.cpp")
        << QString::fromLatin1(SRCDIR "tst_qfileinfo.cpp")
        << true;
    QTest::newRow("data1")
        << QString::fromLatin1(SRCDIR "tst_qfileinfo.cpp")
        << QString::fromLatin1("/tst_qfileinfo.cpp")
        << false;
    QTest::newRow("data2")
        << QString::fromLatin1("tst_qfileinfo.cpp")
        << QDir::currentPath() + QString::fromLatin1("/tst_qfileinfo.cpp")
        << true;
    QTest::newRow("casesense1")
        << QString::fromLatin1(SRCDIR "tst_qfileInfo.cpp")
        << QString::fromLatin1(SRCDIR "tst_qfileinfo.cpp")
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
        << true;
#elif defined(Q_OS_MAC)
        << !caseSensitiveOnMac;
#else
        << false;
#endif
}

void tst_QFileInfo::compare()
{
    QFETCH(QString, file1);
    QFETCH(QString, file2);
    QFETCH(bool, same);
    QFileInfo fi1(file1), fi2(file2);
    QCOMPARE(fi1 == fi2, same);
}

void tst_QFileInfo::consistent_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("expected");

#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
    QTest::newRow("slashes") << QString::fromLatin1("\\a\\a\\a\\a") << QString::fromLatin1("/a/a/a/a");
#endif
    QTest::newRow("ending slash") << QString::fromLatin1("/a/somedir/") << QString::fromLatin1("/a/somedir/");
    QTest::newRow("no ending slash") << QString::fromLatin1("/a/somedir") << QString::fromLatin1("/a/somedir");
}

void tst_QFileInfo::consistent()
{
    QFETCH(QString, file);
    QFETCH(QString, expected);

    QFileInfo fi(file);
    QCOMPARE(fi.filePath(), expected);
    QCOMPARE(fi.dir().path() + "/" + fi.fileName(), expected);
}


void tst_QFileInfo::fileTimes_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::newRow("simple") << QString::fromLatin1("simplefile.txt");
    QTest::newRow( "longfile" ) << QString::fromLatin1("longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName.txt");
    QTest::newRow( "longfile absolutepath" ) << QFileInfo(QString::fromLatin1("longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName"
                                                     "longFileNamelongFileNamelongFileNamelongFileName.txt")).absoluteFilePath();
}

void tst_QFileInfo::fileTimes()
{
#if defined(Q_OS_WINCE)
    int sleepTime = 3000;
#else
    int sleepTime = 2000;
#endif
    QFETCH(QString, fileName);
    if (QFile::exists(fileName)) {
        QVERIFY(QFile::remove(fileName));
    }
    QTest::qSleep(sleepTime);
    {
        QFile file(fileName);
#if defined(Q_OS_WINCE)
        QEXPECT_FAIL("longfile", "No long filenames on WinCE", Abort);
        QEXPECT_FAIL("longfile absolutepath", "No long filenames on WinCE", Abort);
#elif defined(Q_OS_SYMBIAN)
        QEXPECT_FAIL("longfile", "Maximum total filepath cannot exceed 256 characters in Symbian", Abort);
        QEXPECT_FAIL("longfile absolutepath", "Maximum total filepath cannot exceed 256 characters in Symbian", Abort);
#endif
        QVERIFY(file.open(QFile::WriteOnly | QFile::Text));
#ifdef Q_OS_UNIX
        if (qIsLikelyToBeNfs(file.handle()))
            QSKIP("This Test doesn't work on NFS", SkipAll);
#endif
        QTextStream ts(&file);
        ts << fileName << endl;
    }
    QTest::qSleep(sleepTime);
    QDateTime beforeWrite = QDateTime::currentDateTime();
    QTest::qSleep(sleepTime);
    {
        QFileInfo fileInfo(fileName);
        QVERIFY(fileInfo.created() < beforeWrite);
        QFile file(fileName);
        QVERIFY(file.open(QFile::ReadWrite | QFile::Text));
        QTextStream ts(&file);
        ts << fileName << endl;
    }
    QTest::qSleep(sleepTime);
    QDateTime beforeRead = QDateTime::currentDateTime();
    QTest::qSleep(sleepTime);
    {
        QFileInfo fileInfo(fileName);
// On unix created() returns the same as lastModified().
#if !defined(Q_OS_UNIX) && !defined(Q_OS_WINCE)
        QVERIFY(fileInfo.created() < beforeWrite);
#endif
        QVERIFY(fileInfo.lastModified() > beforeWrite);
        QFile file(fileName);
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));
        QTextStream ts(&file);
        QString line = ts.readLine();
        QCOMPARE(line, fileName);
    }

    QFileInfo fileInfo(fileName);
#if !defined(Q_OS_UNIX) && !defined(Q_OS_WINCE)
    QVERIFY(fileInfo.created() < beforeWrite);
#endif
    //In Vista the last-access timestamp is not updated when the file is accessed/touched (by default).
    //To enable this the HKLM\SYSTEM\CurrentControlSet\Control\FileSystem\NtfsDisableLastAccessUpdate
    //is set to 0, in the test machine.
#ifdef Q_OS_WINCE
    QEXPECT_FAIL("simple", "WinCE only stores date of access data, not the time", Continue);
#endif
#ifdef Q_OS_SYMBIAN
        QEXPECT_FAIL("simple", "Symbian implementation of stat doesn't return read time right", Abort);
#endif
    QVERIFY(fileInfo.lastRead() > beforeRead);
    QVERIFY(fileInfo.lastModified() > beforeWrite);
    QVERIFY(fileInfo.lastModified() < beforeRead);
}

void tst_QFileInfo::fileTimes_oldFile()
{
    // This is not supported on WinCE
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    // All files are opened in share mode (both read and write).
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    // All files on Windows can be read; there's no such thing as an
    // unreadable file. Add GENERIC_WRITE if WriteOnly is passed.
    int accessRights = GENERIC_READ | GENERIC_WRITE;

    SECURITY_ATTRIBUTES securityAtts = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Regular file mode. In Unbuffered mode, pass the no-buffering flag.
    DWORD flagsAndAtts = FILE_ATTRIBUTE_NORMAL;

    // WriteOnly can create files, ReadOnly cannot.
    DWORD creationDisp = OPEN_ALWAYS;

    // Create the file handle.
    HANDLE fileHandle = CreateFile(L"oldfile.txt",
        accessRights,
        shareMode,
        &securityAtts,
        creationDisp,
        flagsAndAtts,
        NULL);

    // Set file times back to 1601.
    SYSTEMTIME stime;
    stime.wYear = 1601;
    stime.wMonth = 1;
    stime.wDayOfWeek = 1;
    stime.wDay = 1;
    stime.wHour = 1;
    stime.wMinute = 0;
    stime.wSecond = 0;
    stime.wMilliseconds = 0;

    FILETIME ctime;
    QVERIFY(SystemTimeToFileTime(&stime, &ctime));
    FILETIME atime = ctime;
    FILETIME mtime = atime;
    QVERIFY(fileHandle);
    QVERIFY(SetFileTime(fileHandle, &ctime, &atime, &mtime) != 0);

    CloseHandle(fileHandle);

    QFileInfo info("oldfile.txt");
    QCOMPARE(info.lastModified(), QDateTime(QDate(1601, 1, 1), QTime(1, 0), Qt::UTC).toLocalTime());
#endif
}

void tst_QFileInfo::isSymLink_data()
{
#ifndef NO_SYMLINKS
    QFile::remove("link.lnk");
    QFile::remove("brokenlink.lnk");
    QFile::remove("dummyfile");

    QFile file1(SRCDIR "tst_qfileinfo.cpp");
    QVERIFY(file1.link("link.lnk"));

    QFile file2("dummyfile");
    file2.open(QIODevice::WriteOnly);
    QVERIFY(file2.link("brokenlink.lnk"));
    file2.remove();

    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isSymLink");
    QTest::addColumn<QString>("linkTarget");

    QTest::newRow("existent file") << SRCDIR "tst_qfileinfo.cpp" << false << "";
    QTest::newRow("link") << "link.lnk" << true << QFileInfo(SRCDIR "tst_qfileinfo.cpp").absoluteFilePath();
    QTest::newRow("broken link") << "brokenlink.lnk" << true << QFileInfo("dummyfile").absoluteFilePath();
#endif
}

void tst_QFileInfo::isSymLink()
{
#ifndef NO_SYMLINKS
    QFETCH(QString, path);
    QFETCH(bool, isSymLink);
    QFETCH(QString, linkTarget);

    QFileInfo fi(path);
    QCOMPARE(fi.isSymLink(), isSymLink);
    QCOMPARE(fi.symLinkTarget(), linkTarget);
#else
    QSKIP("no symbolic link support on this platform", SkipAll);
#endif
}

void tst_QFileInfo::isHidden_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isHidden");
    foreach (const QFileInfo& info, QDir::drives()) {
        QTest::newRow(qPrintable("drive." + info.path())) << info.path() << false;
    }

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    QVERIFY(QDir("./hidden-directory").exists() || QDir().mkdir("./hidden-directory"));
    QVERIFY(SetFileAttributesW(reinterpret_cast<LPCWSTR>(QString("./hidden-directory").utf16()),FILE_ATTRIBUTE_HIDDEN));
    QTest::newRow("C:/path/to/hidden-directory") << QDir::currentPath() + QString::fromLatin1("/hidden-directory") << true;
    QTest::newRow("C:/path/to/hidden-directory/.") << QDir::currentPath() + QString::fromLatin1("/hidden-directory/.") << true;
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    QVERIFY(QDir("./.hidden-directory").exists() || QDir().mkdir("./.hidden-directory"));
    QTest::newRow("/path/to/.hidden-directory") << QDir::currentPath() + QString("/.hidden-directory") << true;
    QTest::newRow("/path/to/.hidden-directory/.") << QDir::currentPath() + QString("/.hidden-directory/.") << true;
    QTest::newRow("/path/to/.hidden-directory/..") << QDir::currentPath() + QString("/.hidden-directory/..") << true;
#endif

#if defined(Q_OS_MAC)
    // /bin has the hidden attribute on Mac OS X
    QTest::newRow("/bin/") << QString::fromLatin1("/bin/") << true;
#elif !defined(Q_OS_WIN) && !defined(Q_OS_SYMBIAN)
    QTest::newRow("/bin/") << QString::fromLatin1("/bin/") << false;
#endif

#ifdef Q_OS_MAC
    QTest::newRow("mac_etc") << QString::fromLatin1("/etc") << true;
    QTest::newRow("mac_private_etc") << QString::fromLatin1("/private/etc") << false;
    QTest::newRow("mac_Applications") << QString::fromLatin1("/Applications") << false;
#endif

#ifdef Q_OS_SYMBIAN
    // No guaranteed hidden file knows to exist in Symbian filesystem, so make one.
    QString hiddenFileName("hidden.txt");
    QString notHiddenFileName("nothidden.txt");
    QTest::newRow("hidden file") << hiddenFileName << true;
    QTest::newRow("non-hidden file") << notHiddenFileName << false;

    {
        QFile file(hiddenFileName);
        QVERIFY(file.open(QIODevice::WriteOnly));
        QTextStream t(&file);
        t << "foobar";

        QFile file2(notHiddenFileName);
        QVERIFY(file2.open(QIODevice::WriteOnly));
        QTextStream t2(&file2);
        t2 << "foobar";
    }

    RFs rfs;
    TInt err = rfs.Connect();
    QCOMPARE(err, KErrNone);
    HBufC* symFile = qt_QString2HBufC(hiddenFileName);
    err = rfs.SetAtt(*symFile, KEntryAttHidden, 0);
    rfs.Close();
    delete symFile;
    QCOMPARE(err, KErrNone);
#endif
}

void tst_QFileInfo::isHidden()
{
    QFETCH(QString, path);
    QFETCH(bool, isHidden);
    QFileInfo fi(path);

    QCOMPARE(fi.isHidden(), isHidden);
}

#if defined(Q_OS_MAC)
void tst_QFileInfo::isHiddenFromFinder()
{
    const char *filename = "test_foobar.txt";

    QFile testFile(filename);
    testFile.open(QIODevice::WriteOnly | QIODevice::Append);
    testFile.write(QByteArray("world"));
    testFile.close();

    struct stat buf;
    stat(filename, &buf);
    chflags(filename, buf.st_flags | UF_HIDDEN);

    QFileInfo fi(filename);
    QCOMPARE(fi.isHidden(), true);

    testFile.remove();
}
#endif

void tst_QFileInfo::isBundle_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isBundle");
    QTest::newRow("root") << QString::fromLatin1("/") << false;
#ifdef Q_OS_MAC
    QTest::newRow("mac_Applications") << QString::fromLatin1("/Applications") << false;
    QTest::newRow("mac_Applications") << QString::fromLatin1("/Applications/Safari.app") << true;
#endif
}

void tst_QFileInfo::isBundle()
{
    QFETCH(QString, path);
    QFETCH(bool, isBundle);
    QFileInfo fi(path);
    QCOMPARE(fi.isBundle(), isBundle);
}

void tst_QFileInfo::isLocalFs_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isLocalFs");

    QTest::newRow("local root") << QString::fromLatin1("/") << true;
    QTest::newRow("local non-existent file") << QString::fromLatin1("/abrakadabra.boo") << true;

    QTest::newRow("qresource root") << QString::fromLatin1(":/") << false;
}

void tst_QFileInfo::isLocalFs()
{
    QFETCH(QString, path);
    QFETCH(bool, isLocalFs);

    QFileInfo info(path);
    QFileInfoPrivate *privateInfo = getPrivate(info);
    QCOMPARE((privateInfo->fileEngine == 0), isLocalFs);
    if (privateInfo->fileEngine)
       QCOMPARE(bool(privateInfo->fileEngine->fileFlags(QAbstractFileEngine::LocalDiskFlag)
                     & QAbstractFileEngine::LocalDiskFlag), isLocalFs);
}

void tst_QFileInfo::refresh()
{
#if defined(Q_OS_WINCE) || defined(Q_OS_WIN)
    int sleepTime = 3000;
#else
    int sleepTime = 2000;
#endif

    QFile::remove("file1");
    QFile file("file1");
    QVERIFY(file.open(QFile::WriteOnly));
    QCOMPARE(file.write("JAJAJAA"), qint64(7));
    file.flush();

    QFileInfo info(file);
    QDateTime lastModified = info.lastModified();
    QCOMPARE(info.size(), qint64(7));

    QTest::qSleep(sleepTime);

    QCOMPARE(file.write("JOJOJO"), qint64(6));
    file.flush();
    QVERIFY(info.lastModified() == lastModified);

    QCOMPARE(info.size(), qint64(7));
#if defined(Q_OS_WIN) || defined(Q_OS_WINCE)
    if (QSysInfo::windowsVersion() & QSysInfo::WV_VISTA ||
                QSysInfo::windowsVersion() & QSysInfo::WV_CE_based)
        file.close();
#endif
#if defined(Q_OS_WINCE)
    // On Windows CE we need to close the file.
    // Otherwise the content will be cached and not
    // flushed to the storage, although we flushed it
    // manually!!! CE has interim cache, we cannot influence.
    QTest::qWait(5000);
#endif
    info.refresh();
    QCOMPARE(info.size(), qint64(13));
    QVERIFY(info.lastModified() > lastModified);

    QFileInfo info2 = info;
    QCOMPARE(info2.size(), info.size());

    info2.refresh();
    QCOMPARE(info2.size(), info.size());
}

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
void tst_QFileInfo::ntfsJunctionPointsAndSymlinks_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("isSymLink");
    QTest::addColumn<QString>("linkTarget");
    QTest::addColumn<QString>("canonicalFilePath");

    QDir pwd;
    pwd.mkdir("target");

    QLibrary kernel32("kernel32");
    typedef BOOLEAN (WINAPI *PtrCreateSymbolicLink)(LPCWSTR, LPCWSTR, DWORD);
    PtrCreateSymbolicLink createSymbolicLinkW = 0;
    createSymbolicLinkW = (PtrCreateSymbolicLink) kernel32.resolve("CreateSymbolicLinkW");
    if (!createSymbolicLinkW) {
        //we need at least one data set for the test not to fail when skipping _data function
        QDir target("target");
        QTest::newRow("dummy") << target.path() << false << "" << target.canonicalPath();
        QSKIP("symbolic links not supported by operating system",SkipSingle);
    }
    {
        //Directory symlinks
        QDir target("target");
        QVERIFY(target.exists());

        QString absTarget = QDir::toNativeSeparators(target.absolutePath());
        QString absSymlink = QDir::toNativeSeparators(pwd.absolutePath()).append("\\abs_symlink");
        QString relTarget = "target";
        QString relSymlink = "rel_symlink";
        QString fileInTarget(absTarget);
        fileInTarget.append("\\file");
        QString fileInSymlink(absSymlink);
        fileInSymlink.append("\\file");
        QFile file(fileInTarget);
        file.open(QIODevice::ReadWrite);
        file.close();

        DWORD err = ERROR_SUCCESS ;
        if (!pwd.exists("abs_symlink"))
            if (!createSymbolicLinkW((wchar_t*)absSymlink.utf16(),(wchar_t*)absTarget.utf16(),0x1))
                err = GetLastError();
        if (err == ERROR_SUCCESS && !pwd.exists(relSymlink))
            if (!createSymbolicLinkW((wchar_t*)relSymlink.utf16(),(wchar_t*)relTarget.utf16(),0x1))
                err = GetLastError();
        if (err != ERROR_SUCCESS) {
            wchar_t errstr[0x100];
            DWORD count = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
                0, err, 0, errstr, 0x100, 0);
            QString error(QString::fromUtf16(errstr, count));
            qWarning() << error;
            //we need at least one data set for the test not to assert fail when skipping _data function
            QDir target("target");
            QTest::newRow("dummy") << target.path() << false << "" << target.canonicalPath();
            QSKIP("link not supported by FS or insufficient privilege", SkipSingle);
        }
        QVERIFY(file.exists());

        QTest::newRow("absolute dir symlink") << absSymlink << true << QDir::fromNativeSeparators(absTarget) << target.canonicalPath();
        QTest::newRow("relative dir symlink") << relSymlink << true << QDir::fromNativeSeparators(relTarget) << target.canonicalPath();
        QTest::newRow("file in symlink dir") << fileInSymlink << false << "" << target.canonicalPath().append("/file");
    }
    {
        //File symlinks
        QFileInfo target(SRCDIR "tst_qfileinfo.cpp");
        QString absTarget = QDir::toNativeSeparators(target.absoluteFilePath());
        QString absSymlink = QDir::toNativeSeparators(pwd.absolutePath()).append("\\abs_symlink.cpp");
        QString relTarget = QDir::toNativeSeparators(pwd.relativeFilePath(target.absoluteFilePath()));
        QString relSymlink = "rel_symlink.cpp";
        QVERIFY(pwd.exists("abs_symlink.cpp") || createSymbolicLinkW((wchar_t*)absSymlink.utf16(),(wchar_t*)absTarget.utf16(),0x0));
        QVERIFY(pwd.exists(relSymlink) || createSymbolicLinkW((wchar_t*)relSymlink.utf16(),(wchar_t*)relTarget.utf16(),0x0));

        QTest::newRow("absolute file symlink") << absSymlink << true << QDir::fromNativeSeparators(absTarget) << target.canonicalFilePath();
        QTest::newRow("relative file symlink") << relSymlink << true << QDir::fromNativeSeparators(relTarget) << target.canonicalFilePath();
    }

    //Junctions
    QString target = "target";
    QString junction = "junction_pwd";
    FileSystem::createNtfsJunction(target, junction);
    QFileInfo targetInfo(target);
    QTest::newRow("junction_pwd") << junction << true << targetInfo.absoluteFilePath() << targetInfo.canonicalFilePath();

    QFileInfo fileInJunction(targetInfo.absoluteFilePath().append("/file"));
    QFile file(fileInJunction.absoluteFilePath());
    file.open(QIODevice::ReadWrite);
    file.close();
    QVERIFY(file.exists());
    QTest::newRow("file in junction") << fileInJunction.absoluteFilePath() << false << "" << fileInJunction.canonicalFilePath();

    target = QDir::rootPath();
    junction = "junction_root";
    FileSystem::createNtfsJunction(target, junction);
    targetInfo.setFile(target);
    QTest::newRow("junction_root") << junction << true << targetInfo.absoluteFilePath() << targetInfo.canonicalFilePath();

    //Mountpoint
    typedef BOOLEAN (WINAPI *PtrGetVolumeNameForVolumeMountPointW)(LPCWSTR, LPWSTR, DWORD);
    PtrGetVolumeNameForVolumeMountPointW getVolumeNameForVolumeMountPointW = 0;
    getVolumeNameForVolumeMountPointW = (PtrGetVolumeNameForVolumeMountPointW) kernel32.resolve("GetVolumeNameForVolumeMountPointW");
    if(getVolumeNameForVolumeMountPointW)
    {
        wchar_t buffer[MAX_PATH];
        QString rootPath = QDir::toNativeSeparators(QDir::rootPath());
        QVERIFY(getVolumeNameForVolumeMountPointW((wchar_t*)rootPath.utf16(), buffer, MAX_PATH));
        QString rootVolume = QString::fromWCharArray(buffer);
        junction = "mountpoint";
        rootVolume.replace("\\\\?\\","\\??\\");
        FileSystem::createNtfsJunction(rootVolume, junction);
        QTest::newRow("mountpoint") << junction << true <<  QDir::fromNativeSeparators(rootPath) << QDir::rootPath();
    }
}

void tst_QFileInfo::ntfsJunctionPointsAndSymlinks()
{
    QFETCH(QString, path);
    QFETCH(bool, isSymLink);
    QFETCH(QString, linkTarget);
    QFETCH(QString, canonicalFilePath);

    QFileInfo fi(path);
    QCOMPARE(fi.isSymLink(), isSymLink);
    QCOMPARE(fi.symLinkTarget(), linkTarget);
    QCOMPARE(fi.canonicalFilePath(), canonicalFilePath);
}

void tst_QFileInfo::brokenShortcut()
{
    QString linkName("borkenlink.lnk");
    QFile::remove(linkName);
    QFile file(linkName);
    file.open(QFile::WriteOnly);
    file.write("b0rk");
    file.close();

    QFileInfo info(linkName);
    QVERIFY(info.isSymLink());
    QVERIFY(!info.exists());
    QFile::remove(linkName);
}
#endif

void tst_QFileInfo::isWritable()
{
    QFile tempfile("tempfile.txt");
    tempfile.open(QIODevice::WriteOnly);
    tempfile.write("This file is generated by the QFileInfo autotest.");
    tempfile.close();

    QVERIFY(QFileInfo("tempfile.txt").isWritable());
    tempfile.remove();

#ifdef Q_OS_WIN
#ifdef Q_OS_WINCE
    QFileInfo fi("\\Windows\\wince.nls");
#else
    QFileInfo fi("c:\\pagefile.sys");
#endif
    QVERIFY(fi.exists());
    QVERIFY(!fi.isWritable());
#endif
#if defined (Q_OS_UNIX) && !defined (Q_OS_SYMBIAN)
    if (::getuid() == 0)
        QVERIFY(QFileInfo("/etc/passwd").isWritable());
    else
        QVERIFY(!QFileInfo("/etc/passwd").isWritable());
#endif
}

void tst_QFileInfo::isExecutable()
{
#ifdef Q_OS_SYMBIAN
    QString appPath = "c:/sys/bin/tst_qfileinfo.exe";
#else
    QString appPath = QCoreApplication::applicationDirPath();
    appPath += "/tst_qfileinfo";
# if defined(Q_OS_WIN)
    appPath += ".exe";
# endif
#endif
    QFileInfo fi(appPath);
    QCOMPARE(fi.isExecutable(), true);

    QCOMPARE(QFileInfo("qfileinfo.pro").isExecutable(), false);
}


void tst_QFileInfo::testDecomposedUnicodeNames_data()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("exists");
    QString currPath = QDir::currentPath();
    QTest::newRow("latin-only") << currPath + "/4.pdf" << "4.pdf" << true;
    QTest::newRow("one-decomposed uni") << currPath + QString::fromUtf8("/4 ä.pdf") << QString::fromUtf8("4 ä.pdf") << true;
    QTest::newRow("many-decomposed uni") << currPath + QString::fromUtf8("/4 äääcopy.pdf") << QString::fromUtf8("4 äääcopy.pdf") << true;
    QTest::newRow("no decomposed") << currPath + QString::fromUtf8("/4 øøøcopy.pdf") << QString::fromUtf8("4 øøøcopy.pdf") << true;
}

static void createFileNative(const QString &filePath)
{
#ifdef Q_OS_UNIX
    int fd = open(filePath.normalized(QString::NormalizationForm_D).toUtf8().constData(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        QFAIL("couldn't create file");
    } else {
        close(fd);
    }
#else
    Q_UNUSED(filePath);
#endif
}

static void removeFileNative(const QString &filePath)
{
#ifdef Q_OS_UNIX
    unlink(filePath.normalized(QString::NormalizationForm_D).toUtf8().constData());
#else
    Q_UNUSED(filePath);
#endif
}

void tst_QFileInfo::testDecomposedUnicodeNames()
{
#ifndef Q_OS_MAC
    QSKIP("This is a OS X only test (unless you know more about filesystems, then maybe you should try it ;)", SkipAll);
#endif
    QFETCH(QString, filePath);
    createFileNative(filePath);

    QFileInfo file(filePath);
    QTEST(file.fileName(), "fileName");
    QTEST(file.exists(), "exists");
    removeFileNative(filePath);
}

void tst_QFileInfo::equalOperator() const
{
    /* Compare two default constructed values. Yes, to me it seems it should be the opposite too, but
     * this is how the code was written. */
    QVERIFY(!(QFileInfo() == QFileInfo()));
}


void tst_QFileInfo::equalOperatorWithDifferentSlashes() const
{
    const QFileInfo fi1("/usr");
    const QFileInfo fi2("/usr/");

    QCOMPARE(fi1, fi2);
}

void tst_QFileInfo::notEqualOperator() const
{
    /* Compare two default constructed values. Yes, to me it seems it should be the opposite too, but
     * this is how the code was written. */
    QVERIFY(QFileInfo() != QFileInfo());
}

void tst_QFileInfo::detachingOperations()
{
    QFileInfo info1;
    QVERIFY(info1.caching());
    info1.setCaching(false);

    {
        QFileInfo info2 = info1;

        QVERIFY(!info1.caching());
        QVERIFY(!info2.caching());

        info2.setCaching(true);
        QVERIFY(info2.caching());

        info1.setFile("foo");
        QVERIFY(!info1.caching());
    }

    {
        QFile file("foo");
        info1.setFile(file);
        QVERIFY(!info1.caching());
    }

    info1.setFile(QDir(), "foo");
    QVERIFY(!info1.caching());

    {
        QFileInfo info3;
        QVERIFY(info3.caching());

        info3 = info1;
        QVERIFY(!info3.caching());
    }

    info1.refresh();
    QVERIFY(!info1.caching());

    QVERIFY(info1.makeAbsolute());
    QVERIFY(!info1.caching());

    info1.detach();
    QVERIFY(!info1.caching());
}

#if !defined(Q_OS_WINCE) && !defined(Q_OS_SYMBIAN)
#if defined (Q_OS_WIN)
BOOL IsUserAdmin()
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    b = AllocateAndInitializeSid(
                &NtAuthority,
                2,
                SECURITY_BUILTIN_DOMAIN_RID,
                DOMAIN_ALIAS_RID_ADMINS,
                0, 0, 0, 0, 0, 0,
                &AdministratorsGroup);
    if (b) {
        if (!CheckTokenMembership( NULL, AdministratorsGroup, &b))
            b = FALSE;
        FreeSid(AdministratorsGroup);
    }

    return(b);
}
#endif

void tst_QFileInfo::owner()
{
    QString userName;
#if defined(Q_OS_UNIX)
    {
        passwd *user = getpwuid(geteuid());
        QVERIFY(user);
        char *usernameBuf = user->pw_name;
        userName = QString::fromLocal8Bit(usernameBuf);
    }
#endif
#if defined(Q_OS_WIN)
    wchar_t  usernameBuf[1024];
    DWORD  bufSize = 1024;
    if (GetUserNameW(usernameBuf, &bufSize)) {
        userName = QString::fromWCharArray(usernameBuf);
        if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && IsUserAdmin()) {
            // Special case : If the user is a member of Administrators group, all files
            // created by the current user are owned by the Administrators group.
            LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
            DWORD dwLevel = 0;
            DWORD dwFlags = LG_INCLUDE_INDIRECT ;
            DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
            DWORD dwEntriesRead = 0;
            DWORD dwTotalEntries = 0;
            NET_API_STATUS nStatus;
            nStatus = NetUserGetLocalGroups(0, usernameBuf, dwLevel, dwFlags, (LPBYTE *) &pBuf,
                                            dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries);
            // Check if the current user is a member of Administrators group
            if (nStatus == NERR_Success && pBuf){
                for (int i = 0; i < dwEntriesRead; i++) {
                    QString groupName = QString::fromWCharArray(pBuf[i].lgrui0_name);
                    if (!groupName.compare(QLatin1String("Administrators")))
                        userName = groupName;
                }
            }
            if (pBuf != NULL)
                NetApiBufferFree(pBuf);
        }
    }
    extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
    qt_ntfs_permission_lookup = 1;
#endif
    if (userName.isEmpty())
        QSKIP("Can't retrieve the user name", SkipAll);
    QString fileName("ownertest.txt");
    QVERIFY(!QFile::exists(fileName) || QFile::remove(fileName));
    {
        QFile testFile(fileName);
        QVERIFY(testFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QByteArray testData("testfile");
        QVERIFY(testFile.write(testData) != -1);
    }
    QFileInfo fi(fileName);
    QVERIFY(fi.exists());
    QCOMPARE(fi.owner(), userName);

    QFile::remove(fileName);
#if defined(Q_OS_WIN)
    qt_ntfs_permission_lookup = 0;
#endif
}
#endif

void tst_QFileInfo::group()
{
    QString expected;
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    struct group *gr;
    gid_t gid = getegid();
    gr = getgrgid(gid);
    expected = QString::fromLocal8Bit(gr->gr_name);
#endif

    QString fileName("ownertest.txt");
    if (QFile::exists(fileName))
        QFile::remove(fileName);
    QFile testFile(fileName);
    QVERIFY(testFile.open(QIODevice::WriteOnly | QIODevice::Text));
    QByteArray testData("testfile");
    QVERIFY(testFile.write(testData) != -1);
    testFile.close();
    QFileInfo fi(fileName);
    QVERIFY(fi.exists());

    QCOMPARE(fi.group(), expected);
}

void tst_QFileInfo::invalidState()
{
    // Shouldn't crash;

    {
        QFileInfo info;
        QCOMPARE(info.size(), qint64(0));
        QVERIFY(!info.exists());

        info.setCaching(false);

        info.created();
        info.lastRead();
        info.lastModified();
    }

    {
        QFileInfo info("");
        QCOMPARE(info.size(), qint64(0));
        QVERIFY(!info.exists());

        info.setCaching(false);

        info.created();
        info.lastRead();
        info.lastModified();
    }

    {
        QFileInfo info("file-doesn't-really-exist.txt");
        QCOMPARE(info.size(), qint64(0));
        QVERIFY(!info.exists());

        info.setCaching(false);

        info.created();
        info.lastRead();
        info.lastModified();
    }

    QVERIFY(true);
}

QTEST_MAIN(tst_QFileInfo)
#include "tst_qfileinfo.moc"
