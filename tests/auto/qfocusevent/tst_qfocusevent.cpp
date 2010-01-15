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

#if defined (QT3_SUPPORT)
#include <q3popupmenu.h>
#else
#include <qmenu.h>
#endif

#include <qlabel.h>
#include <qdialog.h>
#include <qevent.h>
#include <qlineedit.h>
#include <QBoxLayout>

#include "../../shared/util.h"

QT_FORWARD_DECLARE_CLASS(QWidget)

//TESTED_CLASS=
//TESTED_FILES=gui/kernel/qevent.h gui/kernel/qevent.cpp

class FocusLineEdit : public QLineEdit
{
public:
    FocusLineEdit( QWidget* parent = 0, const char* name = 0 ) : QLineEdit(name, parent) {}
    int focusInEventReason;
    int focusOutEventReason;
    bool focusInEventRecieved;
    bool focusInEventGotFocus;
    bool focusOutEventRecieved;
    bool focusOutEventLostFocus;
protected:
    virtual void keyPressEvent( QKeyEvent *e )
    {
//        qDebug( QString("keyPressEvent: %1").arg(e->key()) );
        QLineEdit::keyPressEvent( e );
    }
    void focusInEvent( QFocusEvent* e )
    {
        QLineEdit::focusInEvent( e );
        focusInEventReason = e->reason();
	    focusInEventGotFocus = e->gotFocus();
        focusInEventRecieved = TRUE;
    }
    void focusOutEvent( QFocusEvent* e )
    {
        QLineEdit::focusOutEvent( e );
        focusOutEventReason = e->reason();
	    focusOutEventLostFocus = !e->gotFocus();
        focusOutEventRecieved = TRUE;
    }
};

class tst_QFocusEvent : public QObject
{
    Q_OBJECT

public:
    tst_QFocusEvent();
    virtual ~tst_QFocusEvent();


    void initWidget();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void checkReason_Tab();
    void checkReason_ShiftTab();
    void checkReason_BackTab();
    void checkReason_Popup();
    void checkReason_focusWidget();
    void checkReason_Shortcut();
    void checkReason_ActiveWindow();

private:
    QWidget* testFocusWidget;
    FocusLineEdit* childFocusWidgetOne;
    FocusLineEdit* childFocusWidgetTwo;
};

tst_QFocusEvent::tst_QFocusEvent()
{
}

tst_QFocusEvent::~tst_QFocusEvent()
{

}

void tst_QFocusEvent::initTestCase()
{
    testFocusWidget = new QWidget( 0 );
    childFocusWidgetOne = new FocusLineEdit( testFocusWidget );
    childFocusWidgetOne->setGeometry( 10, 10, 180, 20 );
    childFocusWidgetTwo = new FocusLineEdit( testFocusWidget );
    childFocusWidgetTwo->setGeometry( 10, 50, 180, 20 );

    //qApp->setMainWidget( testFocusWidget ); Qt4
    testFocusWidget->resize( 200,100 );
    testFocusWidget->show();
// Applications don't get focus when launched from the command line on Mac.
#ifdef Q_WS_MAC
    testFocusWidget->raise();
#endif
}

void tst_QFocusEvent::cleanupTestCase()
{
    delete testFocusWidget;
}

void tst_QFocusEvent::init()
{
}

void tst_QFocusEvent::cleanup()
{
    childFocusWidgetTwo->setGeometry( 10, 50, 180, 20 );
}

void tst_QFocusEvent::initWidget()
{
    // On X11 we have to ensure the event was processed before doing any checking, on Windows
    // this is processed straight away.
    QApplication::setActiveWindow(childFocusWidgetOne);

    for (int i = 0; i < 1000; ++i) {
	if (childFocusWidgetOne->isActiveWindow() && childFocusWidgetOne->hasFocus())
	    break;
	childFocusWidgetOne->activateWindow();
	childFocusWidgetOne->setFocus();
	qApp->processEvents();
	QTest::qWait(100);
    }

    // The first lineedit should have focus
    QVERIFY( childFocusWidgetOne->hasFocus() );

    childFocusWidgetOne->focusInEventRecieved = FALSE;
    childFocusWidgetOne->focusInEventGotFocus = FALSE;
    childFocusWidgetOne->focusInEventReason = -1;
    childFocusWidgetOne->focusOutEventRecieved = FALSE;
    childFocusWidgetOne->focusOutEventLostFocus = FALSE;
    childFocusWidgetOne->focusOutEventReason = -1;
    childFocusWidgetTwo->focusInEventRecieved = FALSE;
    childFocusWidgetTwo->focusInEventGotFocus = FALSE;
    childFocusWidgetTwo->focusInEventReason = -1;
    childFocusWidgetTwo->focusOutEventRecieved = FALSE;
    childFocusWidgetTwo->focusOutEventLostFocus = FALSE;
    childFocusWidgetTwo->focusOutEventReason = -1;
}

void tst_QFocusEvent::checkReason_Tab()
{
    initWidget();

    // Now test the tab key
    QTest::keyClick( childFocusWidgetOne, Qt::Key_Tab );

    QVERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int) Qt::TabFocusReason );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, (int) Qt::TabFocusReason );
}

void tst_QFocusEvent::checkReason_ShiftTab()
{
    initWidget();

    // Now test the shift + tab key
    QTest::keyClick( childFocusWidgetOne, Qt::Key_Tab, Qt::ShiftModifier );

    QVERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int)Qt::BacktabFocusReason );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, (int)Qt::BacktabFocusReason );

}

