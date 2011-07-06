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
#include <QtGui>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QBoxLayout : public QObject
{
    Q_OBJECT

public:
    tst_QBoxLayout();
    virtual ~tst_QBoxLayout();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void insertSpacerItem();
    void sizeHint();
    void sizeConstraints();
    void setGeometry();
    void setStyleShouldChangeSpacing();

    void taskQTBUG_7103_minMaxWidthNotRespected();
};

class CustomLayoutStyle : public QWindowsStyle
{
    Q_OBJECT
public:
    CustomLayoutStyle() : QWindowsStyle()
    {
        hspacing = 5;
        vspacing = 10;
    }

    virtual int pixelMetric(PixelMetric metric, const QStyleOption * option = 0,
                            const QWidget * widget = 0 ) const;

    int hspacing;
    int vspacing;
};

int CustomLayoutStyle::pixelMetric(PixelMetric metric, const QStyleOption * option /*= 0*/,
                                   const QWidget * widget /*= 0*/ ) const
{
    switch (metric) {
        case PM_LayoutLeftMargin:
            return 0;
        break;
        case PM_LayoutTopMargin:
            return 3;
        break;
        case PM_LayoutRightMargin:
            return 6;
        break;
        case PM_LayoutBottomMargin:
            return 9;
        break;
        case PM_LayoutHorizontalSpacing:
            return hspacing;
        case PM_LayoutVerticalSpacing:
            return vspacing;
        break;
        default:
            break;
    }
    return QWindowsStyle::pixelMetric(metric, option, widget);
}


tst_QBoxLayout::tst_QBoxLayout()
{
}

tst_QBoxLayout::~tst_QBoxLayout()
{
}

void tst_QBoxLayout::initTestCase()
{
}

void tst_QBoxLayout::cleanupTestCase()
{
}

void tst_QBoxLayout::init()
{
}

void tst_QBoxLayout::cleanup()
{
}

void tst_QBoxLayout::insertSpacerItem()
{
    QWidget *window = new QWidget;

    QSpacerItem *spacer1 = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLineEdit("Foooooooooooooooooooooooooo"));
    layout->addSpacerItem(spacer1);
    layout->addWidget(new QLineEdit("Baaaaaaaaaaaaaaaaaaaaaaaaar"));
    layout->insertSpacerItem(0, spacer2);
    window->setLayout(layout);

    QVERIFY(layout->itemAt(0) == spacer2);
    QVERIFY(layout->itemAt(2) == spacer1);

    window->show();
}

void tst_QBoxLayout::sizeHint()
{
    QWidget *window = new QWidget;
    QHBoxLayout *lay1 = new QHBoxLayout;
    QHBoxLayout *lay2 = new QHBoxLayout;
    QLabel *label = new QLabel("widget twooooooooooooooooooooooooooooooooooooooooooooooooooooooo");
    lay2->addWidget(label);
    lay1->addLayout(lay2);
    window->setLayout(lay1);
    window->show();
    label->setText("foooooooo baaaaaaar");
    QSize sh = lay1->sizeHint();
    QApplication::processEvents();
    // Note that this is not strictly required behaviour - actually
    // the preferred behaviour would be that sizeHint returns
    // the same value regardless of what's lying in the event queue.
    // (i.e. we would check for equality here instead)
    QVERIFY(lay1->sizeHint() != sh);
}

void tst_QBoxLayout::sizeConstraints()
{
    QWidget *window = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(new QLabel("foooooooooooooooooooooooooooooooooooo"));
    lay->addWidget(new QLabel("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaar"));
    lay->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(lay);
    window->show();
    QApplication::processEvents();
    QSize sh = window->sizeHint();
    lay->takeAt(1);
    QVERIFY(sh.width() >= window->sizeHint().width() &&
            sh.height() >= window->sizeHint().height());

}

void tst_QBoxLayout::setGeometry()
{
    QWidget toplevel;
    QWidget w(&toplevel);
    QVBoxLayout *lay = new QVBoxLayout;
    lay->setMargin(0);
    lay->setSpacing(0);
    QHBoxLayout *lay2 = new QHBoxLayout;
    QDial *dial = new QDial;
    lay2->addWidget(dial);
    lay2->setAlignment(Qt::AlignTop);
    lay2->setAlignment(Qt::AlignRight);
    lay->addLayout(lay2);
    w.setLayout(lay);
    toplevel.show();

    QRect newGeom(0, 0, 70, 70);
    lay2->setGeometry(newGeom);
    QVERIFY2(newGeom.contains(dial->geometry()), "dial->geometry() should be smaller and within newGeom");
}

void tst_QBoxLayout::setStyleShouldChangeSpacing()
{

    QWidget *window = new QWidget;
    QHBoxLayout *hbox = new QHBoxLayout(window);
    QPushButton *pb1 = new QPushButton(tr("The spacing between this"));
    QPushButton *pb2 = new QPushButton(tr("and this button should depend on the style of the parent widget"));;
    pb1->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    pb2->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    hbox->addWidget(pb1);
    hbox->addWidget(pb2);
    CustomLayoutStyle *style1 = new CustomLayoutStyle;
    style1->hspacing = 6;
    window->setStyle(style1);
    window->show();

    QTest::qWait(100);
    int spacing = pb2->geometry().left() - pb1->geometry().right() - 1;
    QCOMPARE(spacing, 6);

    CustomLayoutStyle *style2 = new CustomLayoutStyle();
    style2->hspacing = 10;
    window->setStyle(style2);
    QTest::qWait(100);
    spacing = pb2->geometry().left() - pb1->geometry().right() - 1;
    QCOMPARE(spacing, 10);

    delete window;
    delete style1;
    delete style2;
}

void tst_QBoxLayout::taskQTBUG_7103_minMaxWidthNotRespected()
{
    QLabel *label = new QLabel("Qt uses standard C++, but makes extensive use of the C pre-processor to enrich the language. Qt can also be used in several other programming languages via language bindings. It runs on all major platforms, and has extensive internationalization support. Non-GUI features include SQL database access, XML parsing, thread management, network support and a unified cross-platform API for file handling.");
    label->setWordWrap(true);
    label->setFixedWidth(200);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));

    QWidget widget;
    widget.setLayout(layout);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    int height = label->height();

    QRect g = widget.geometry();
    g.setWidth(600);
    widget.setGeometry(g);

    QTest::qWait(50);

    QCOMPARE(label->height(), height);
}

QTEST_MAIN(tst_QBoxLayout)
#include "tst_qboxlayout.moc"
