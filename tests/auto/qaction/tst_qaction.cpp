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
#include <qevent.h>
#include <qaction.h>
#include <qmenu.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QAction : public QObject
{
    Q_OBJECT

public:
    tst_QAction();
    virtual ~tst_QAction();


    void updateState(QActionEvent *e);

public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void getSetCheck();
    void setText_data();
    void setText();
    void setIconText_data() { setText_data(); }
    void setIconText();
    void actionEvent();
    void setStandardKeys();
    void alternateShortcuts();
    void enabledVisibleInteraction();
    void task200823_tooltip();
    void task229128TriggeredSignalWithoutActiongroup();
    void task229128TriggeredSignalWhenInActiongroup();

private:
    int m_lastEventType;
    QAction *m_lastAction;
    QWidget *m_tstWidget;
};

// Testing get/set functions
void tst_QAction::getSetCheck()
{
    QAction obj1(0);
    // QActionGroup * QAction::actionGroup()
    // void QAction::setActionGroup(QActionGroup *)
    QActionGroup *var1 = new QActionGroup(0);
    obj1.setActionGroup(var1);
    QCOMPARE(var1, obj1.actionGroup());
    obj1.setActionGroup((QActionGroup *)0);
    QCOMPARE((QActionGroup *)0, obj1.actionGroup());
    delete var1;

    // QMenu * QAction::menu()
    // void QAction::setMenu(QMenu *)
    QMenu *var2 = new QMenu(0);
    obj1.setMenu(var2);
    QCOMPARE(var2, obj1.menu());
    obj1.setMenu((QMenu *)0);
    QCOMPARE((QMenu *)0, obj1.menu());
    delete var2;

    QCOMPARE(obj1.priority(), QAction::NormalPriority);
    obj1.setPriority(QAction::LowPriority);
    QCOMPARE(obj1.priority(), QAction::LowPriority);
}

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    MyWidget(tst_QAction *tst, QWidget *parent = 0) : QWidget(parent) { this->tst = tst; }

protected:
    virtual void actionEvent(QActionEvent *e) { tst->updateState(e); }

private:
    tst_QAction *tst;
};

tst_QAction::tst_QAction()
{
}

tst_QAction::~tst_QAction()
{

}

void tst_QAction::initTestCase()
{
    m_lastEventType = 0;
    m_lastAction = 0;

    MyWidget *mw = new MyWidget(this);
    m_tstWidget = mw;
    mw->show();
    qApp->setActiveWindow(mw);
}

void tst_QAction::cleanupTestCase()
{
    QWidget *testWidget = m_tstWidget;
    if (testWidget) {
        testWidget->hide();
        delete testWidget;
    }
}

void tst_QAction::setText_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("iconText");
    QTest::addColumn<QString>("textFromIconText");

    //next we fill it with data
    QTest::newRow("Normal") << "Action" << "Action" << "Action";
    QTest::newRow("Ampersand") << "Search && Destroy" << "Search & Destroy" << "Search && Destroy";
    QTest::newRow("Mnemonic and ellipsis") << "O&pen File ..." << "Open File" << "Open File";
}

void tst_QAction::setText()
{
    QFETCH(QString, text);

    QAction action(0);
    action.setText(text);

    QCOMPARE(action.text(), text);

    QFETCH(QString, iconText);
    QCOMPARE(action.iconText(), iconText);
}

void tst_QAction::setIconText()
{
    QFETCH(QString, iconText);

    QAction action(0);
    action.setIconText(iconText);
    QCOMPARE(action.iconText(), iconText);

    QFETCH(QString, textFromIconText);
    QCOMPARE(action.text(), textFromIconText);
}


void tst_QAction::updateState(QActionEvent *e)
{
    if (!e) {
        m_lastEventType = 0;
        m_lastAction = 0;
    } else {
        m_lastEventType = (int)e->type();
        m_lastAction = e->action();
    }
}

void tst_QAction::actionEvent()
{
    QAction a(0);
    a.setText("action text");

    // add action
    m_tstWidget->addAction(&a);
    qApp->processEvents();

    QCOMPARE(m_lastEventType, (int)QEvent::ActionAdded);
    QCOMPARE(m_lastAction, &a);

    // change action
    a.setText("new action text");
    qApp->processEvents();

    QCOMPARE(m_lastEventType, (int)QEvent::ActionChanged);
    QCOMPARE(m_lastAction, &a);

    // remove action
    m_tstWidget->removeAction(&a);
    qApp->processEvents();

    QCOMPARE(m_lastEventType, (int)QEvent::ActionRemoved);
    QCOMPARE(m_lastAction, &a);
}

//basic testing of standard keys
void tst_QAction::setStandardKeys()
{
    QAction act(0);
    act.setShortcut(QKeySequence("CTRL+L"));
    QList<QKeySequence> list;
    act.setShortcuts(list);
    act.setShortcuts(QKeySequence::Copy);
    QVERIFY(act.shortcut() == act.shortcuts().first());

    QList<QKeySequence> expected;
#if defined(Q_WS_MAC) || defined(Q_OS_SYMBIAN)
    expected  << QKeySequence("CTRL+C");
#elif defined(Q_WS_WIN) || defined(Q_WS_QWS) || defined(Q_WS_QPA)
    expected  << QKeySequence("CTRL+C") << QKeySequence("CTRL+INSERT");
#else
    expected  << QKeySequence("CTRL+C") << QKeySequence("F16") << QKeySequence("CTRL+INSERT");
#endif
    QVERIFY(act.shortcuts() == expected);
}


