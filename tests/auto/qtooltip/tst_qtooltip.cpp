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
#include <qtooltip.h>
#include "../../shared/util.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QToolTip : public QObject
{
    Q_OBJECT

public:
    tst_QToolTip() {}
    virtual ~tst_QToolTip() {}

public slots:
    void initTestCase() {}
    void cleanupTestCase() {}
    void init() {}
    void cleanup() {}

private slots:

    // task-specific tests below me
    void task183679_data();
    void task183679();
    void whatsThis();
    void setPalette();
};

class Widget_task183679 : public QWidget
{
    Q_OBJECT
public:
    Widget_task183679(QWidget *parent = 0) : QWidget(parent) {}

    void showDelayedToolTip(int msecs)
    {
        QTimer::singleShot(msecs, this, SLOT(showToolTip()));
    }

private slots:
    void showToolTip()
    {
        QToolTip::showText(mapToGlobal(QPoint(0, 0)), "tool tip text", this);
    }
};

Q_DECLARE_METATYPE(Qt::Key)

void tst_QToolTip::task183679_data()
{
    QTest::addColumn<Qt::Key>("key");
    QTest::addColumn<bool>("visible");
#ifdef Q_WS_MAC
    const bool visibleAfterNonModifier = false;
#else
    const bool visibleAfterNonModifier = true;
#endif
    QTest::newRow("non-modifier") << Qt::Key_A << visibleAfterNonModifier;
    QTest::newRow("Shift") << Qt::Key_Shift << true;
    QTest::newRow("Control") << Qt::Key_Control << true;
    QTest::newRow("Alt") << Qt::Key_Alt << true;
    QTest::newRow("Meta") << Qt::Key_Meta << true;
}

void tst_QToolTip::task183679()
{
    QFETCH(Qt::Key, key);
    QFETCH(bool, visible);

    Widget_task183679 widget;
    widget.show();
    QApplication::setActiveWindow(&widget);
    QTest::qWaitForWindowShown(&widget);
    QTest::qWait(30);


    widget.showDelayedToolTip(100);
    QTest::qWait(300);
    QTRY_VERIFY(QToolTip::isVisible());

    QTest::keyPress(&widget, key);

    // Important: the following delay must be larger than the duration of the timer potentially
    // initiated by the key press (currently 300 msecs), but smaller than the minimum
    // auto-close timeout (currently 10000 msecs)
    QTest::qWait(1500);

    QCOMPARE(QToolTip::isVisible(), visible);
}

#include <QWhatsThis>

void tst_QToolTip::whatsThis()
{
    qApp->setStyleSheet( "QWidget { font-size: 72px; }" );
    QWhatsThis::showText(QPoint(0,0), "THis is text");
    QTest::qWait(400);	
    QWidget *whatsthis = 0;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->inherits("QWhatsThat")) {
            whatsthis = widget;
            break;
        }
    }
    QVERIFY(whatsthis);
    QVERIFY(whatsthis->isVisible());
    QVERIFY(whatsthis->height() > 100); // Test QTBUG-2416
    qApp->setStyleSheet("");
}


void tst_QToolTip::setPalette()
{
    //the previous test may still have a tooltip pending for deletion
    QVERIFY(!QToolTip::isVisible());

    QToolTip::showText(QPoint(), "tool tip text", 0);

    QTRY_VERIFY(QToolTip::isVisible());

    QWidget *toolTip = 0;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if (widget->windowType() == Qt::ToolTip
            && widget->objectName() == QLatin1String("qtooltip_label"))
        {
            toolTip = widget;
            break;
        }
    }

    QVERIFY(toolTip);
    QTRY_VERIFY(toolTip->isVisible());

    const QPalette oldPalette = toolTip->palette();
    QPalette newPalette = oldPalette;
    newPalette.setColor(QPalette::ToolTipBase, Qt::red);
    newPalette.setColor(QPalette::ToolTipText, Qt::blue);
    QToolTip::setPalette(newPalette);
    QCOMPARE(toolTip->palette(), newPalette);
}

QTEST_MAIN(tst_QToolTip)
#include "tst_qtooltip.moc"
