/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qcoreapplication.h>
#include <qstring.h>
#include <qtemporaryfile.h>
#include <qfile.h>
#include <qdir.h>
#include <qset.h>

#if defined(Q_OS_WIN)
# include <windows.h>
#endif
#if defined(Q_OS_UNIX)
# include <sys/types.h>
# include <sys/stat.h>
# include <errno.h>
# include <fcntl.h>             // open(2)
# include <unistd.h>            // close(2)
#endif

#ifdef Q_OS_SYMBIAN
#define SRCDIR ""
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_QTemporaryFile : public QObject
{
    Q_OBJECT
public:
    tst_QTemporaryFile();
    virtual ~tst_QTemporaryFile();
public slots:
    void init();
    void cleanup();
private slots:
    void construction();
    void fileTemplate();
    void fileTemplate_data();
    void getSetCheck();
    void fileName();
    void fileNameIsEmpty();
    void autoRemove();
    void write();
    void openCloseOpenClose();
    void size();
    void resize();
    void openOnRootDrives();
    void stressTest();
    void rename();
    void renameFdLeak();
    void reOpenThroughQFile();
    void keepOpenMode();
    void resetTemplateAfterError();
    void setTemplateAfterOpen();
    void autoRemoveAfterFailedRename();

public:
};

void tst_QTemporaryFile::construction()
{
    QTemporaryFile file(0);
    QString tmp = QDir::tempPath();
    QCOMPARE(file.fileTemplate().left(tmp.size()), tmp);
    QCOMPARE(file.fileTemplate().at(tmp.size()), QChar('/'));
}

// Testing get/set functions
void tst_QTemporaryFile::getSetCheck()
{
    QTemporaryFile obj1;
    // bool QTemporaryFile::autoRemove()
    // void QTemporaryFile::setAutoRemove(bool)
    obj1.setAutoRemove(false);
    QCOMPARE(false, obj1.autoRemove());
    obj1.setAutoRemove(true);
    QCOMPARE(true, obj1.autoRemove());
}

tst_QTemporaryFile::tst_QTemporaryFile()
{
}

tst_QTemporaryFile::~tst_QTemporaryFile()
{

}

void tst_QTemporaryFile::init()
{
// TODO: Add initialization code here.
// This will be executed immediately before each test is run.
}

void tst_QTemporaryFile::cleanup()
{
// TODO: Add cleanup code here.
// This will be executed immediately after each test is run.
}

void tst_QTemporaryFile::fileTemplate_data()
{
    QTest::addColumn<QString>("constructorTemplate");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("fileTemplate");

    QTest::newRow("constructor default")          << "" << "" << "";
    QTest::newRow("constructor with xxx sufix") << "qt_XXXXXXxxx" << "xxx" << "";
    QTest::newRow("constructor with xXx sufix") << "qt_XXXXXXxXx" << "xXx" << "";
    QTest::newRow("constructor with no sufix") << "qt_XXXXXX" << "" << "";
    QTest::newRow("constructor with >6 X's and xxx suffix") << "qt_XXXXXXXXXXxxx" << "xxx" << "";
    QTest::newRow("constructor with >6 X's, no suffix") << "qt_XXXXXXXXXX" << "" << "";

    QTest::newRow("set template, no suffix") << "" << "" << "foo";
    QTest::newRow("set template, with lowercase XXXXXX") << "" << "xxxxxx" << "qt_XXXXXXxxxxxx";
    QTest::newRow("set template, with xxx") << "" << ".xxx" << "qt_XXXXXX.xxx";
    QTest::newRow("set template, with >6 X's") << "" << ".xxx" << "qt_XXXXXXXXXXXXXX.xxx";
    QTest::newRow("set template, with >6 X's, no suffix") << "" << "" << "qt_XXXXXXXXXXXXXX";
}

void tst_QTemporaryFile::fileTemplate()
{
    QFETCH(QString, constructorTemplate);
    QFETCH(QString, suffix);
    QFETCH(QString, fileTemplate);

    QTemporaryFile file(constructorTemplate);
    if (!fileTemplate.isEmpty())
        file.setFileTemplate(fileTemplate);

    QCOMPARE(file.open(), true);

    QCOMPARE(file.fileName().right(suffix.length()), suffix);
    file.close();
}


