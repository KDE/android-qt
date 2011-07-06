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

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qboxlayout.h>
#include <qmenubar.h>
#include <qdialog.h>
#include <qsizegrip.h>
#include <qlabel.h>
#include <QtGui/QFrame>
#include <QtGui/QWindowsStyle>
#include <QtGui/QSizePolicy>
#include <QPushButton>
#include <QRadioButton>
#include <private/qlayoutengine_p.h>

#ifdef Q_OS_MAC
# include <QtGui/QMacStyle>
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_QLayout : public QObject
{
Q_OBJECT

public:
    tst_QLayout();
    virtual ~tst_QLayout();

private slots:
    void getSetCheck();
    void geometry();
    void smartMaxSize();
    void setLayoutBugs();
#ifdef QT3_SUPPORT
    void task193350_sizeGrip();
#endif
    void setContentsMargins();
    void layoutItemRect();
    void warnIfWrongParent();
    void controlTypes();
    void adjustSizeShouldMakeSureLayoutIsActivated();
};

tst_QLayout::tst_QLayout()
{
}

tst_QLayout::~tst_QLayout()
{
}

// Testing get/set functions
void tst_QLayout::getSetCheck()
{
    QBoxLayout obj1(QBoxLayout::LeftToRight);
    // QWidget * QLayout::menuBar()
    // void QLayout::setMenuBar(QWidget *)
    QMenuBar *var1 = new QMenuBar();
    obj1.setMenuBar(var1);
    QCOMPARE(static_cast<QWidget *>(var1), obj1.menuBar());
    obj1.setMenuBar((QWidget *)0);
    QCOMPARE((QWidget *)0, obj1.menuBar());
    delete var1;
}

class SizeHinterFrame : public QFrame
{
public:
    SizeHinterFrame(const QSize &sh, const QSize &msh = QSize())
    : QFrame(0), sh(sh), msh(msh) {
        setFrameStyle(QFrame::Box | QFrame::Plain);
    }



    void setSizeHint(const QSize &s) { sh = s; }
    QSize sizeHint() const { return sh; }
    QSize minimumSizeHint() const { return msh; }

private:
    QSize sh;
    QSize msh;
};


void tst_QLayout::geometry()
{
    // For QWindowsStyle we know that QWidgetItem::geometry() and QWidget::geometry()
    // should be the same.
    QApplication::setStyle(new QWindowsStyle);
    QWidget topLevel;
    QWidget w(&topLevel);
    QVBoxLayout layout(&w);
    SizeHinterFrame widget(QSize(100,100));
    layout.addWidget(&widget);
    QLayoutItem *item = layout.itemAt(0);
    topLevel.show();
    QApplication::processEvents();
    QCOMPARE(item->geometry().size(), QSize(100,100));

    widget.setMinimumSize(QSize(110,110));
    QCOMPARE(item->geometry().size(), QSize(110,110));

    widget.setMinimumSize(QSize(0,0));
    widget.setMaximumSize(QSize(90,90));
    widget.setSizeHint(QSize(100,100));
    QCOMPARE(item->geometry().size(), QSize(90,90));
}

void tst_QLayout::smartMaxSize()
{
    QVector<int> expectedWidths; 

    QFile f(QLatin1String(SRCDIR "/baseline/smartmaxsize"));

    QCOMPARE(f.open(QIODevice::ReadOnly | QIODevice::Text), true);

    QTextStream stream(&f);

    while(!stream.atEnd()) {
        QString line = stream.readLine(200);
        expectedWidths.append(line.section(QLatin1Char(' '), 6, -1, QString::SectionSkipEmpty).toInt());
    }
    f.close();

    int sizeCombinations[] = { 0, 10, 20, QWIDGETSIZE_MAX};
    QSizePolicy::Policy policies[] = {  QSizePolicy::Fixed, 
                                        QSizePolicy::Minimum, 
                                        QSizePolicy::Maximum, 
                                        QSizePolicy::Preferred,
                                        QSizePolicy::Expanding,
                                        QSizePolicy::MinimumExpanding,
                                        QSizePolicy::Ignored
                                        };
    Qt::Alignment alignments[] = {  0,
                                    Qt::AlignLeft,
                                    Qt::AlignRight,
                                    Qt::AlignHCenter
                                    };

    int expectedIndex = 0;
    int regressionCount = 0;
    for (int p = 0; p < sizeof(policies)/sizeof(QSizePolicy::Policy); ++p) {
        QSizePolicy sizePolicy;
        sizePolicy.setHorizontalPolicy(policies[p]);
        for (int min = 0; min < sizeof(sizeCombinations)/sizeof(int); ++min) {
            int minSize = sizeCombinations[min];
            for (int max = 0; max < sizeof(sizeCombinations)/sizeof(int); ++max) {
                int maxSize = sizeCombinations[max];
                for (int sh = 0; sh < sizeof(sizeCombinations)/sizeof(int); ++sh) {
                    int sizeHint = sizeCombinations[sh];
                    for (int a = 0; a < sizeof(alignments)/sizeof(int); ++a) {
                        Qt::Alignment align = alignments[a];
                        QSize sz = qSmartMaxSize(QSize(sizeHint, 1), QSize(minSize, 1), QSize(maxSize, 1), sizePolicy, align);
                        int width = sz.width();
#if 0
                        qDebug() << expectedIndex << sizePolicy.horizontalPolicy() << align << minSize << sizeHint << maxSize << width;
#else
                        int expectedWidth = expectedWidths[expectedIndex];
                        if (width != expectedWidth) {
                            qDebug() << "error at index" << expectedIndex << ":" << sizePolicy.horizontalPolicy() << align << minSize << sizeHint << maxSize << width;
                            ++regressionCount;
                        }
#endif
                        ++expectedIndex;
                    }
                }
            }
        }
    }
    QCOMPARE(regressionCount, 0);
}

