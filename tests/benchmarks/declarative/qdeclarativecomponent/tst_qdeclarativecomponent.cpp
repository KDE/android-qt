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
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QFile>
#include <QDebug>
#include "testtypes.h"

//TESTED_FILES=

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qmlcomponent : public QObject
{
    Q_OBJECT

public:
    tst_qmlcomponent();
    virtual ~tst_qmlcomponent();

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void creation_data();
    void creation();

private:
    QDeclarativeEngine engine;
};

tst_qmlcomponent::tst_qmlcomponent()
{
}

tst_qmlcomponent::~tst_qmlcomponent()
{
}

void tst_qmlcomponent::initTestCase()
{
    registerTypes();
}

void tst_qmlcomponent::cleanupTestCase()
{
}

void tst_qmlcomponent::creation_data()
{
    QTest::addColumn<QString>("file");

    QTest::newRow("Object") << SRCDIR "/data/object.qml";
    QTest::newRow("Object - Id") << SRCDIR "/data/object_id.qml";
    QTest::newRow("MyQmlObject") << SRCDIR "/data/myqmlobject.qml";
    QTest::newRow("MyQmlObject: basic binding") << SRCDIR "/data/myqmlobject_binding.qml";
    QTest::newRow("Synthesized properties") << SRCDIR "/data/synthesized_properties.qml";
    QTest::newRow("Synthesized properties.2") << SRCDIR "/data/synthesized_properties.2.qml";
    QTest::newRow("SameGame - BoomBlock") << SRCDIR "/data/samegame/BoomBlock.qml";
}

void tst_qmlcomponent::creation()
{
    QFETCH(QString, file);

    QDeclarativeComponent c(&engine, file);
    QVERIFY(c.isReady());

    QObject *obj = c.create();
    delete obj;

    QBENCHMARK {
        QObject *obj = c.create();
        delete obj;
    }
}

QTEST_MAIN(tst_qmlcomponent)
#include "tst_qdeclarativecomponent.moc"