/*
    This tests whether the temporary file really gets placed in QDir::tempPath
*/
void tst_QTemporaryFile::fileName()
{
    // Get QDir::tempPath and make an absolute path.
    QString tempPath = QDir::tempPath();
    QString absoluteTempPath = QDir(tempPath).absolutePath();
    QTemporaryFile file;
    file.setAutoRemove(true);
    file.open();
    QString fileName = file.fileName();
    QVERIFY(QFile::exists(fileName));
    // Get path to the temp file, whithout the file name.
    QString absoluteFilePath = QFileInfo(fileName).absolutePath();
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
    absoluteFilePath = absoluteFilePath.toLower();
    absoluteTempPath = absoluteTempPath.toLower();
#endif
    QCOMPARE(absoluteFilePath, absoluteTempPath);
}

void tst_QTemporaryFile::fileNameIsEmpty()
{
    QString filename;
    {
        QTemporaryFile file;
        QVERIFY(file.fileName().isEmpty());

        QVERIFY(file.open());
        QVERIFY(!file.fileName().isEmpty());

        filename = file.fileName();
        QVERIFY(QFile::exists(filename));

        file.close();
        QVERIFY(!file.isOpen());
        QVERIFY(QFile::exists(filename));
        QVERIFY(!file.fileName().isEmpty());
    }
    QVERIFY(!QFile::exists(filename));
}

void tst_QTemporaryFile::autoRemove()
{
	// Test auto remove
	QString fileName;
	{
		QTemporaryFile file("tempXXXXXX");
		file.setAutoRemove(true);
		QVERIFY(file.open());
		fileName = file.fileName();
		file.close();
	}
	QVERIFY(!QFile::exists(fileName));

	// Test if disabling auto remove works.
	{
		QTemporaryFile file("tempXXXXXX");
		file.setAutoRemove(false);
		QVERIFY(file.open());
		fileName = file.fileName();
		file.close();
	}
	QVERIFY(QFile::exists(fileName));
	QVERIFY(QFile::remove(fileName));


	// Do not explicitly call setAutoRemove (tests if it really is the default as documented)
	{
		QTemporaryFile file("tempXXXXXX");
		QVERIFY(file.open());
		fileName = file.fileName();
		file.close();
	}
	QVERIFY(!QFile::exists(fileName));

}

void tst_QTemporaryFile::write()
{
    QByteArray data("OLE\nOLE\nOLE");
    QTemporaryFile file;
    QVERIFY(file.open());
    QCOMPARE((int)file.write(data), data.size());
    file.reset();
    QFile compare(file.fileName());
    compare.open(QIODevice::ReadOnly);
    QCOMPARE(compare.readAll() , data);
    file.close();
}

void tst_QTemporaryFile::openCloseOpenClose()
{
    QString fileName;
    {
        // Create a temp file
        QTemporaryFile file("tempXXXXXX");
        file.setAutoRemove(true);
        QVERIFY(file.open());
        file.write("OLE");
        fileName = file.fileName();
        QVERIFY(QFile::exists(fileName));
        file.close();

        // Check that it still exists after being closed
        QVERIFY(QFile::exists(fileName));
        QVERIFY(!file.isOpen());
        QVERIFY(file.open());
        QCOMPARE(file.readAll(), QByteArray("OLE"));
        // Check that it's still the same file after being opened again.
        QCOMPARE(file.fileName(), fileName);
    }
    QVERIFY(!QFile::exists(fileName));
}

void tst_QTemporaryFile::size()
{
    QTemporaryFile file;
    QVERIFY(file.open());
    QVERIFY(file.exists());
    QVERIFY(!file.isSequential());
    QByteArray str("foobar");
    file.write(str);
    QVERIFY(QFile::exists(file.fileName()));
    // On CE it takes more time for the filesystem to update
    // the information. Usually you have to close it or seek
    // to get latest information. flush() does not help either.
#if !defined(Q_OS_WINCE)
    QCOMPARE(file.size(), qint64(6));
#endif
    file.seek(0);
    QCOMPARE(file.size(), qint64(6));
}

void tst_QTemporaryFile::resize()
{
    QTemporaryFile file;
    file.setAutoRemove(true);
    QVERIFY(file.open());
    QVERIFY(file.resize(100));

    QCOMPARE(QFileInfo(file.fileName()).size(), qint64(100));

    file.close();
}

