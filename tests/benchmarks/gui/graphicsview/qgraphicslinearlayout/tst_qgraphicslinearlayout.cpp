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
#include <QtGui/qgraphicslinearlayout.h>
#include <QtGui/qgraphicswidget.h>
#include <QtGui/qgraphicsview.h>

class tst_QGraphicsLinearLayout : public QObject
{
    Q_OBJECT
public:
    tst_QGraphicsLinearLayout() {}
    ~tst_QGraphicsLinearLayout() {}

private slots:
    void heightForWidth_data();
    void heightForWidth();
};


struct MySquareWidget : public QGraphicsWidget
{
    MySquareWidget() {}
    virtual QSizeF  sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const
    {
        if (which != Qt::PreferredSize)
            return QGraphicsWidget::sizeHint(which, constraint);
        if (constraint.width() < 0)
            return QGraphicsWidget::sizeHint(which, constraint);
        return QSizeF(constraint.width(), constraint.width());
    }
};

void tst_QGraphicsLinearLayout::heightForWidth_data()
{
    QTest::addColumn<bool>("hfw");
    QTest::addColumn<bool>("nested");

    QTest::newRow("hfw") << true << false;
    QTest::newRow("hfw, nested") << true << true;
    QTest::newRow("not hfw") << false << false;
    QTest::newRow("not hfw, nested") << false << true;
}

void tst_QGraphicsLinearLayout::heightForWidth()
{
    QFETCH(bool, hfw);
    QFETCH(bool, nested);

    QGraphicsScene scene;
    QGraphicsWidget *form = new QGraphicsWidget;
    scene.addItem(form);

    QGraphicsLinearLayout *outerlayout = 0;
    if (nested) {
       outerlayout = new QGraphicsLinearLayout(form);
       for (int i = 0; i < 8; i++) {
           QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
           outerlayout->addItem(layout);
           outerlayout = layout;
       }
    }

    QGraphicsLinearLayout *qlayout = 0;
    qlayout = new QGraphicsLinearLayout(Qt::Vertical);
    if (nested)
        outerlayout->addItem(qlayout);
    else
        form->setLayout(qlayout);

    MySquareWidget *widget = new MySquareWidget;
    for (int i = 0; i < 1; i++) {
        widget = new MySquareWidget;
        QSizePolicy sizepolicy = widget->sizePolicy();
        sizepolicy.setHeightForWidth(hfw);
        widget->setSizePolicy(sizepolicy);
        qlayout->addItem(widget);
    }
    // make sure only one iteration is done.
    // run with tst_QGraphicsLinearLayout.exe "heightForWidth" -tickcounter -iterations 6
    // this will iterate 6 times the whole test, (not only the benchmark)
    // which should reduce warmup time and give a realistic picture of the performance of
    // effectiveSizeHint()
    QSizeF constraint(hfw ? 100 : -1, -1);
    QBENCHMARK {
        (void)form->effectiveSizeHint(Qt::PreferredSize, constraint);
    }

}


QTEST_MAIN(tst_QGraphicsLinearLayout)

#include "tst_qgraphicslinearlayout.moc"
