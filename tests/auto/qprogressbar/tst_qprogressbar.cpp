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
#include "qprogressbar.h"
#include <qapplication.h>
#include <qstyleoption.h>
#include <qdebug.h>
#include <qtimer.h>
#include <QStyleFactory>

#include "../../shared/util.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QProgressBar : public QObject
{
Q_OBJECT
private slots:
    void getSetCheck();
    void minMaxSameValue();
    void destroyIndeterminate();
    void text();
    void format();
    void setValueRepaint();
    void sizeHint();
    void formatedText_data();
    void formatedText();

    void task245201_testChangeStyleAndDelete_data();
    void task245201_testChangeStyleAndDelete();
};

// Testing get/set functions
void tst_QProgressBar::getSetCheck()
{
    QProgressBar obj1;
    // bool QProgressBar::invertedAppearance()
    // void QProgressBar::setInvertedAppearance(bool)
    obj1.setInvertedAppearance(false);
    QCOMPARE(false, obj1.invertedAppearance());
    obj1.setInvertedAppearance(true);
    QCOMPARE(true, obj1.invertedAppearance());

    // int QProgressBar::minimum()
    // void QProgressBar::setMinimum(int)
    obj1.setMinimum(0);
    QCOMPARE(0, obj1.minimum());
    obj1.setMinimum(INT_MAX);
    QCOMPARE(INT_MAX, obj1.minimum());
    obj1.setMinimum(INT_MIN);
    QCOMPARE(INT_MIN, obj1.minimum());

    // int QProgressBar::maximum()
    // void QProgressBar::setMaximum(int)
    obj1.setMaximum(0);
    QCOMPARE(0, obj1.maximum());
    obj1.setMaximum(INT_MIN);
    QCOMPARE(INT_MIN, obj1.maximum());
    obj1.setMaximum(INT_MAX);
    QCOMPARE(INT_MAX, obj1.maximum());

    // int QProgressBar::value()
    // void QProgressBar::setValue(int)
    obj1.setValue(0);
    QCOMPARE(0, obj1.value());
    obj1.setValue(INT_MIN);
    QCOMPARE(INT_MIN, obj1.value());
    obj1.setValue(INT_MAX);
    QCOMPARE(INT_MAX, obj1.value());
}

void tst_QProgressBar::minMaxSameValue()
{
    QProgressBar bar;
    bar.setRange(10, 10);
    bar.setValue(10);
    bar.show();
}

void tst_QProgressBar::destroyIndeterminate()
{
    // This test crashes on styles that animate indeterminate / busy
    // progressbars, and forget to remove the bars from internal logics when
    // it's deleted.
    QPointer<QProgressBar> bar = new QProgressBar;
    bar->setMaximum(0);
    bar->show();

    QEventLoop loop;
    QTimer::singleShot(500, bar, SLOT(deleteLater()));
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();

    QVERIFY(!bar);
}

void tst_QProgressBar::text()
{
    QProgressBar bar;
    bar.setRange(10, 10);
    bar.setValue(10);
    QCOMPARE(bar.text(), QString("100%"));
    bar.setRange(0, 10);
    QCOMPARE(bar.text(), QString("100%"));
    bar.setValue(5);
    QCOMPARE(bar.text(), QString("50%"));
    bar.setRange(0, 5);
    bar.setValue(0);
    bar.setRange(5, 5);
    QCOMPARE(bar.text(), QString());
}

class ProgressBar : public QProgressBar
{
    void paintEvent(QPaintEvent *event)
    {
        repainted = true;
        QProgressBar::paintEvent(event);
    }
public:
    bool repainted;
    using QProgressBar::initStyleOption;
};

void tst_QProgressBar::format()
{
    ProgressBar bar;
    bar.setRange(0, 10);
    bar.setValue(1);
    bar.show();
    QTest::qWaitForWindowShown(&bar);

    QTest::qWait(20);
    bar.repainted = false;
    bar.setFormat("%v of %m (%p%)");
    QTest::qWait(20);
    QTRY_VERIFY(bar.repainted);
    bar.repainted = false;
    bar.setFormat("%v of %m (%p%)");
    qApp->processEvents();

#ifndef Q_WS_MAC
    // Animated scroll bars get paint events all the time
#ifdef Q_OS_WIN
    if (QSysInfo::WindowsVersion < QSysInfo::WV_VISTA)
#endif
    QVERIFY(!bar.repainted);
#endif

    QCOMPARE(bar.text(), QString("1 of 10 (10%)"));
    bar.setRange(5, 5);
    bar.setValue(5);
    QCOMPARE(bar.text(), QString("5 of 0 (100%)"));
    bar.setRange(0, 5);
    bar.setValue(0);
    bar.setRange(5, 5);
    QCOMPARE(bar.text(), QString());
}