void tst_QTemporaryFile::openOnRootDrives()
{
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    unsigned int lastErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
    // If it's possible to create a file in the root directory, it
    // must be possible to create a temp file there too.
    foreach (QFileInfo driveInfo, QDir::drives()) {
        QFile testFile(driveInfo.filePath() + "XXXXXX.txt");
        if (testFile.open(QIODevice::ReadWrite)) {
            testFile.remove();
            QTemporaryFile file(driveInfo.filePath() + "XXXXXX.txt");
            file.setAutoRemove(true);
            QVERIFY(file.open());
        }
    }
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    SetErrorMode(lastErrorMode);
#endif
}

void tst_QTemporaryFile::stressTest()
{
#if defined(Q_OS_WINCE) || defined(Q_OS_SYMBIAN)
    // 200 is still ok, first colision happens after ~30
    const int iterations = 200;
#else
    const int iterations = 1000;
#endif

    QSet<QString> names;
    for (int i = 0; i < iterations; ++i) {
        QTemporaryFile file;
        file.setAutoRemove(false);
        if (!file.open()) {
            qDebug() << "Could not open File:" << file.fileName();
            continue;
        }
        QVERIFY(!names.contains(file.fileName()));
        names.insert(file.fileName());
    }
    for (QSet<QString>::const_iterator it = names.constBegin(); it != names.constEnd(); ++it) {
        QFile::remove(*it);
    }
}

void tst_QTemporaryFile::rename()
{
    // This test checks that the temporary file is deleted, even after a
    // rename.

    QDir dir;
    QVERIFY(!dir.exists("temporary-file.txt"));

    QString tempname;
    {
        QTemporaryFile file(dir.filePath("temporary-file.XXXXXX"));

        QVERIFY(file.open());
        tempname = file.fileName();
        QVERIFY(dir.exists(tempname));

        QVERIFY(file.rename("temporary-file.txt"));
        QVERIFY(!dir.exists(tempname));
        QVERIFY(dir.exists("temporary-file.txt"));
        QCOMPARE(file.fileName(), QString("temporary-file.txt"));
    }

    QVERIFY(!dir.exists(tempname));
    QVERIFY(!dir.exists("temporary-file.txt"));
}

void tst_QTemporaryFile::renameFdLeak()
{
#ifdef Q_OS_UNIX
    // Test this on Unix only

    // Open a bunch of files to force the fd count to go up
    static const int count = 10;
    int bunch_of_files[count];
    for (int i = 0; i < count; ++i) {
        bunch_of_files[i] = ::open(SRCDIR "tst_qtemporaryfile.cpp", O_RDONLY);
        QVERIFY(bunch_of_files[i] != -1);
    }

    int fd;
    {
        QTemporaryFile file;
        file.setAutoRemove(false);
        QVERIFY(file.open());

        // close the bunch of files
        for (int i = 0; i < count; ++i)
            ::close(bunch_of_files[i]);

        // save the file descriptor for later
        fd = file.handle();

        // rename the file to something
        QString newPath = QDir::tempPath() + "/tst_qtemporaryfile-renameFdLeak-" + QString::number(getpid());
        file.rename(newPath);
        QFile::remove(newPath);
    }

    // check if QTemporaryFile closed the file
    QVERIFY(::close(fd) == -1 && errno == EBADF);
#endif
}

void tst_QTemporaryFile::reOpenThroughQFile()
{
    QByteArray data("abcdefghij");

    QTemporaryFile file;
    QVERIFY(((QFile &)file).open(QIODevice::WriteOnly));
    QCOMPARE(file.write(data), (qint64)data.size());

    file.close();
    QVERIFY(file.open());
    QCOMPARE(file.readAll(), data);
}

void tst_QTemporaryFile::keepOpenMode()
{
    QByteArray data("abcdefghij");

    {
        QTemporaryFile file;
        QVERIFY(((QFile &)file).open(QIODevice::WriteOnly));
        QVERIFY(QIODevice::WriteOnly == file.openMode());

        QCOMPARE(file.write(data), (qint64)data.size());
        file.close();

        QVERIFY(((QFile &)file).open(QIODevice::ReadOnly));
        QVERIFY(QIODevice::ReadOnly == file.openMode());
        QCOMPARE(file.readAll(), data);
    }

    {
        QTemporaryFile file;
        QVERIFY(file.open());
        QCOMPARE(file.write(data), (qint64)data.size());
        QVERIFY(file.rename("temporary-file.txt"));

        QVERIFY(((QFile &)file).open(QIODevice::ReadOnly));
        QVERIFY(QIODevice::ReadOnly == file.openMode());
        QCOMPARE(file.readAll(), data);

        QVERIFY(((QFile &)file).open(QIODevice::WriteOnly));
        QVERIFY(QIODevice::WriteOnly == file.openMode());
    }
}

