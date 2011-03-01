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



#include <qapplication.h>
#include <qpainter.h>
#include <qstyleoption.h>
#include <qkeysequence.h>
#include <qevent.h>
#include <qgridlayout.h>



#include <qabstractbutton.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QAbstractButton : public QObject
{
    Q_OBJECT

public:
    tst_QAbstractButton();
    virtual ~tst_QAbstractButton();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void setAutoRepeat_data();
    void setAutoRepeat();

    void pressed();
    void released();
    void text();
    void setText();
    void icon();
    void setIcon();

    void shortcut();
    void setShortcut();

    void animateClick();

    void isCheckable();
    void isDown();
    void setDown();
    void isChecked();
    void autoRepeat();
    void toggle();
    void clicked();
    void toggled();
    void isEnabled();
    void setEnabled();
/*
    void state();
    void group();
    void stateChanged();
*/

    void shortcutEvents();
    void stopRepeatTimer();

    void keyNavigation();

protected slots:
    void onClicked();
    void onToggled( bool on );
    void onPressed();
    void onReleased();
    void resetValues();

private:
    uint click_count;
    uint toggle_count;
    uint press_count;
    uint release_count;

    QAbstractButton *testWidget;
};

// QAbstractButton is an abstract class in 4.0
class MyButton : public QAbstractButton
{
public:
    MyButton(QWidget *p = 0) : QAbstractButton(p) {}
    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);
        QRect r = rect();
        p.fillRect(r, isDown() ? Qt::black : (isChecked() ? Qt::lightGray : Qt::white));
        p.setPen(isDown() ? Qt::white : Qt::black);
        p.drawRect(r);
        p.drawText(r, Qt::AlignCenter | Qt::TextShowMnemonic, text());
        if (hasFocus()) {
            r.adjust(2, 2, -2, -2);
            QStyleOptionFocusRect opt;
            opt.rect = r;
            opt.palette = palette();
            opt.state = QStyle::State_None;
            style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, &p, this);
#ifdef Q_WS_MAC
            p.setPen(Qt::red);
            p.drawRect(r);
#endif
        }
    }
    QSize sizeHint() const
    {
        QSize sh(8, 8);
        if (!text().isEmpty())
            sh += fontMetrics().boundingRect(text()).size();
        return sh;
    }

    void resetTimerEvents() { timerEvents = 0; }
    int timerEventCount() const { return timerEvents; }

private:

    int timerEvents;

    void timerEvent(QTimerEvent *event)
    {
        ++timerEvents;
        QAbstractButton::timerEvent(event);
    }
};

tst_QAbstractButton::tst_QAbstractButton()
{
}

tst_QAbstractButton::~tst_QAbstractButton()
{
}

void tst_QAbstractButton::initTestCase()
{
    testWidget = new MyButton(0);
    testWidget->setObjectName("testObject");
    testWidget->resize( 200, 200 );
    testWidget->show();

    connect( testWidget, SIGNAL(clicked()), this, SLOT(onClicked()) );
    connect( testWidget, SIGNAL(pressed()), this, SLOT(onPressed()) );
    connect( testWidget, SIGNAL(released()), this, SLOT(onReleased()) );
    connect( testWidget, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)) );
}

void tst_QAbstractButton::cleanupTestCase()
{
    delete testWidget;
}

void tst_QAbstractButton::init()
{
    testWidget->setText("Test");
    testWidget->setEnabled( TRUE );
    testWidget->setDown( FALSE );
    testWidget->setAutoRepeat( FALSE );
    QKeySequence seq;
    testWidget->setShortcut( seq );

    toggle_count = 0;
    press_count = 0;
    release_count = 0;
    click_count = 0;
}

void tst_QAbstractButton::cleanup()
{
}

void tst_QAbstractButton::resetValues()
{
    toggle_count = 0;
    press_count = 0;
    release_count = 0;
    click_count = 0;
}

void tst_QAbstractButton::onClicked()
{
    click_count++;
}

void tst_QAbstractButton::onToggled( bool /*on*/ )
{
    toggle_count++;
}

void tst_QAbstractButton::onPressed()
{
    press_count++;
}

void tst_QAbstractButton::onReleased()
{
    release_count++;
}

void tst_QAbstractButton::autoRepeat()
{
    DEPENDS_ON(" setAutoRepeat" );
}

void tst_QAbstractButton::setAutoRepeat_data()
{
    QTest::addColumn<int>("mode");
    QTest::newRow( "" ) << 0;
    QTest::newRow( "" ) << 1;
    QTest::newRow( "" ) << 2;
    QTest::newRow( "" ) << 3;
    QTest::newRow( "" ) << 4;
    QTest::newRow( "" ) << 5;
    QTest::newRow( "" ) << 6;
}

#define REPEAT_DELAY 1000

