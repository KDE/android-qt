/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qabstractproxymodel.h>
#include <QItemSelection>
#include <qstandarditemmodel.h>

class tst_QAbstractProxyModel : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qabstractproxymodel_data();
    void qabstractproxymodel();
    void data_data();
    void data();
    void flags_data();
    void flags();
    void headerData_data();
    void headerData();
    void itemData_data();
    void itemData();
    void mapFromSource_data();
    void mapFromSource();
    void mapSelectionFromSource_data();
    void mapSelectionFromSource();
    void mapSelectionToSource_data();
    void mapSelectionToSource();
    void mapToSource_data();
    void mapToSource();
    void revert_data();
    void revert();
    void setSourceModel_data();
    void setSourceModel();
    void submit_data();
    void submit();
    void testRoleNames();
};

// Subclass that exposes the protected functions.
class SubQAbstractProxyModel : public QAbstractProxyModel
{
public:
    // QAbstractProxyModel::mapFromSource is a pure virtual function.
    QModelIndex mapFromSource(QModelIndex const& sourceIndex) const
        { Q_UNUSED(sourceIndex); return QModelIndex(); }

    // QAbstractProxyModel::mapToSource is a pure virtual function.
    QModelIndex mapToSource(QModelIndex const& proxyIndex) const
        { Q_UNUSED(proxyIndex); return QModelIndex(); }

    QModelIndex index(int, int, const QModelIndex&) const
    {
        return QModelIndex();
    }

    QModelIndex parent(const QModelIndex&) const
    {
        return QModelIndex();
    }

    int rowCount(const QModelIndex&) const
    {
        return 0;
    }

    int columnCount(const QModelIndex&) const
    {
        return 0;
    }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QAbstractProxyModel::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QAbstractProxyModel::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QAbstractProxyModel::init()
{
}

// This will be called after every test function.
void tst_QAbstractProxyModel::cleanup()
{
}

void tst_QAbstractProxyModel::qabstractproxymodel_data()
{
}

void tst_QAbstractProxyModel::qabstractproxymodel()
{
    SubQAbstractProxyModel model;
    model.data(QModelIndex());
    model.flags(QModelIndex());
    model.headerData(0, Qt::Vertical, 0);
    model.itemData(QModelIndex());
    model.mapFromSource(QModelIndex());
    model.mapSelectionFromSource(QItemSelection());
    model.mapSelectionToSource(QItemSelection());
    model.mapToSource(QModelIndex());
    model.revert();
    model.setSourceModel(0);
    QCOMPARE(model.sourceModel(), (QAbstractItemModel*)0);
    model.submit();
}

Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QModelIndex)
void tst_QAbstractProxyModel::data_data()
{
    QTest::addColumn<QModelIndex>("proxyIndex");
    QTest::addColumn<int>("role");
    QTest::addColumn<QVariant>("data");
    QTest::newRow("null") << QModelIndex() << 0 << QVariant();
}

// public QVariant data(QModelIndex const& proxyIndex, int role = Qt::DisplayRole) const
void tst_QAbstractProxyModel::data()
{
    QFETCH(QModelIndex, proxyIndex);
    QFETCH(int, role);
    QFETCH(QVariant, data);

    SubQAbstractProxyModel model;
    QCOMPARE(model.data(proxyIndex, role), data);
}

Q_DECLARE_METATYPE(Qt::ItemFlags)
void tst_QAbstractProxyModel::flags_data()
{
    QTest::addColumn<QModelIndex>("index");
    QTest::addColumn<Qt::ItemFlags>("flags");
    QTest::newRow("null") << QModelIndex() << (Qt::ItemFlags)0;
}

// public Qt::ItemFlags flags(QModelIndex const& index) const
void tst_QAbstractProxyModel::flags()
{
    QFETCH(QModelIndex, index);
    QFETCH(Qt::ItemFlags, flags);

    SubQAbstractProxyModel model;
    QCOMPARE(model.flags(index), flags);
}

Q_DECLARE_METATYPE(Qt::Orientation)
Q_DECLARE_METATYPE(Qt::ItemDataRole)
void tst_QAbstractProxyModel::headerData_data()
{
    QTest::addColumn<int>("section");
    QTest::addColumn<Qt::Orientation>("orientation");
    QTest::addColumn<Qt::ItemDataRole>("role");
    QTest::addColumn<QVariant>("headerData");
    QTest::newRow("null") << 0 << Qt::Vertical << Qt::UserRole << QVariant();
}

// public QVariant headerData(int section, Qt::Orientation orientation, int role) const
void tst_QAbstractProxyModel::headerData()
{
    QFETCH(int, section);
    QFETCH(Qt::Orientation, orientation);
    QFETCH(Qt::ItemDataRole, role);
    QFETCH(QVariant, headerData);

    SubQAbstractProxyModel model;
    QCOMPARE(model.headerData(section, orientation, role), headerData);
}