void tst_QProgressBar::setValueRepaint()
{
    ProgressBar pbar;
    pbar.setMinimum(0);
    pbar.setMaximum(10);
    pbar.setFormat("%v");
    pbar.show();
    QTest::qWaitForWindowShown(&pbar);

    QApplication::processEvents();
    for (int i = pbar.minimum(); i < pbar.maximum(); ++i) {
        pbar.repainted = false;
        pbar.setValue(i);
        QTest::qWait(50);
        QTRY_VERIFY(pbar.repainted);
    }
}

void tst_QProgressBar::sizeHint()
{
    ProgressBar bar;
    bar.setMinimum(0);
    bar.setMaximum(10);
    bar.setValue(5);

    //test if the sizeHint is big enough
    QFontMetrics fm = bar.fontMetrics();
    QStyleOptionProgressBarV2 opt;
    bar.initStyleOption(&opt);
    QSize size = QSize(9 * 7 + fm.width(QLatin1Char('0')) * 4, fm.height() + 8);
    size= bar.style()->sizeFromContents(QStyle::CT_ProgressBar, &opt, size, &bar);
    QSize barSize = bar.sizeHint();
    QVERIFY(barSize.width() >= size.width());
    QCOMPARE(barSize.height(), size.height());
}

void tst_QProgressBar::formatedText_data()
{
    QTest::addColumn<int>("minimum");
    QTest::addColumn<int>("maximum");
    QTest::addColumn<int>("value");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("text");

    QTest::newRow("1") <<  -100 << 100 << 0 << QString::fromLatin1(" %p - %v - %m ") << QString::fromLatin1(" 50 - 0 - 200 ");
    QTest::newRow("2") <<  -100 << 0 << -25 << QString::fromLatin1(" %p - %v - %m ") << QString::fromLatin1(" 75 - -25 - 100 ");
    QTest::newRow("3") <<  10 << 10 << 10 << QString::fromLatin1(" %p - %v - %m ") << QString::fromLatin1(" 100 - 10 - 0 ");
    QTest::newRow("task152227") <<  INT_MIN << INT_MAX << 42 << QString::fromLatin1(" %p - %v - %m ") << QString::fromLatin1(" 50 - 42 - 4294967295 ");
}

void tst_QProgressBar::formatedText()
{
    QFETCH(int, minimum);
    QFETCH(int, maximum);
    QFETCH(int, value);
    QFETCH(QString, format);
    QFETCH(QString, text);
    QProgressBar bar;
    bar.setRange(minimum, maximum);
    bar.setValue(value);
    bar.setFormat(format);
    QCOMPARE(bar.text(), text);
}

void tst_QProgressBar::task245201_testChangeStyleAndDelete_data()
{
    QTest::addColumn<QString>("style1_str");
    QTest::addColumn<QString>("style2_str");

    QTest::newRow("plastique-windows") << QString::fromLatin1("plastique") << QString::fromLatin1("windows");
    QTest::newRow("mlotif-windows") << QString::fromLatin1("motif") << QString::fromLatin1("windows");
    QTest::newRow("cleanlooks-cde") << QString::fromLatin1("cleanlooks") << QString::fromLatin1("cde");
    QTest::newRow("gtk-plastique") << QString::fromLatin1("gtk") << QString::fromLatin1("plastique");
}

void tst_QProgressBar::task245201_testChangeStyleAndDelete()
{
    QFETCH(QString, style1_str);
    QFETCH(QString, style2_str);

    QProgressBar *bar = new QProgressBar;

    QStyle *style = QStyleFactory::create(style1_str);
    bar->setStyle(style);
    bar->show();
    QStyle *style2 = QStyleFactory::create(style2_str);
    bar->setStyle(style2);
    QTest::qWait(10);

    delete bar;
    QTest::qWait(100); //should not crash
    delete style;
    delete style2;
}

QTEST_MAIN(tst_QProgressBar)
#include "tst_qprogressbar.moc"