int test_count = 0;
int last_mode = 0;

void tst_QAbstractButton::setAutoRepeat()
{
    QFETCH( int, mode );

    //FIXME: temp code to check that the test fails consistenly
    //retest( 3 );

    switch (mode)
    {
    case 0:
        QVERIFY( !testWidget->isCheckable() );
        break;
    case 1:
        // check if we can toggle the mode
        testWidget->setAutoRepeat( TRUE );
        QVERIFY( testWidget->autoRepeat() );

        testWidget->setAutoRepeat( FALSE );
        QVERIFY( !testWidget->autoRepeat() );
        break;
    case 2:
        // check that the button is down if we press space and not in autorepeat
        testWidget->setDown( FALSE );
        testWidget->setAutoRepeat( FALSE );
        QTest::keyPress( testWidget, Qt::Key_Space );

        QTest::qWait( REPEAT_DELAY );

        QVERIFY( release_count == 0 );
        QVERIFY( testWidget->isDown() );
        QVERIFY( toggle_count == 0 );
        QVERIFY( press_count == 1 );
        QVERIFY( click_count == 0 );

        QTest::keyRelease( testWidget, Qt::Key_Space );
        QVERIFY( click_count == 1 );
        QVERIFY( release_count == 1 );
        break;
    case 3:
        // check that the button is down if we press space while in autorepeat
        testWidget->setDown(false);
        testWidget->setAutoRepeat(true);
        QTest::keyPress(testWidget, Qt::Key_Space);
        QTest::qWait(REPEAT_DELAY);
        QVERIFY(testWidget->isDown());
        QTest::keyRelease(testWidget, Qt::Key_Space);
        QVERIFY(release_count == press_count);
        QVERIFY(toggle_count == 0);
        QVERIFY(press_count == click_count);
        QVERIFY(click_count > 1);
        break;
    case 4:
        // check that pressing ENTER has no effect when autorepeat is FALSE
        testWidget->setDown( FALSE );
        testWidget->setAutoRepeat( FALSE );
        QTest::keyPress( testWidget, Qt::Key_Enter );

        QTest::qWait( REPEAT_DELAY );

        QVERIFY( !testWidget->isDown() );
        QVERIFY( toggle_count == 0 );
        QVERIFY( press_count == 0 );
        QVERIFY( release_count == 0 );
        QVERIFY( click_count == 0 );
        QTest::keyRelease( testWidget, Qt::Key_Enter );

        QVERIFY( click_count == 0 );
        break;
    case 5:
        // check that pressing ENTER has no effect when autorepeat is TRUE
        testWidget->setDown( FALSE );
        testWidget->setAutoRepeat( TRUE );
        QTest::keyPress( testWidget, Qt::Key_Enter );

        QTest::qWait( REPEAT_DELAY );

        QVERIFY( !testWidget->isDown() );
        QVERIFY( toggle_count == 0 );
        QVERIFY( press_count == 0 );
        QVERIFY( release_count == 0 );
        QVERIFY( click_count == 0 );

        QTest::keyRelease( testWidget, Qt::Key_Enter );

        QVERIFY( click_count == 0 );
        break;
    case 6:
        // verify autorepeat is off by default.
        MyButton tmp( 0);
        tmp.setObjectName("tmp" );
        QVERIFY( !tmp.autoRepeat() );
        break;
    }
}

void tst_QAbstractButton::pressed()
{
    // pressed/released signals expected for a QAbstractButton
    QTest::keyPress( testWidget, ' ' );
    QCOMPARE( press_count, (uint)1 );
}

void tst_QAbstractButton::released()
{
    // pressed/released signals expected for a QAbstractButton
    QTest::keyPress( testWidget, ' ' );
    QTest::keyRelease( testWidget, ' ' );
    QCOMPARE( release_count, (uint)1 );
}

void tst_QAbstractButton::setText()
{
    testWidget->setText("");
    QCOMPARE( testWidget->text(), QString("") );
    testWidget->setText("simple");
    QCOMPARE( testWidget->text(), QString("simple") );
    testWidget->setText("&ampersand");
    QCOMPARE( testWidget->text(), QString("&ampersand") );
#ifndef Q_WS_MAC // no mneonics on Mac.
    QCOMPARE( testWidget->shortcut(), QKeySequence("ALT+A"));
#endif
    testWidget->setText("te&st");
    QCOMPARE( testWidget->text(), QString("te&st") );
#ifndef Q_WS_MAC // no mneonics on Mac.
    QCOMPARE( testWidget->shortcut(), QKeySequence("ALT+S"));
#endif
    testWidget->setText("foo");
    QCOMPARE( testWidget->text(), QString("foo") );
#ifndef Q_WS_MAC // no mneonics on Mac.
    QCOMPARE( testWidget->shortcut(), QKeySequence());
#endif
}