void tst_QAbstractProxyModel::itemData_data()
{
    QTest::addColumn<QModelIndex>("index");
    QTest::addColumn<int>("count");

    QTest::newRow("null") << QModelIndex() << 0;
}

// public QMap<int,QVariant> itemData(QModelIndex const& index) const
void tst_QAbstractProxyModel::itemData()
{
    QFETCH(QModelIndex, index);
    QFETCH(int, count);
    SubQAbstractProxyModel model;
    QCOMPARE(model.itemData(index).count(), count);
}

void tst_QAbstractProxyModel::mapFromSource_data()
{
    QTest::addColumn<QModelIndex>("sourceIndex");
    QTest::addColumn<QModelIndex>("mapFromSource");
    QTest::newRow("null") << QModelIndex() << QModelIndex();
}

// public QModelIndex mapFromSource(QModelIndex const& sourceIndex) const
void tst_QAbstractProxyModel::mapFromSource()
{
    QFETCH(QModelIndex, sourceIndex);
    QFETCH(QModelIndex, mapFromSource);

    SubQAbstractProxyModel model;
    QCOMPARE(model.mapFromSource(sourceIndex), mapFromSource);
}

Q_DECLARE_METATYPE(QItemSelection)
void tst_QAbstractProxyModel::mapSelectionFromSource_data()
{
    QTest::addColumn<QItemSelection>("selection");
    QTest::addColumn<QItemSelection>("mapSelectionFromSource");
    QTest::newRow("null") << QItemSelection() << QItemSelection();
    QTest::newRow("empty") << QItemSelection(QModelIndex(), QModelIndex()) << QItemSelection(QModelIndex(), QModelIndex());
}

// public QItemSelection mapSelectionFromSource(QItemSelection const& selection) const
void tst_QAbstractProxyModel::mapSelectionFromSource()
{
    QFETCH(QItemSelection, selection);
    QFETCH(QItemSelection, mapSelectionFromSource);

    SubQAbstractProxyModel model;
    QCOMPARE(model.mapSelectionFromSource(selection), mapSelectionFromSource);
}

void tst_QAbstractProxyModel::mapSelectionToSource_data()
{
    QTest::addColumn<QItemSelection>("selection");
    QTest::addColumn<QItemSelection>("mapSelectionToSource");
    QTest::newRow("null") << QItemSelection() << QItemSelection();
    QTest::newRow("empty") << QItemSelection(QModelIndex(), QModelIndex()) << QItemSelection(QModelIndex(), QModelIndex());
}

// public QItemSelection mapSelectionToSource(QItemSelection const& selection) const
void tst_QAbstractProxyModel::mapSelectionToSource()
{
    QFETCH(QItemSelection, selection);
    QFETCH(QItemSelection, mapSelectionToSource);

    SubQAbstractProxyModel model;
    QCOMPARE(model.mapSelectionToSource(selection), mapSelectionToSource);
}

void tst_QAbstractProxyModel::mapToSource_data()
{
    QTest::addColumn<QModelIndex>("proxyIndex");
    QTest::addColumn<QModelIndex>("mapToSource");
    QTest::newRow("null") << QModelIndex() << QModelIndex();
}

// public QModelIndex mapToSource(QModelIndex const& proxyIndex) const
void tst_QAbstractProxyModel::mapToSource()
{
    QFETCH(QModelIndex, proxyIndex);
    QFETCH(QModelIndex, mapToSource);

    SubQAbstractProxyModel model;
    QCOMPARE(model.mapToSource(proxyIndex), mapToSource);
}

void tst_QAbstractProxyModel::revert_data()
{
    //QTest::addColumn<int>("foo");
    //QTest::newRow("null") << 0;
}

// public void revert()
void tst_QAbstractProxyModel::revert()
{
    //QFETCH(int, foo);

    SubQAbstractProxyModel model;
    model.revert();
}

void tst_QAbstractProxyModel::setSourceModel_data()
{
    //QTest::addColumn<int>("sourceModelCount");
    //QTest::newRow("null") << 0;
}

// public void setSourceModel(QAbstractItemModel* sourceModel)
void tst_QAbstractProxyModel::setSourceModel()
{
    //QFETCH(int, sourceModelCount);

    SubQAbstractProxyModel model;
    QStandardItemModel *sourceModel = new QStandardItemModel(&model);
    model.setSourceModel(sourceModel);
    QCOMPARE(model.sourceModel(), static_cast<QAbstractItemModel*>(sourceModel));

    QStandardItemModel *sourceModel2 = new QStandardItemModel(&model);
    model.setSourceModel(sourceModel2);
    QCOMPARE(model.sourceModel(), static_cast<QAbstractItemModel*>(sourceModel2));

    delete sourceModel2;
    QCOMPARE(model.sourceModel(), static_cast<QAbstractItemModel*>(0));
}

