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

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtTest/QtTest>

QT_BEGIN_NAMESPACE
namespace QtSharedPointer {
    Q_CORE_EXPORT void internalSafetyCheckCleanCheck();
}
QT_END_NAMESPACE

class tst_QSharedPointer_and_QWidget: public QObject
{
    Q_OBJECT
private slots:
    void weak_externalDelete();
    void weak_parentDelete();
    void weak_parentDelete_setParent();

    void strong_weak();

    void strong_sharedptrDelete();

public slots:
    void cleanup() { check(); }

public:
    inline void check()
    {
#ifdef QT_BUILD_INTERNAL
        QtSharedPointer::internalSafetyCheckCleanCheck();
#endif
    }
};

void tst_QSharedPointer_and_QWidget::weak_externalDelete()
{
    QWidget *w = new QWidget;
    QWeakPointer<QWidget> ptr = w;

    QVERIFY(!ptr.isNull());

    delete w;
    QVERIFY(ptr.isNull());
}

void tst_QSharedPointer_and_QWidget::weak_parentDelete()
{
    QWidget *parent = new QWidget;
    QWidget *w = new QWidget(parent);
    QWeakPointer<QWidget> ptr = w;

    QVERIFY(!ptr.isNull());

    delete parent;
    QVERIFY(ptr.isNull());
}

void tst_QSharedPointer_and_QWidget::weak_parentDelete_setParent()
{
    QWidget *parent = new QWidget;
    QWidget *w = new QWidget;
    QWeakPointer<QWidget> ptr = w;
    w->setParent(parent);

    QVERIFY(!ptr.isNull());

    delete parent;
    QVERIFY(ptr.isNull());
}

// -- mixed --

void tst_QSharedPointer_and_QWidget::strong_weak()
{
    QSharedPointer<QWidget> ptr(new QWidget);
    QWeakPointer<QWidget> weak = ptr.data();
    QWeakPointer<QWidget> weak2 = ptr;

    QVERIFY(!weak.isNull());
    QVERIFY(!weak2.isNull());

    ptr.clear(); // deletes

    QVERIFY(weak.isNull());
    QVERIFY(weak2.isNull());
}


// ---- strong management ----

void tst_QSharedPointer_and_QWidget::strong_sharedptrDelete()
{
    QWidget *parent = new QWidget;
    QSharedPointer<QWidget> ptr(new QWidget(parent));
    QWeakPointer<QWidget> weak = ptr;
    QPointer<QWidget> check = ptr.data();

    QVERIFY(!check.isNull());
    QVERIFY(!weak.isNull());

    ptr.clear();  // deletes

    QVERIFY(check.isNull());
    QVERIFY(weak.isNull());

    delete parent; // mustn't crash
}

QTEST_MAIN(tst_QSharedPointer_and_QWidget)

#include "tst_qsharedpointer_and_qwidget.moc"