void tst_QAbstractButton::text()
{
    DEPENDS_ON( "setText" );
}

void tst_QAbstractButton::setIcon()
{
    const char *test1_xpm[] = {
    "12 8 2 1",
    ". c None",
    "c c #ff0000",
    ".........ccc",
    "........ccc.",
    ".......ccc..",
    "ccc...ccc...",
    ".ccc.ccc....",
    "..ccccc.....",
    "...ccc......",
    "....c.......",
    };

    QPixmap p(test1_xpm);
    testWidget->setIcon( p );
    QCOMPARE( testWidget->icon().pixmap(12, 8), p );

    // Test for #14793

    const char *test2_xpm[] = {
    "12 8 2 1",
    ". c None",
    "c c #ff0000",
    "ccc......ccc",
    ".ccc....ccc.",
    "..ccc..ccc..",
    "....cc.cc...",
    ".....ccc....",
    "....cc.cc...",
    "...ccc.ccc..",
    "..ccc...ccc.",
    };

    int currentHeight = testWidget->height();
    int currentWidth = testWidget->width();

    QPixmap p2( test2_xpm );
    for ( int a = 0; a<5; a++ )
	testWidget->setIcon( p2 );

    QCOMPARE( testWidget->icon().pixmap(12, 8), p2 );

    QCOMPARE( testWidget->height(), currentHeight );
    QCOMPARE( testWidget->width(), currentWidth );
}

void tst_QAbstractButton::icon()
{
    DEPENDS_ON( "setIcon" );
}

void tst_QAbstractButton::isEnabled()
{
    DEPENDS_ON( "setEnabled" );
}

void tst_QAbstractButton::setEnabled()
{
    testWidget->setEnabled( FALSE );
    QVERIFY( !testWidget->isEnabled() );
//    QTEST( testWidget, "disabled" );

    testWidget->setEnabled( TRUE );
    QVERIFY( testWidget->isEnabled() );
//    QTEST( testWidget, "enabled" );
}

void tst_QAbstractButton::isCheckable()
{
    QVERIFY( !testWidget->isCheckable() );
}


void tst_QAbstractButton::isDown()
{
    DEPENDS_ON( "setDown" );
}

void tst_QAbstractButton::setDown()
{
    testWidget->setDown( FALSE );
    QVERIFY( !testWidget->isDown() );

    testWidget->setDown( TRUE );
    QTest::qWait(300);
    QVERIFY( testWidget->isDown() );

    testWidget->setDown( TRUE );

    // add some debugging stuff
    QWidget *grab = QWidget::keyboardGrabber();
    if (grab != 0 && grab != testWidget)
        qDebug( "testWidget != keyboardGrabber" );
    grab = qApp->focusWidget();
    if (grab != 0 && grab != testWidget)
        qDebug( "testWidget != focusWidget" );

    QTest::keyClick( testWidget, Qt::Key_Escape );
    QVERIFY( !testWidget->isDown() );
}

void tst_QAbstractButton::isChecked()
{
    testWidget->setDown( FALSE );
    QVERIFY( !testWidget->isChecked() );

    testWidget->setDown( TRUE );
    QVERIFY( !testWidget->isChecked() );

    testWidget->setDown( FALSE );
    testWidget->toggle();
    QVERIFY( testWidget->isChecked() == testWidget->isCheckable() );
}

void tst_QAbstractButton::toggle()
{
    DEPENDS_ON( "toggled" );
}

void tst_QAbstractButton::toggled()
{
    testWidget->toggle();
    QVERIFY( toggle_count == 0 );

    QTest::mousePress( testWidget, Qt::LeftButton );
    QVERIFY( toggle_count == 0 );
    QVERIFY( click_count == 0 );

    QTest::mouseRelease( testWidget, Qt::LeftButton );
    QVERIFY( click_count == 1 );
}

void tst_QAbstractButton::shortcut()
{
    DEPENDS_ON( "setShortcut" );
}

void tst_QAbstractButton::setShortcut()
{
    QKeySequence seq( Qt::Key_A );
    testWidget->setShortcut( seq );
    QApplication::setActiveWindow(testWidget);

    // must be active to get shortcuts
    for (int i = 0; !testWidget->isActiveWindow() && i < 100; ++i) {
       testWidget->activateWindow();
       QApplication::instance()->processEvents();
       QTest::qWait(100);
    }
    QVERIFY(testWidget->isActiveWindow());

    QTest::keyClick( testWidget, 'A' );
    QTest::qWait(300);                      // Animate click takes time
    QCOMPARE(click_count,  (uint)1);
    QCOMPARE(press_count,  (uint)1); // Press is part of a click
    QCOMPARE(release_count,(uint)1); // Release is part of a click

    QVERIFY( toggle_count == 0 );

//     resetValues();
//     QTest::keyPress( testWidget, 'A' );
//     QTest::qWait(10000);
//     QTest::keyRelease( testWidget, 'A' );
//     QCOMPARE(click_count,  (uint)1);
//     QCOMPARE(press_count,  (uint)1);
//     QCOMPARE(release_count,(uint)1);

//     qDebug() << click_count;

}