/*!
    In this test we verify that the Qt::KeyBacktab key is handled in a qfocusevent
*/
void tst_QFocusEvent::checkReason_BackTab()
{
#ifdef Q_OS_WIN32 // key is not supported on Windows
    QSKIP( "Backtab is not supported on Windows", SkipAll );
#else
    initWidget();
    QVERIFY( childFocusWidgetOne->hasFocus() );

    // Now test the backtab key
    QTest::keyClick( childFocusWidgetOne, Qt::Key_Backtab );
    QTest::qWait(200);

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, int(Qt::BacktabFocusReason) );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, int(Qt::BacktabFocusReason) );
#endif
}

void tst_QFocusEvent::checkReason_Popup()
{
    initWidget();

    // Now test the popup reason
#if defined (QT3_SUPPORT)
    Q3PopupMenu* popupMenu = new Q3PopupMenu( testFocusWidget );
    popupMenu->insertItem( "Test" );
    popupMenu->popup( QPoint(0,0) );
    QTest::qWait(50);

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventLostFocus);

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusInEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, int(Qt::PopupFocusReason));

    popupMenu->hide();

    QVERIFY(childFocusWidgetOne->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusInEventGotFocus);

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusInEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );
#else
    QMenu* popupMenu = new QMenu( testFocusWidget );
    popupMenu->addMenu( "Test" );
    popupMenu->popup( QPoint(0,0) );
    QTest::qWait(50);

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventLostFocus);

    QTRY_VERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusInEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, int(Qt::PopupFocusReason));

    popupMenu->hide();

    QVERIFY(childFocusWidgetOne->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusInEventGotFocus);

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusInEventRecieved );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );
#endif
}

#ifdef Q_WS_MAC
QT_BEGIN_NAMESPACE
    extern void qt_set_sequence_auto_mnemonic(bool);
QT_END_NAMESPACE
#endif

void tst_QFocusEvent::checkReason_Shortcut()
{
    initWidget();
#ifdef Q_WS_MAC
    qt_set_sequence_auto_mnemonic(true);
#endif
    QLabel* label = new QLabel( "&Test", testFocusWidget );
    label->setBuddy( childFocusWidgetTwo );
    label->setGeometry( 10, 50, 90, 20 );
    childFocusWidgetTwo->setGeometry( 105, 50, 95, 20 );
    label->show();

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( !childFocusWidgetTwo->hasFocus() );

    QTest::keyClick( label, Qt::Key_T, Qt::AltModifier );

    QVERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetTwo->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);
    QVERIFY(childFocusWidgetTwo->focusInEventGotFocus);

    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int)Qt::ShortcutFocusReason );
    QVERIFY( childFocusWidgetTwo->focusInEventRecieved );
    QCOMPARE( childFocusWidgetTwo->focusInEventReason, (int)Qt::ShortcutFocusReason );
    QVERIFY( !childFocusWidgetTwo->focusOutEventRecieved );

    label->hide();
    QVERIFY( childFocusWidgetTwo->hasFocus() );
    QVERIFY( !childFocusWidgetOne->hasFocus() );
#ifdef Q_WS_MAC
    qt_set_sequence_auto_mnemonic(false);
#endif
}

void tst_QFocusEvent::checkReason_focusWidget()
{
    // This test checks that a widget doesn't loose
    // its focuswidget just because the focuswidget looses focus.
    QWidget window1;
    QWidget frame1;
    QWidget frame2;
    QLineEdit edit1;
    QLineEdit edit2;

    QVBoxLayout outerLayout;
    outerLayout.addWidget(&frame1);
    outerLayout.addWidget(&frame2);
    window1.setLayout(&outerLayout);

    QVBoxLayout leftLayout;
    QVBoxLayout rightLayout;
    leftLayout.addWidget(&edit1);
    rightLayout.addWidget(&edit2);
    frame1.setLayout(&leftLayout);
    frame2.setLayout(&rightLayout);
    window1.show();

    edit1.setFocus();
    QTest::qWait(100);
    edit2.setFocus();

    QVERIFY(frame1.focusWidget() != 0);
    QVERIFY(frame2.focusWidget() != 0);
}

void tst_QFocusEvent::checkReason_ActiveWindow()
{
    initWidget();

    QDialog* d = new QDialog( testFocusWidget );
    d->show();
    d->activateWindow(); // ### CDE
    QApplication::setActiveWindow(d);
    QTest::qWaitForWindowShown(d);

    QTRY_VERIFY(childFocusWidgetOne->focusOutEventRecieved);
    QVERIFY(childFocusWidgetOne->focusOutEventLostFocus);

    QVERIFY( !childFocusWidgetOne->focusInEventRecieved );
    QVERIFY( childFocusWidgetOne->focusOutEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusOutEventReason, (int)Qt::ActiveWindowFocusReason);
    QVERIFY( !childFocusWidgetOne->hasFocus() );

    d->hide();
    QTest::qWait(100);

#if defined(Q_OS_IRIX)
    QEXPECT_FAIL("", "IRIX requires explicit activateWindow(), so this test does not make any sense.", Abort);
#endif
    QTRY_VERIFY(childFocusWidgetOne->focusInEventRecieved);
    QVERIFY(childFocusWidgetOne->focusInEventGotFocus);

    QVERIFY( childFocusWidgetOne->hasFocus() );
    QVERIFY( childFocusWidgetOne->focusInEventRecieved );
    QCOMPARE( childFocusWidgetOne->focusInEventReason, (int)Qt::ActiveWindowFocusReason);
}


QTEST_MAIN(tst_QFocusEvent)
#include "tst_qfocusevent.moc"