void tst_QAction::alternateShortcuts()
{
    //test the alternate shortcuts (by adding more than 1 shortcut)

    QWidget *wid = m_tstWidget;

    {
        QAction act(wid);
        wid->addAction(&act);
        QList<QKeySequence> shlist = QList<QKeySequence>() << QKeySequence("CTRL+P") << QKeySequence("CTRL+A");
        act.setShortcuts(shlist);

        QSignalSpy spy(&act, SIGNAL(triggered()));

        act.setAutoRepeat(true);
        QTest::keyClick(wid, Qt::Key_A, Qt::ControlModifier);
        QCOMPARE(spy.count(), 1); //act should have been triggered

        act.setAutoRepeat(false);
        QTest::keyClick(wid, Qt::Key_A, Qt::ControlModifier);
        QCOMPARE(spy.count(), 2); //act should have been triggered a 2nd time

        //end of the scope of the action, it will be destroyed and removed from wid
        //This action should also unregister its shortcuts
    }


    //this tests a crash (if the action did not unregister its alternate shortcuts)
    QTest::keyClick(wid, Qt::Key_A, Qt::ControlModifier);
}

void tst_QAction::enabledVisibleInteraction()
{
    QAction act(0);
    // check defaults
    QVERIFY(act.isEnabled());
    QVERIFY(act.isVisible());

    // !visible => !enabled
    act.setVisible(false);
    QVERIFY(!act.isEnabled());
    act.setVisible(true);
    QVERIFY(act.isEnabled());
    act.setEnabled(false);
    QVERIFY(act.isVisible());

    // check if shortcut is disabled if not visible
    m_tstWidget->addAction(&act);
    act.setShortcut(QKeySequence("Ctrl+T"));
    QSignalSpy spy(&act, SIGNAL(triggered()));
    act.setEnabled(true);
    act.setVisible(false);
    QTest::keyClick(m_tstWidget, Qt::Key_T, Qt::ControlModifier);
    QCOMPARE(spy.count(), 0); //act is not visible, so don't trigger
    act.setVisible(false);
    act.setEnabled(true);
    QTest::keyClick(m_tstWidget, Qt::Key_T, Qt::ControlModifier);
    QCOMPARE(spy.count(), 0); //act is not visible, so don't trigger
    act.setVisible(true);
    act.setEnabled(true);
    QTest::keyClick(m_tstWidget, Qt::Key_T, Qt::ControlModifier);
    QCOMPARE(spy.count(), 1); //act is visible and enabled, so trigger
}

void tst_QAction::task200823_tooltip()
{
    QAction *action = new QAction("foo", 0);
    QString shortcut("ctrl+o");
    action->setShortcut(shortcut);

    // we want a non-standard tooltip that shows the shortcut
    action->setToolTip(QString("%1 (%2)").arg(action->text()).arg(action->shortcut().toString()));

    QString ref = QString("foo (%1)").arg(QKeySequence(shortcut).toString());
    QCOMPARE(action->toolTip(), ref);
}

void tst_QAction::task229128TriggeredSignalWithoutActiongroup()
{
    // test without a group
    QAction *actionWithoutGroup = new QAction("Test", qApp);
    QSignalSpy spyWithoutGroup(actionWithoutGroup, SIGNAL(triggered(bool)));
    QCOMPARE(spyWithoutGroup.count(), 0);
    actionWithoutGroup->trigger();
    // signal should be emitted
    QCOMPARE(spyWithoutGroup.count(), 1);

    // it is now a checkable checked action
    actionWithoutGroup->setCheckable(true);
    actionWithoutGroup->setChecked(true);
    spyWithoutGroup.clear();
    QCOMPARE(spyWithoutGroup.count(), 0);
    actionWithoutGroup->trigger();
    // signal should be emitted
    QCOMPARE(spyWithoutGroup.count(), 1);
}

void tst_QAction::task229128TriggeredSignalWhenInActiongroup()
{
    QActionGroup ag(0);
    QAction *action = new QAction("Test", &ag);
    QAction *checkedAction = new QAction("Test 2", &ag);
    ag.addAction(action);
    action->setCheckable(true);
    ag.addAction(checkedAction);
    checkedAction->setCheckable(true);
    checkedAction->setChecked(true);

    QSignalSpy actionSpy(checkedAction, SIGNAL(triggered(bool)));
    QSignalSpy actionGroupSpy(&ag, SIGNAL(triggered(QAction *)));
    QCOMPARE(actionGroupSpy.count(), 0);
    QCOMPARE(actionSpy.count(), 0);
    checkedAction->trigger();
    // check that both the group and the action have emitted the signal
    QCOMPARE(actionGroupSpy.count(), 1);
    QCOMPARE(actionSpy.count(), 1);
}

QTEST_MAIN(tst_QAction)
#include "tst_qaction.moc"