void tst_QAbstractButton::animateClick()
{
    testWidget->animateClick();
    QVERIFY( testWidget->isDown() );
    qApp->processEvents();
    QVERIFY( testWidget->isDown() );
    QTest::qWait(200);
    qApp->processEvents();
    QVERIFY( !testWidget->isDown() );
}

void tst_QAbstractButton::clicked()
{
    DEPENDS_ON( "toggled" );
}

/*
void tst_QAbstractButton::group()
{
}

void tst_QAbstractButton::state()
{
}

void tst_QAbstractButton::stateChanged()
{
}
*/

void tst_QAbstractButton::shortcutEvents()
{
    MyButton button;
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(bool)));

    for (int i = 0; i < 4; ++i) {
        QKeySequence sequence;
        QShortcutEvent event(sequence, false);
        QApplication::sendEvent(&button, &event);
        if (i < 2)
            QTest::qWait(500);
    }

    QTest::qWait(1000); // ensure animate timer is expired

    QCOMPARE(pressedSpy.count(), 3);
    QCOMPARE(releasedSpy.count(), 3);
    QCOMPARE(clickedSpy.count(), 3);
}

void tst_QAbstractButton::stopRepeatTimer()
{
    MyButton button;
    button.setAutoRepeat(true);

    // Mouse trigger case:
    button.resetTimerEvents();
    QTest::mousePress(&button, Qt::LeftButton);
    QTest::qWait(1000);
    QVERIFY(button.timerEventCount() > 0);

    QTest::mouseRelease(&button, Qt::LeftButton);
    button.resetTimerEvents();
    QTest::qWait(1000);
    QCOMPARE(button.timerEventCount(), 0);

    // Key trigger case:
    button.resetTimerEvents();
    QTest::keyPress(&button, Qt::Key_Space);
    QTest::qWait(1000);
    QVERIFY(button.timerEventCount() > 0);

    QTest::keyRelease(&button, Qt::Key_Space);
    button.resetTimerEvents();
    QTest::qWait(1000);
    QCOMPARE(button.timerEventCount(), 0);
}

void tst_QAbstractButton::keyNavigation()
{
    QSKIP("Key navigation in QAbstractButton will be fixed/improved as part of task 194373", SkipSingle);

    QWidget widget;
    QGridLayout *layout = new QGridLayout(&widget);
    QAbstractButton *buttons[3][3];
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            buttons[y][x] = new MyButton(&widget);
            buttons[y][x]->setFocusPolicy(Qt::StrongFocus);
            layout->addWidget(buttons[y][x], y, x);
        }
    }

    widget.show();
    qApp->setActiveWindow(&widget);
    widget.activateWindow();
    QTest::qWait(30);

    buttons[1][1]->setFocus();
    QTest::qWait(400);
    QVERIFY(buttons[1][1]->hasFocus());
    QTest::keyPress(buttons[1][1], Qt::Key_Up);
    QTest::qWait(100);
    QVERIFY(buttons[0][1]->hasFocus());
    QTest::keyPress(buttons[0][1], Qt::Key_Down);
    QTest::qWait(100);
    QVERIFY(buttons[1][1]->hasFocus());
    QTest::keyPress(buttons[1][1], Qt::Key_Left);
    QTest::qWait(100);
    QVERIFY(buttons[1][0]->hasFocus());
    QTest::keyPress(buttons[1][0], Qt::Key_Down);
    QTest::qWait(100);
    QVERIFY(buttons[2][0]->hasFocus());
    QTest::keyPress(buttons[2][0], Qt::Key_Right);
    QTest::qWait(100);
    QVERIFY(buttons[2][1]->hasFocus());
    QTest::keyPress(buttons[2][1], Qt::Key_Right);
    QTest::qWait(100);
    QVERIFY(buttons[2][2]->hasFocus());
    QTest::keyPress(buttons[2][2], Qt::Key_Up);
    QTest::qWait(100);
    QVERIFY(buttons[1][2]->hasFocus());
    QTest::keyPress(buttons[1][2], Qt::Key_Up);
    QTest::qWait(100);
    QVERIFY(buttons[0][2]->hasFocus());
    buttons[0][1]->hide();
    QTest::keyPress(buttons[0][2], Qt::Key_Left);
    QTest::qWait(100);
    QVERIFY(buttons[0][0]->hasFocus());


}

QTEST_MAIN(tst_QAbstractButton)
#include "tst_qabstractbutton.moc"
