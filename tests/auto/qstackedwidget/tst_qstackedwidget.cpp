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

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qstackedwidget.h>
#include <qpushbutton.h>
#include <QHBoxLayout>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QStackedWidget : public QObject
{
Q_OBJECT

public:
    tst_QStackedWidget();
    virtual ~tst_QStackedWidget();

private slots:
    void getSetCheck();
	void testMinimumSize();
};

tst_QStackedWidget::tst_QStackedWidget()
{
}

tst_QStackedWidget::~tst_QStackedWidget()
{
}

// Testing that stackedwidget respect the minimum size of it's contents (task 95319)
void tst_QStackedWidget::testMinimumSize()
{
	QWidget w;
    QStackedWidget sw(&w);
    QPushButton button("Text", &sw);
	sw.addWidget(&button);
    QHBoxLayout hboxLayout;
    hboxLayout.addWidget(&sw);
    w.setLayout(&hboxLayout);
    w.show();
	QVERIFY(w.minimumSize() != QSize(0, 0));
}

// Testing get/set functions
void tst_QStackedWidget::getSetCheck()
{
    QStackedWidget obj1;
    // int QStackedWidget::currentIndex()
    // void QStackedWidget::setCurrentIndex(int)
    obj1.setCurrentIndex(0);
    QCOMPARE(-1, obj1.currentIndex());
    obj1.setCurrentIndex(INT_MIN);
    QCOMPARE(-1, obj1.currentIndex());
    obj1.setCurrentIndex(INT_MAX);
    QCOMPARE(-1, obj1.currentIndex());

    // QWidget * QStackedWidget::currentWidget()
    // void QStackedWidget::setCurrentWidget(QWidget *)
    QWidget *var2 = new QWidget();
    obj1.addWidget(var2);
    obj1.setCurrentWidget(var2);
    QCOMPARE(var2, obj1.currentWidget());

// Disabled, task to fix is 128939.
#if 0
    // Layouts assert on any unknown widgets here, 0-pointers included.
    // This seems wrong behavior, since the setCurrentIndex(int), which
    // is really a convenience function for setCurrentWidget(QWidget*),
    // has no problem handling out-of-bounds indices.
    // ("convenience function" => "just another way of achieving the
    // same goal")
	obj1.setCurrentWidget((QWidget *)0);
    QCOMPARE(obj1.currentWidget(), var2);
#endif
    delete var2;
}

QTEST_MAIN(tst_QStackedWidget)
#include "tst_qstackedwidget.moc"