void tst_QTemporaryFile::resetTemplateAfterError()
{
    // calling setFileTemplate on a failed open

    QString tempPath = QDir::tempPath();

    QString const fileTemplate("destination/qt_temp_file_test.XXXXXX");
    QString const fileTemplate2(tempPath + "/qt_temp_file_test.XXXXXX");

    QVERIFY2( QDir(tempPath).exists() || QDir().mkpath(tempPath), "Test precondition" );
    QVERIFY2( !QFile::exists("destination"), "Test precondition" );
    QVERIFY2( !QFile::exists(fileTemplate2) || QFile::remove(fileTemplate2), "Test precondition" );

    QFile file(fileTemplate2);
    QByteArray fileContent("This file is intentionally NOT left empty.");

    QVERIFY( file.open(QIODevice::ReadWrite | QIODevice::Truncate) );
    QCOMPARE( file.write(fileContent), (qint64)fileContent.size() );
    QVERIFY( file.flush() );

    QString fileName;
    {
        QTemporaryFile temp;

        QVERIFY( temp.fileName().isEmpty() );
        QVERIFY( !temp.fileTemplate().isEmpty() );

        temp.setFileTemplate( fileTemplate );

        QVERIFY( temp.fileName().isEmpty() );
        QCOMPARE( temp.fileTemplate(), fileTemplate );

        QVERIFY( !temp.open() );

        QVERIFY( temp.fileName().isEmpty() );
        QCOMPARE( temp.fileTemplate(), fileTemplate );

        temp.setFileTemplate( fileTemplate2 );
        QVERIFY( temp.open() );

        fileName = temp.fileName();
        QVERIFY( QFile::exists(fileName) );
        QVERIFY( !fileName.isEmpty() );
        QVERIFY2( fileName != fileTemplate2,
            ("Generated name shouldn't be same as template: " + fileTemplate2).toLocal8Bit().constData() );
    }

    QVERIFY( !QFile::exists(fileName) );

    file.seek(0);
    QCOMPARE( QString(file.readAll()), QString(fileContent) );
    QVERIFY( file.remove() );
}

void tst_QTemporaryFile::setTemplateAfterOpen()
{
    QTemporaryFile temp;

    QVERIFY( temp.fileName().isEmpty() );
    QVERIFY( !temp.fileTemplate().isEmpty() );

    QVERIFY( temp.open() );

    QString const fileName = temp.fileName();
    QString const newTemplate("funny-path/funny-name-XXXXXX.tmp");

    QVERIFY( !fileName.isEmpty() );
    QVERIFY( QFile::exists(fileName) );
    QVERIFY( !temp.fileTemplate().isEmpty() );
    QVERIFY( temp.fileTemplate() != newTemplate );

    temp.close(); // QTemporaryFile::setFileTemplate will assert on isOpen() up to 4.5.2
    temp.setFileTemplate(newTemplate);
    QCOMPARE( temp.fileTemplate(), newTemplate );

    QVERIFY( temp.open() );
    QCOMPARE( temp.fileName(), fileName );
    QCOMPARE( temp.fileTemplate(), newTemplate );
}

void tst_QTemporaryFile::autoRemoveAfterFailedRename()
{
    struct CleanOnReturn
    {
        ~CleanOnReturn()
        {
            if (!tempName.isEmpty())
                QFile::remove(tempName);
        }

        void reset()
        {
            tempName.clear();
        }

        QString tempName;
    };

    CleanOnReturn cleaner;

    {
        QTemporaryFile file;
        QVERIFY( file.open() );
        cleaner.tempName = file.fileName();

        QVERIFY( QFile::exists(cleaner.tempName) );
        QVERIFY( !QFileInfo("i-do-not-exist").isDir() );
        QVERIFY( !file.rename("i-do-not-exist/file.txt") );
        QVERIFY( QFile::exists(cleaner.tempName) );
    }

    QVERIFY( !QFile::exists(cleaner.tempName) );
    cleaner.reset();
}

QTEST_MAIN(tst_QTemporaryFile)
#include "tst_qtemporaryfile.moc"
