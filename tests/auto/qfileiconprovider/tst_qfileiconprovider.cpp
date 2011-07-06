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
#include <qfileiconprovider.h>
#include <qfileinfo.h>

class tst_QFileIconProvider : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qfileiconprovider_data();
    void qfileiconprovider();

    void iconType_data();
    void iconType();

    void iconInfo_data();
    void iconInfo();

    void type_data();
    void type();
};

// Subclass that exposes the protected functions.
class SubQFileIconProvider : public QFileIconProvider
{
public:

};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QFileIconProvider::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QFileIconProvider::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QFileIconProvider::init()
{
}

// This will be called after every test function.
void tst_QFileIconProvider::cleanup()
{
}


void tst_QFileIconProvider::qfileiconprovider_data()
{
}

void tst_QFileIconProvider::qfileiconprovider()
{
    // don't crash
    SubQFileIconProvider provider;
}

Q_DECLARE_METATYPE(QFileIconProvider::IconType)
void tst_QFileIconProvider::iconType_data()
{
    QTest::addColumn<QFileIconProvider::IconType>("type");

    QTest::newRow("computer") << QFileIconProvider::Computer;
    QTest::newRow("desktop") << QFileIconProvider::Desktop;
    QTest::newRow("trashcan") << QFileIconProvider::Trashcan;
    QTest::newRow("network") << QFileIconProvider::Network;
    QTest::newRow("drive") << QFileIconProvider::Drive;
    QTest::newRow("folder") << QFileIconProvider::Folder;
    QTest::newRow("file") << QFileIconProvider::File;
}

// public QIcon icon(QFileIconProvider::IconType const& type) const
void tst_QFileIconProvider::iconType()
{
    QFETCH(QFileIconProvider::IconType, type);
    SubQFileIconProvider provider;
    QVERIFY(!provider.icon(type).isNull());
}

Q_DECLARE_METATYPE(QFileInfo)
void tst_QFileIconProvider::iconInfo_data()
{
    QTest::addColumn<QFileInfo>("info");
    QTest::addColumn<bool>("setPath");

    QTest::newRow("null") << QFileInfo() << false;
    QTest::newRow("drive") << QFileInfo(QDir::rootPath()) << true;
    QTest::newRow("home") << QFileInfo(QDir::homePath()) << true;
    QTest::newRow("current") << QFileInfo(QDir::currentPath()) << true;
}

// public QIcon icon(QFileInfo const& info) const
void tst_QFileIconProvider::iconInfo()
{
    QFETCH(QFileInfo, info);
    QFETCH(bool, setPath);

    if (setPath)
        QVERIFY(info.exists());
    SubQFileIconProvider provider;
    // we should always get an icon
    QVERIFY(!provider.icon(info).isNull());
}

void tst_QFileIconProvider::type_data()
{
    QTest::addColumn<QFileInfo>("info");
    // Return value is _very_ system dependent, hard to test
    // QTest::addColumn<QString>("type");

    QTest::newRow("null") << QFileInfo();
    QTest::newRow("drive") << QFileInfo(QDir::rootPath());
    QTest::newRow("home") << QFileInfo(QDir::homePath());
    QTest::newRow("current") << QFileInfo(QDir::currentPath());
    QTest::newRow("exe") << QFileInfo(QCoreApplication::applicationFilePath());
}

// public QString type(QFileInfo const& info) const
void tst_QFileIconProvider::type()
{
    QFETCH(QFileInfo, info);
    SubQFileIconProvider provider;
    QVERIFY(!provider.type(info).isEmpty());
}

QTEST_MAIN(tst_QFileIconProvider)
#include "tst_qfileiconprovider.moc"