void tst_QAbstractProxyModel::submit_data()
{
    QTest::addColumn<bool>("submit");
    QTest::newRow("null") << true;
}

// public bool submit()
void tst_QAbstractProxyModel::submit()
{
    QFETCH(bool, submit);

    SubQAbstractProxyModel model;
    QCOMPARE(model.submit(), submit);
}

class StandardItemModelWithCustomRoleNames : public QStandardItemModel
{
public:
    enum CustomRole {
        CustomRole1 = Qt::UserRole,
        CustomRole2
    };

    StandardItemModelWithCustomRoleNames() {
        QHash<int, QByteArray> _roleNames = roleNames();
        _roleNames.insert(CustomRole1, "custom1");
        _roleNames.insert(CustomRole2, "custom2");
        setRoleNames(_roleNames);
    }
};

class AnotherStandardItemModelWithCustomRoleNames : public QStandardItemModel
{
    public:
        enum CustomRole {
            AnotherCustomRole1 = Qt::UserRole + 10,  // Different to StandardItemModelWithCustomRoleNames::CustomRole1
            AnotherCustomRole2
        };

        AnotherStandardItemModelWithCustomRoleNames() {
            QHash<int, QByteArray> _roleNames = roleNames();
            _roleNames.insert(AnotherCustomRole1, "another_custom1");
            _roleNames.insert(AnotherCustomRole2, "another_custom2");
            setRoleNames(_roleNames);
        }
};

/**
    Verifies that @p subSet is a subset of @p superSet. That is, all keys in @p subSet exist in @p superSet and have the same values.
*/
static void verifySubSetOf(const QHash<int, QByteArray> &superSet, const QHash<int, QByteArray> &subSet)
{
    QHash<int, QByteArray>::const_iterator it = subSet.constBegin();
    const QHash<int, QByteArray>::const_iterator end = subSet.constEnd();
    for ( ; it != end; ++it ) {
        QVERIFY(superSet.contains(it.key()));
        QVERIFY(it.value() == superSet.value(it.key()));
    }
}

void tst_QAbstractProxyModel::testRoleNames()
{
    QStandardItemModel defaultModel;
    StandardItemModelWithCustomRoleNames model;
    QHash<int, QByteArray> rootModelRoleNames = model.roleNames();
    QHash<int, QByteArray> defaultModelRoleNames = defaultModel.roleNames();

    verifySubSetOf( rootModelRoleNames, defaultModelRoleNames);
    QVERIFY( rootModelRoleNames.size() == defaultModelRoleNames.size() + 2 );
    QVERIFY( rootModelRoleNames.contains(StandardItemModelWithCustomRoleNames::CustomRole1));
    QVERIFY( rootModelRoleNames.contains(StandardItemModelWithCustomRoleNames::CustomRole2));
    QVERIFY( rootModelRoleNames.value(StandardItemModelWithCustomRoleNames::CustomRole1) == "custom1" );
    QVERIFY( rootModelRoleNames.value(StandardItemModelWithCustomRoleNames::CustomRole2) == "custom2" );

    SubQAbstractProxyModel proxy1;
    proxy1.setSourceModel(&model);
    QHash<int, QByteArray> proxy1RoleNames = proxy1.roleNames();
    verifySubSetOf( proxy1RoleNames, defaultModelRoleNames );
    QVERIFY( proxy1RoleNames.size() == defaultModelRoleNames.size() + 2 );
    QVERIFY( proxy1RoleNames.contains(StandardItemModelWithCustomRoleNames::CustomRole1));
    QVERIFY( proxy1RoleNames.contains(StandardItemModelWithCustomRoleNames::CustomRole2));
    QVERIFY( proxy1RoleNames.value(StandardItemModelWithCustomRoleNames::CustomRole1) == "custom1" );
    QVERIFY( proxy1RoleNames.value(StandardItemModelWithCustomRoleNames::CustomRole2) == "custom2" );

    SubQAbstractProxyModel proxy2;
    proxy2.setSourceModel(&proxy1);
    QHash<int, QByteArray> proxy2RoleNames = proxy2.roleNames();
    verifySubSetOf( proxy2RoleNames, defaultModelRoleNames );
    QVERIFY( proxy2RoleNames.size() == defaultModelRoleNames.size() + 2 );
    QVERIFY( proxy2RoleNames.contains(StandardItemModelWithCustomRoleNames::CustomRole1));
    QVERIFY( proxy2RoleNames.contains(StandardItemModelWithCustomRoleNames::CustomRole2));
    QVERIFY( proxy2RoleNames.value(StandardItemModelWithCustomRoleNames::CustomRole1) == "custom1" );
    QVERIFY( proxy2RoleNames.value(StandardItemModelWithCustomRoleNames::CustomRole2) == "custom2" );

}

QTEST_MAIN(tst_QAbstractProxyModel)
#include "tst_qabstractproxymodel.moc"

