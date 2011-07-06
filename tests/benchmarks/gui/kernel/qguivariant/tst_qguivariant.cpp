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

#include <qtest.h>
#include <QtCore/qvariant.h>

#define ITERATION_COUNT 1e5

class tst_QGuiVariant : public QObject
{
    Q_OBJECT

public:
    tst_QGuiVariant();
    virtual ~tst_QGuiVariant();

private slots:
    void createGuiType_data();
    void createGuiType();
    void createGuiTypeCopy_data();
    void createGuiTypeCopy();
};

tst_QGuiVariant::tst_QGuiVariant()
{
}

tst_QGuiVariant::~tst_QGuiVariant()
{
}

void tst_QGuiVariant::createGuiType_data()
{
    QTest::addColumn<int>("typeId");
    for (int i = QMetaType::FirstGuiType; i <= QMetaType::LastGuiType; ++i) {
#ifndef QT3_SUPPORT
        if (i != 63) // QMetaType::QColorGroup (63) requires QT3_SUPPORT
#endif
        QTest::newRow(QMetaType::typeName(i)) << i;
    }
}

// Tests how fast a Qt GUI type can be default-constructed by a
// QVariant. The purpose of this benchmark is to measure the overhead
// of creating (and destroying) a QVariant compared to creating the
// type directly.
void tst_QGuiVariant::createGuiType()
{
    QFETCH(int, typeId);
    QBENCHMARK {
        for (int i = 0; i < ITERATION_COUNT; ++i)
            QVariant(typeId, (void *)0);
    }
}

void tst_QGuiVariant::createGuiTypeCopy_data()
{
    createGuiType_data();
}

// Tests how fast a Qt GUI type can be copy-constructed by a
// QVariant. The purpose of this benchmark is to measure the overhead
// of creating (and destroying) a QVariant compared to creating the
// type directly.
void tst_QGuiVariant::createGuiTypeCopy()
{
    QFETCH(int, typeId);
    QVariant other(typeId, (void *)0);
    const void *copy = other.constData();
    QBENCHMARK {
        for (int i = 0; i < ITERATION_COUNT; ++i)
            QVariant(typeId, copy);
    }
}

QTEST_MAIN(tst_QGuiVariant)
#include "tst_qguivariant.moc"