void tst_QLayout::setLayoutBugs()
{
    QWidget widget(0);
    QHBoxLayout *hBoxLayout = new QHBoxLayout(&widget);

    for(int i = 0; i < 6; ++i) {
        QPushButton *pushButton = new QPushButton("Press me!", &widget);
        hBoxLayout->addWidget(pushButton);
    }

    widget.setLayout(hBoxLayout);
    QVERIFY(widget.layout() == hBoxLayout);

    QWidget containerWidget(0);
    containerWidget.setLayout(widget.layout());
    QVERIFY(widget.layout() == 0);
    QVERIFY(containerWidget.layout() == hBoxLayout);
}

#ifdef QT3_SUPPORT
void tst_QLayout::task193350_sizeGrip()
{
    QDialog dialog;
    dialog.setSizeGripEnabled(true);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setAutoAdd(true);
    new QLabel("Label", &dialog);

    dialog.show();
    QCOMPARE(layout->indexOf(qFindChild<QSizeGrip *>(&dialog)),-1);
}
#endif

class MyLayout : public QLayout
{
    public:
        MyLayout() : invalidated(false) {}
        virtual void invalidate() {invalidated = true;}
        bool invalidated;
        QSize sizeHint() const {return QSize();}
        void addItem(QLayoutItem*) {}
        QLayoutItem* itemAt(int) const {return 0;}
        QLayoutItem* takeAt(int) {return 0;}
        int count() const {return 0;}
};

void tst_QLayout::setContentsMargins()
{
    MyLayout layout;
    layout.invalidated = false;
    int left, top, right, bottom;
    
    layout.setContentsMargins(52, 53, 54, 55);
    QVERIFY(layout.invalidated);
    layout.invalidated = false;

    layout.getContentsMargins(&left, &top, &right, &bottom);
    QCOMPARE(left, 52);
    QCOMPARE(top, 53);
    QCOMPARE(right, 54);
    QCOMPARE(bottom, 55);
 
    layout.setContentsMargins(52, 53, 54, 55);
    QVERIFY(!layout.invalidated);
}

class EventReceiver : public QObject
{
public:
    bool eventFilter(QObject *watched, QEvent *event)
    {
        if (event->type() == QEvent::Show) {
            geom = static_cast<QWidget*>(watched)->geometry();
        }
        return false;
    }
    QRect geom;
};

void tst_QLayout::layoutItemRect()
{
#ifdef Q_OS_MAC
    if (qobject_cast<QMacStyle*>(QApplication::style())) {
        QWidget *window = new QWidget;
        QRadioButton *radio = new QRadioButton(window);
        QWidgetItem item(radio);
        EventReceiver eventReceiver;
        radio->installEventFilter(&eventReceiver);

        radio->show();
        QApplication::processEvents();
        QApplication::processEvents();
        QSize s = item.sizeHint();

        item.setAlignment(Qt::AlignVCenter);
        item.setGeometry(QRect(QPoint(0, 0), s));

        QCOMPARE(radio->geometry().size(), radio->sizeHint());
        delete radio;
    }
#endif
}

void tst_QLayout::warnIfWrongParent()
{
    QWidget root;
    QHBoxLayout lay;
    lay.setParent(&root);
    QTest::ignoreMessage(QtWarningMsg, "QLayout::parentWidget: A layout can only have another layout as a parent.");
    QCOMPARE(lay.parentWidget(), static_cast<QWidget*>(0));
}

void tst_QLayout::controlTypes()
{
    QVBoxLayout layout;
    QCOMPARE(layout.controlTypes(), QSizePolicy::DefaultType);
    QSizePolicy p;
    QCOMPARE(p.controlType(),QSizePolicy::DefaultType);

}

void tst_QLayout::adjustSizeShouldMakeSureLayoutIsActivated()
{
    QWidget main;

    QVBoxLayout *const layout = new QVBoxLayout(&main);
    layout->setMargin(0);
    SizeHinterFrame *frame = new SizeHinterFrame(QSize(200, 10), QSize(200, 8));
    frame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(frame);

    SizeHinterFrame *frame2 = new SizeHinterFrame(QSize(200, 10), QSize(200, 8));
    frame2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(frame2);

    main.show();

    frame2->hide();
    main.adjustSize();
    QCOMPARE(main.size(), QSize(200, 10));
}

QTEST_MAIN(tst_QLayout)
#include "tst_qlayout.moc"
