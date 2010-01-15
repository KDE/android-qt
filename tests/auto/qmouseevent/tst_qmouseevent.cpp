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
#include <qapplication.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qdialog.h>


#include <qevent.h>
#include <qwidget.h>

//TESTED_CLASS=
//TESTED_FILES=gui/widgets/qmenubar.h gui/widgets/qmenubar.cpp

class MouseEventWidget : public QWidget
{
public:
    MouseEventWidget(QWidget *parent = 0) : QWidget(parent)
    {
	setFocusPolicy(Qt::StrongFocus);
    }
    bool mousePressEventRecieved;
    bool mouseReleaseEventRecieved;
#ifdef QT3_SUPPORT
    int mousePressStateBefore;
    int mousePressStateAfter;
#endif
    int mousePressButton;
    int mousePressButtons;
    int mousePressModifiers;
#ifdef QT3_SUPPORT
    int mouseReleaseStateBefore;
    int mouseReleaseStateAfter;
#endif
    int mouseReleaseButton;
    int mouseReleaseButtons;
    int mouseReleaseModifiers;
protected:
    void mousePressEvent(QMouseEvent *e)
    {
	QWidget::mousePressEvent(e);
#ifdef QT3_SUPPORT
	mousePressStateBefore = e->state();
	mousePressStateAfter = e->stateAfter();
#endif
	mousePressButton = e->button();
	mousePressButtons = e->buttons();
	mousePressModifiers = e->modifiers();
	mousePressEventRecieved = TRUE;
	e->accept();
    }
    void mouseReleaseEvent(QMouseEvent *e)
    {
	QWidget::mouseReleaseEvent(e);
#ifdef QT3_SUPPORT
	mouseReleaseStateBefore = e->state();
	mouseReleaseStateAfter = e->stateAfter();
#endif
	mouseReleaseButton = e->button();
	mouseReleaseButtons = e->buttons();
	mouseReleaseModifiers = e->modifiers();
	mouseReleaseEventRecieved = TRUE;
	e->accept();
    }
};

class tst_QMouseEvent : public QObject
{
    Q_OBJECT

public:
    tst_QMouseEvent();
    virtual ~tst_QMouseEvent();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void checkMousePressEvent_data();
    void checkMousePressEvent();
    void checkMouseReleaseEvent_data();
    void checkMouseReleaseEvent();

    void qt3supportConstructors();

private:
    MouseEventWidget* testMouseWidget;
};



tst_QMouseEvent::tst_QMouseEvent()
{
}

tst_QMouseEvent::~tst_QMouseEvent()
{

}

void tst_QMouseEvent::initTestCase()
{
    testMouseWidget = new MouseEventWidget(0);
    testMouseWidget->show();
}

void tst_QMouseEvent::cleanupTestCase()
{
    delete testMouseWidget;
}

void tst_QMouseEvent::init()
{
    testMouseWidget->mousePressEventRecieved = FALSE;
    testMouseWidget->mouseReleaseEventRecieved = FALSE;
#ifdef QT3_SUPPORT
    testMouseWidget->mousePressStateBefore = 0;
    testMouseWidget->mousePressStateAfter = 0;
    testMouseWidget->mouseReleaseStateBefore = 0;
    testMouseWidget->mouseReleaseStateAfter = 0;
#endif
    testMouseWidget->mousePressButton = 0;
    testMouseWidget->mousePressButtons = 0;
    testMouseWidget->mousePressModifiers = 0;
    testMouseWidget->mouseReleaseButton = 0;
    testMouseWidget->mouseReleaseButtons = 0;
    testMouseWidget->mouseReleaseModifiers = 0;
}

void tst_QMouseEvent::cleanup()
{
}

void tst_QMouseEvent::checkMousePressEvent_data()
{
    QTest::addColumn<int>("buttonPressed");
    QTest::addColumn<int>("keyPressed");

    QTest::newRow("leftButton-nokey") << int(Qt::LeftButton) << int(Qt::NoButton);
    QTest::newRow("leftButton-shiftkey") << int(Qt::LeftButton) << int(Qt::ShiftModifier);
    QTest::newRow("leftButton-controlkey") << int(Qt::LeftButton) << int(Qt::ControlModifier);
    QTest::newRow("leftButton-altkey") << int(Qt::LeftButton) << int(Qt::AltModifier);
    QTest::newRow("leftButton-metakey") << int(Qt::LeftButton) << int(Qt::MetaModifier);
    QTest::newRow("rightButton-nokey") << int(Qt::RightButton) << int(Qt::NoButton);
    QTest::newRow("rightButton-shiftkey") << int(Qt::RightButton) << int(Qt::ShiftModifier);
    QTest::newRow("rightButton-controlkey") << int(Qt::RightButton) << int(Qt::ControlModifier);
    QTest::newRow("rightButton-altkey") << int(Qt::RightButton) << int(Qt::AltModifier);
    QTest::newRow("rightButton-metakey") << int(Qt::RightButton) << int(Qt::MetaModifier);
    QTest::newRow("midButton-nokey") << int(Qt::MidButton) << int(Qt::NoButton);
    QTest::newRow("midButton-shiftkey") << int(Qt::MidButton) << int(Qt::ShiftModifier);
    QTest::newRow("midButton-controlkey") << int(Qt::MidButton) << int(Qt::ControlModifier);
    QTest::newRow("midButton-altkey") << int(Qt::MidButton) << int(Qt::AltModifier);
    QTest::newRow("midButton-metakey") << int(Qt::MidButton) << int(Qt::MetaModifier);
}

