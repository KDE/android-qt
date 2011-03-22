/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtGui/QDesktopWidget>
#include <QDebug>

//TESTED_CLASS=

class tst_QDesktopWidget : public QObject
{
    Q_OBJECT

public:
    tst_QDesktopWidget();
    virtual ~tst_QDesktopWidget();

public slots:
    void init();
    void cleanup();

private slots:
    void numScreens();
    void primaryScreen();
    void screenNumberForQWidget();
    void screenNumberForQPoint();
    void availableGeometry();
    void screenGeometry();
};

tst_QDesktopWidget::tst_QDesktopWidget()
{
}

tst_QDesktopWidget::~tst_QDesktopWidget()
{
}

void tst_QDesktopWidget::init()
{
}

void tst_QDesktopWidget::cleanup()
{
}

void tst_QDesktopWidget::numScreens()
{
    QDesktopWidget desktop;
    QVERIFY(desktop.numScreens() > 0);
}

void tst_QDesktopWidget::primaryScreen()
{
    QDesktopWidget desktop;
    QVERIFY(desktop.primaryScreen() >= 0);
    QVERIFY(desktop.primaryScreen() < desktop.numScreens());
}

void tst_QDesktopWidget::availableGeometry()
{
    QDesktopWidget desktop;
    QTest::ignoreMessage(QtWarningMsg, "QDesktopWidget::availableGeometry(): Attempt "
                                       "to get the available geometry of a null widget");
    desktop.availableGeometry((QWidget *)0);

    QRect total;
    QRect available;

    for (int i = 0; i < desktop.screenCount(); ++i) {
        total = desktop.screenGeometry(i);
        available = desktop.availableGeometry(i);
        QVERIFY(total.contains(available));
    }

    total = desktop.screenGeometry();
    available = desktop.availableGeometry();
    QVERIFY(total.contains(available));
    QCOMPARE(desktop.availableGeometry(desktop.primaryScreen()), available);
    QCOMPARE(desktop.screenGeometry(desktop.primaryScreen()), total);
}

void tst_QDesktopWidget::screenNumberForQWidget()
{
    QDesktopWidget desktop;

    QCOMPARE(desktop.screenNumber(0), 0);

    QWidget widget;
    widget.show();
    QApplication::processEvents();
    QVERIFY(widget.isVisible());

    int widgetScreen = desktop.screenNumber(&widget);
    QVERIFY(widgetScreen > -1);
    QVERIFY(widgetScreen < desktop.numScreens());
}

void tst_QDesktopWidget::screenNumberForQPoint()
{
    // make sure QDesktopWidget::screenNumber(QPoint) returns the correct screen
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect allScreens;
    for (int i = 0; i < desktopWidget->numScreens(); ++i) {
        QRect screenGeometry = desktopWidget->screenGeometry(i);
        QCOMPARE(desktopWidget->screenNumber(screenGeometry.center()), i);
        allScreens |= screenGeometry;
    }

    // make sure QDesktopWidget::screenNumber(QPoint) returns a valid screen for points that aren't on any screen
    int screen;
    screen = desktopWidget->screenNumber(allScreens.topLeft() - QPoint(1, 1));

#ifdef Q_WS_QWS
    QEXPECT_FAIL("", "Task 151710", Abort);
#endif
    QVERIFY(screen >= 0 && screen < desktopWidget->numScreens());
    screen = desktopWidget->screenNumber(allScreens.topRight() + QPoint(1, 1));
    QVERIFY(screen >= 0 && screen < desktopWidget->numScreens());
    screen = desktopWidget->screenNumber(allScreens.bottomLeft() - QPoint(1, 1));
    QVERIFY(screen >= 0 && screen < desktopWidget->numScreens());
    screen = desktopWidget->screenNumber(allScreens.bottomRight() + QPoint(1, 1));
    QVERIFY(screen >= 0 && screen < desktopWidget->numScreens());
}

void tst_QDesktopWidget::screenGeometry()
{
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QTest::ignoreMessage(QtWarningMsg, "QDesktopWidget::screenGeometry(): Attempt "
                                       "to get the screen geometry of a null widget");
    QRect r = desktopWidget->screenGeometry((QWidget *)0);
    QVERIFY(r.isNull());
    QWidget widget;
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    r = desktopWidget->screenGeometry(&widget);

    QRect total;
    QRect available;
    for (int i = 0; i < desktopWidget->screenCount(); ++i) {
        total = desktopWidget->screenGeometry(i);
        available = desktopWidget->availableGeometry(i);
    }
    QVERIFY(total.contains(r));
}

QTEST_MAIN(tst_QDesktopWidget)
#include "tst_qdesktopwidget.moc"