void tst_QMouseEvent::checkMousePressEvent()
{
    QFETCH(int,buttonPressed);
    QFETCH(int,keyPressed);
    int button = buttonPressed;
    int buttons = button;
    int modifiers = keyPressed;

    QTest::mousePress(testMouseWidget, Qt::MouseButton(buttonPressed), Qt::KeyboardModifiers(keyPressed));
    QVERIFY(testMouseWidget->mousePressEventRecieved);
    QCOMPARE(testMouseWidget->mousePressButton, button);
    QCOMPARE(testMouseWidget->mousePressButtons, buttons);
    QCOMPARE(testMouseWidget->mousePressModifiers, modifiers);
#ifdef QT3_SUPPORT
    int stateAfter = buttons|modifiers;
    int stateBefore = stateAfter & ~button;

    QCOMPARE(testMouseWidget->mousePressStateBefore, stateBefore);
    QCOMPARE(testMouseWidget->mousePressStateAfter, stateAfter);
#endif

    QTest::mouseRelease(testMouseWidget, Qt::MouseButton(buttonPressed), Qt::KeyboardModifiers(keyPressed));
}

void tst_QMouseEvent::checkMouseReleaseEvent_data()
{
    QTest::addColumn<int>("buttonReleased");
    QTest::addColumn<int>("keyPressed");

    QTest::newRow("leftButton-nokey") << int(Qt::LeftButton) << int(Qt::NoButton);
    QTest::newRow("leftButton-shiftkey") << int(Qt::LeftButton) << int(Qt::ShiftModifier);
    QTest::newRow("leftButton-controlkey") << int(Qt::LeftButton) << int(Qt::ControlModifier);
    QTest::newRow("leftButton-altkey") << int(Qt::LeftButton) << int(Qt::AltModifier);
    QTest::newRow("leftButton-metakey") << int(Qt::LeftButton) << int(Qt::MetaModifier);
    QTest::newRow("rightButton-nokey") << int(Qt::RightButton) << int(Qt::NoButton);
    QTest::newRow("rightButton-shiftkey") << int(Qt::RightButton) << int(Qt::ShiftModifier);
    QTest::newRow("rightButton-controlkey") << int(Qt::RightButton) << int(Qt::ControlModifier);
    QTest::newRow("rightButton-altkey") << int(Qt::RightButton) << int(Qt::AltModifier);
    QTest::newRow("rightButton-metakey") << int(Qt::RightButton) << int(Qt::MetaModifier);
    QTest::newRow("midButton-nokey") << int(Qt::MidButton) << int(Qt::NoButton);
    QTest::newRow("midButton-shiftkey") << int(Qt::MidButton) << int(Qt::ShiftModifier);
    QTest::newRow("midButton-controlkey") << int(Qt::MidButton) << int(Qt::ControlModifier);
    QTest::newRow("midButton-altkey") << int(Qt::MidButton) << int(Qt::AltModifier);
    QTest::newRow("midButton-metakey") << int(Qt::MidButton) << int(Qt::MetaModifier);
}

void tst_QMouseEvent::checkMouseReleaseEvent()
{
    QFETCH(int,buttonReleased);
    QFETCH(int,keyPressed);
    int button = buttonReleased;
    int buttons = 0;
    int modifiers = keyPressed;

    QTest::mouseClick(testMouseWidget, Qt::MouseButton(buttonReleased), Qt::KeyboardModifiers(keyPressed));
    QVERIFY(testMouseWidget->mouseReleaseEventRecieved);
    QCOMPARE(testMouseWidget->mouseReleaseButton, button);
    QCOMPARE(testMouseWidget->mouseReleaseButtons, buttons);
    QCOMPARE(testMouseWidget->mouseReleaseModifiers, modifiers);
#ifdef QT3_SUPPORT
    int stateAfter = buttons|modifiers;
    int stateBefore = stateAfter|button;

    QCOMPARE(testMouseWidget->mouseReleaseStateBefore, stateBefore);
    QCOMPARE(testMouseWidget->mouseReleaseStateAfter, stateAfter);
#endif
}

void tst_QMouseEvent::qt3supportConstructors()
{
#if !defined(QT3_SUPPORT)
    QSKIP( "No Qt3Support present", SkipAll);
#else
    // make sure the state() and stateAfter() functions return the
    // same thing they did in Qt 3 when using these constructors

    {
        QMouseEvent e(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, 0);
        QCOMPARE(e.state(), Qt::ButtonState(Qt::NoButton));
        QCOMPARE(e.stateAfter(), Qt::ButtonState(Qt::LeftButton));
    }

    {
        QMouseEvent e(QEvent::MouseButtonPress, QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, 0);
        QCOMPARE(e.state(), Qt::ButtonState(Qt::NoButton));
        QCOMPARE(e.stateAfter(), Qt::ButtonState(Qt::LeftButton));
    }
#endif
}

QTEST_MAIN(tst_QMouseEvent)
#include "tst_qmouseevent.moc"
