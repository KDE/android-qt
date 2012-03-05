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
#include <QMessageBox>
#include <QDebug>
#include <QPair>
#include <QList>
#include <QPointer>
#include <QTimer>
#include <QApplication>
#include <QPushButton>
#include <QDialogButtonBox>
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
#include <QMacStyle>
#endif
#if !defined(QT_NO_STYLE_CLEANLOOKS)
#include <QCleanlooksStyle>
#endif

#include "../../shared/util.h"

//TESTED_CLASS=
//TESTED_FILES=

#define CONVENIENCE_FUNC_SYMS(func) \
    { \
        int x1 = QMessageBox::func(0, "Foo", "Bar"); \
        int x3 = QMessageBox::func(0, "Foo", "Bar", "Save"); \
        int x6 = QMessageBox::func(0, "Foo", "Bar", "Save", "Save As"); \
        int x7 = QMessageBox::func(0, "Foo", "Bar", "Save", "Save As", "Dont Save"); \
        int x8 = QMessageBox::func(0, "Foo", "Bar", "Save", "Save As", "Dont Save", 1); \
        int x9 = QMessageBox::func(0, "Foo", "Bar", "Save", "Save As", "Dont Save", 1, 2); \
        int x10 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::YesAll, QMessageBox::Yes); \
        int x11 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::YesAll, QMessageBox::Yes, \
                                    QMessageBox::No); \
        qDebug("%d %d %d %d %d %d %d %d", x1, x3, x6, x7, x8, x9, x10, x11); \
        { \
        int x4 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes, (int)QMessageBox::No); \
        int x5 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes, (int)QMessageBox::No); \
        int x6 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default, (int)QMessageBox::No); \
        int x7 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes, QMessageBox::No); \
        int x8 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes, QMessageBox::No); \
        int x9 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No); \
        int x10 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes, (int)QMessageBox::No, (int)QMessageBox::Ok); \
        int x11 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes, (int)QMessageBox::No, (int)QMessageBox::Ok); \
        int x12 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default, (int)QMessageBox::No, (int)QMessageBox::Ok); \
        int x13 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes, QMessageBox::No, (int)QMessageBox::Ok); \
        int x14 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes, QMessageBox::No, (int)QMessageBox::Ok); \
        int x15 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, (int)QMessageBox::Ok); \
        int x16 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes, (int)QMessageBox::No, QMessageBox::Ok); \
        int x17 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes, (int)QMessageBox::No, QMessageBox::Ok); \
        int x18 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default, (int)QMessageBox::No, QMessageBox::Ok); \
        int x19 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes, QMessageBox::No, QMessageBox::Ok); \
        int x20 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes, QMessageBox::No, QMessageBox::Ok); \
        int x21 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, QMessageBox::Ok); \
        qDebug("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21); \
        } \
    }

#define CONVENIENCE_FUNC_SYMS_EXTRA(func) \
    { \
        int x1 = QMessageBox::func(0, "Foo", "Bar", (int)QMessageBox::Yes); \
        int x2 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes); \
        int x3 = QMessageBox::func(0, "Foo", "Bar", QMessageBox::Yes | QMessageBox::Default); \
        qDebug("%d %d %d", x1, x2, x3); \
    }

class tst_QMessageBox : public QObject
{
    Q_OBJECT
public:
    tst_QMessageBox();
    int exec(QMessageBox *msgBox, int key = -1);
    void sendKeySoon();

public slots:
    void sendKey();

private slots:
    void sanityTest();
    void defaultButton();
    void escapeButton();
    void button();
    void statics();
    void about();
    void detailsText();
    void detailsButtonText();

    void shortcut();

    void staticSourceCompat();
    void staticBinaryCompat();
    void instanceSourceCompat();
    void instanceBinaryCompat();

    void testSymbols();
    void incorrectDefaultButton();
    void updateSize();

    void setInformativeText();
    void iconPixmap();

    void init();
    void initTestCase();

private:
    int keyToSend;
    QTimer keySendTimer;
};

tst_QMessageBox::tst_QMessageBox() : keyToSend(-1)
{
    int argc = qApp->argc();
    QT_REQUIRE_VERSION(argc, qApp->argv(), "4.6.2")
}

int tst_QMessageBox::exec(QMessageBox *msgBox, int key)
{
    if (key == -1) {
        QTimer::singleShot(1000, msgBox, SLOT(close()));
    } else {
        keyToSend = key;
        sendKeySoon();
    }
    return msgBox->exec();
}

void tst_QMessageBox::sendKey()
{
    if (keyToSend == -2) {
        QApplication::activeModalWidget()->close();
        keyToSend = -1;
        return;
    }
    if (keyToSend == -1)
        return;
    QKeyEvent *ke = new QKeyEvent(QEvent::KeyPress, keyToSend, Qt::NoModifier);
    qApp->postEvent(QApplication::activeModalWidget(), ke);
    keyToSend = -1;
}

void tst_QMessageBox::sendKeySoon()
{
    keySendTimer.start();
}

void tst_QMessageBox::init()
{
    // if there is any pending key send from the last test, cancel it.
    keySendTimer.stop();
}

void tst_QMessageBox::initTestCase()
{
    keySendTimer.setInterval(1000);
    keySendTimer.setSingleShot(true);
    QVERIFY(QObject::connect(&keySendTimer, SIGNAL(timeout()), this, SLOT(sendKey())));
}

void tst_QMessageBox::sanityTest()
{
    QMessageBox msgBox;
    msgBox.setText("This is insane");
    for (int i = 0; i < 10; i++)
        msgBox.setIcon(QMessageBox::Icon(i));
    msgBox.setIconPixmap(QPixmap());
    msgBox.setIconPixmap(QPixmap("whatever.png"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setTextFormat(Qt::PlainText);
    exec(&msgBox);
}

void tst_QMessageBox::button()
{
    QMessageBox msgBox;
    msgBox.addButton("retry", QMessageBox::DestructiveRole);
    QVERIFY(msgBox.button(QMessageBox::Ok) == 0); // not added yet
    QPushButton *b1 = msgBox.addButton(QMessageBox::Ok);
    QCOMPARE(msgBox.button(QMessageBox::Ok), (QAbstractButton *)b1);  // just added
    QCOMPARE(msgBox.standardButton(b1), QMessageBox::Ok);
    msgBox.addButton(QMessageBox::Cancel);
    QCOMPARE(msgBox.standardButtons(), QMessageBox::Ok | QMessageBox::Cancel);

    // remove the cancel, should not exist anymore
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    QVERIFY(msgBox.button(QMessageBox::Cancel) == 0);
    QVERIFY(msgBox.button(QMessageBox::Yes) != 0);

    // should not crash
    QPushButton *b4 = new QPushButton;
    msgBox.addButton(b4, QMessageBox::DestructiveRole);
    msgBox.addButton(0, QMessageBox::ActionRole);
}

void tst_QMessageBox::defaultButton()
{
    QMessageBox msgBox;
    QVERIFY(msgBox.defaultButton() == 0);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.addButton(QMessageBox::Cancel);
    QVERIFY(msgBox.defaultButton() == 0);
    QPushButton pushButton;
    msgBox.setDefaultButton(&pushButton);
    QVERIFY(msgBox.defaultButton() == 0); // we have not added it yet
    QPushButton *retryButton = msgBox.addButton(QMessageBox::Retry);
    msgBox.setDefaultButton(retryButton);
    QCOMPARE(msgBox.defaultButton(), retryButton);
    exec(&msgBox);
    QCOMPARE(msgBox.clickedButton(), msgBox.button(QMessageBox::Cancel));

    exec(&msgBox, Qt::Key_Enter);
    QCOMPARE(msgBox.clickedButton(), (QAbstractButton *)retryButton);

    QAbstractButton *okButton = msgBox.button(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    QCOMPARE(msgBox.defaultButton(), (QPushButton *)okButton);
    exec(&msgBox, Qt::Key_Enter);
    QCOMPARE(msgBox.clickedButton(), okButton);
    msgBox.setDefaultButton(QMessageBox::Yes); // its not in there!
    QVERIFY(msgBox.defaultButton() == okButton);
    msgBox.removeButton(okButton);
    delete okButton;
    okButton = 0;
    QVERIFY(msgBox.defaultButton() == 0);
    msgBox.setDefaultButton(QMessageBox::Ok);
    QVERIFY(msgBox.defaultButton() == 0);
}

void tst_QMessageBox::escapeButton()
{
    QMessageBox msgBox;
    QVERIFY(msgBox.escapeButton() == 0);
    msgBox.addButton(QMessageBox::Ok);
    exec(&msgBox);
    QVERIFY(msgBox.clickedButton() == msgBox.button(QMessageBox::Ok)); // auto detected (one button only)
    msgBox.addButton(QMessageBox::Cancel);
    QVERIFY(msgBox.escapeButton() == 0);
    QPushButton invalidButton;
    msgBox.setEscapeButton(&invalidButton);
    QVERIFY(msgBox.escapeButton() == 0);
    QAbstractButton *retryButton = msgBox.addButton(QMessageBox::Retry);

    exec(&msgBox);
    QVERIFY(msgBox.clickedButton() == msgBox.button(QMessageBox::Cancel)); // auto detected (cancel)

    msgBox.setEscapeButton(retryButton);
    QCOMPARE(msgBox.escapeButton(), (QAbstractButton *)retryButton);

    // with escape
    exec(&msgBox, Qt::Key_Escape);
    QCOMPARE(msgBox.clickedButton(), retryButton);

    // with close
    exec(&msgBox);
    QCOMPARE(msgBox.clickedButton(), (QAbstractButton *)retryButton);

    QAbstractButton *okButton = msgBox.button(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    QCOMPARE(msgBox.escapeButton(), okButton);
    exec(&msgBox, Qt::Key_Escape);
    QCOMPARE(msgBox.clickedButton(), okButton);
    msgBox.setEscapeButton(QMessageBox::Yes); // its not in there!
    QVERIFY(msgBox.escapeButton() == okButton);
    msgBox.removeButton(okButton);
    delete okButton;
    okButton = 0;
    QVERIFY(msgBox.escapeButton() == 0);
    msgBox.setEscapeButton(QMessageBox::Ok);
    QVERIFY(msgBox.escapeButton() == 0);

    QMessageBox msgBox2;
    msgBox2.addButton(QMessageBox::Yes);
    msgBox2.addButton(QMessageBox::No);
    exec(&msgBox2);
    QVERIFY(msgBox2.clickedButton() == msgBox2.button(QMessageBox::No)); // auto detected (one No button only)

    QPushButton *rejectButton = new QPushButton;
    msgBox2.addButton(rejectButton, QMessageBox::RejectRole);
    exec(&msgBox2);
    QVERIFY(msgBox2.clickedButton() == rejectButton); // auto detected (one reject button only)

    msgBox2.addButton(new QPushButton, QMessageBox::RejectRole);
    exec(&msgBox2);
    QVERIFY(msgBox2.clickedButton() == msgBox2.button(QMessageBox::No)); // auto detected (one No button only)
}

void tst_QMessageBox::statics()
{
    QMessageBox::StandardButton (*statics[4])(QWidget *, const QString &,
         const QString&, QMessageBox::StandardButtons buttons,
         QMessageBox::StandardButton);

    statics[0] = QMessageBox::information;
    statics[1] = QMessageBox::critical;
    statics[2] = QMessageBox::question;
    statics[3] = QMessageBox::warning;

    for (int i = 0; i < 4; i++) {
        keyToSend = Qt::Key_Escape;
        sendKeySoon();
        QMessageBox::StandardButton sb = (*statics[i])(0, "caption",
            "text", QMessageBox::Yes | QMessageBox::No | QMessageBox::Help | QMessageBox::Cancel,
           QMessageBox::NoButton);
        QCOMPARE(sb, QMessageBox::Cancel);
        QCOMPARE(keyToSend, -1);

        keyToSend = -2; // close()
        sendKeySoon();
        sb = (*statics[i])(0, "caption",
           "text", QMessageBox::Yes | QMessageBox::No | QMessageBox::Help | QMessageBox::Cancel,
           QMessageBox::NoButton);
        QCOMPARE(sb, QMessageBox::Cancel);
        QCOMPARE(keyToSend, -1);

        keyToSend = Qt::Key_Enter;
        sendKeySoon();
        sb = (*statics[i])(0, "caption",
           "text", QMessageBox::Yes | QMessageBox::No | QMessageBox::Help,
           QMessageBox::Yes);
        QCOMPARE(sb, QMessageBox::Yes);
        QCOMPARE(keyToSend, -1);

        keyToSend = Qt::Key_Enter;
        sendKeySoon();
        sb = (*statics[i])(0, "caption",
           "text", QMessageBox::Yes | QMessageBox::No | QMessageBox::Help,
            QMessageBox::No);
        QCOMPARE(sb, QMessageBox::No);
        QCOMPARE(keyToSend, -1);
    }
}

void tst_QMessageBox::shortcut()
{
#ifdef Q_WS_MAC
    QSKIP("shortcuts are not used on MAC OS X", SkipAll);
#endif
    QMessageBox msgBox;
    msgBox.addButton("O&k", QMessageBox::YesRole);
    msgBox.addButton("&No", QMessageBox::YesRole);
    msgBox.addButton("&Maybe", QMessageBox::YesRole);
    QCOMPARE(exec(&msgBox, Qt::Key_M), 2);
}

void tst_QMessageBox::about()
{
    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    QMessageBox::about(0, "Caption", "This is an auto test");
    // On Mac, about and aboutQt are not modal, so we need to
    // explicitly run the event loop
#ifdef Q_WS_MAC
    QTRY_COMPARE(keyToSend, -1);
#else
    QCOMPARE(keyToSend, -1);
#endif

#if !defined(Q_OS_WINCE)
    keyToSend = Qt::Key_Enter;
#else
    keyToSend = Qt::Key_Escape;
#endif
    sendKeySoon();
    QMessageBox::aboutQt(0, "Caption");
#ifdef Q_WS_MAC
    QTRY_COMPARE(keyToSend, -1);
#else
    QCOMPARE(keyToSend, -1);
#endif
}

// Old message box enums
const int Old_Ok = 1;
const int Old_Cancel = 2;
const int Old_Yes = 3;
const int Old_No = 4;
const int Old_Abort = 5;
const int Old_Retry = 6;
const int Old_Ignore = 7;
const int Old_YesAll = 8;
const int Old_NoAll = 9;
const int Old_Default = 0x100;
const int Old_Escape = 0x200;

void tst_QMessageBox::staticSourceCompat()
{
    int ret;

    // source compat tests for < 4.2
    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", QMessageBox::Yes, QMessageBox::No);
    int expectedButton = int(QMessageBox::Yes);
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
    if (qobject_cast<QMacStyle *>(qApp->style()))
        expectedButton = int(QMessageBox::No);
#elif !defined(QT_NO_STYLE_CLEANLOOKS)
    if (qobject_cast<QCleanlooksStyle *>(qApp->style()))
        expectedButton = int(QMessageBox::No);
#endif
    QCOMPARE(ret, expectedButton);
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
    QCOMPARE(ret, int(QMessageBox::Yes));
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", QMessageBox::Yes, QMessageBox::No | QMessageBox::Default);
    QCOMPARE(ret, int(QMessageBox::No));
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape);
    QCOMPARE(ret, int(QMessageBox::Yes));
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", QMessageBox::Yes | QMessageBox::Escape, QMessageBox::No | QMessageBox::Default);
    QCOMPARE(ret, int(QMessageBox::No));
    QCOMPARE(keyToSend, -1);

    // the button text versions
    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", "Yes", "No", QString(), 1);
    QCOMPARE(ret, 1);
    QCOMPARE(keyToSend, -1);

    if (0) { // dont run these tests since the dialog wont close!
        keyToSend = Qt::Key_Escape;
        sendKeySoon();
        ret = QMessageBox::information(0, "title", "text", "Yes", "No", QString(), 1);
        QCOMPARE(ret, -1);
        QCOMPARE(keyToSend, -1);

        keyToSend = Qt::Key_Escape;
        sendKeySoon();
        ret = QMessageBox::information(0, "title", "text", "Yes", "No", QString(), 0, 1);
        QCOMPARE(ret, 1);
        QCOMPARE(keyToSend, -1);
    }
}

void tst_QMessageBox::instanceSourceCompat()
{
     QMessageBox mb("Application name here",
                    "Saving the file will overwrite the original file on the disk.\n"
                    "Do you really want to save?",
                    QMessageBox::Information,
                    QMessageBox::Yes | QMessageBox::Default,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape);
    mb.setButtonText(QMessageBox::Yes, "Save");
    mb.setButtonText(QMessageBox::No, "Discard");
    mb.addButton("&Revert", QMessageBox::RejectRole);
    mb.addButton("&Zoo", QMessageBox::ActionRole);

    QCOMPARE(exec(&mb, Qt::Key_Enter), int(QMessageBox::Yes));
    QCOMPARE(exec(&mb, Qt::Key_Escape), int(QMessageBox::Cancel));
#ifdef Q_WS_MAC
    QSKIP("mnemonics are not used on the MAC OS X", SkipAll);
#endif
    QCOMPARE(exec(&mb, Qt::ALT + Qt::Key_R), 0);
    QCOMPARE(exec(&mb, Qt::ALT + Qt::Key_Z), 1);
}

void tst_QMessageBox::staticBinaryCompat()
{
    int ret;

    // binary compat tests for < 4.2
    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", Old_Yes, Old_No, 0);
    int expectedButton = int(Old_Yes);
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
    if (qobject_cast<QMacStyle *>(qApp->style()))
        expectedButton = int(Old_No);
#elif !defined(QT_NO_STYLE_CLEANLOOKS)
    if (qobject_cast<QCleanlooksStyle *>(qApp->style()))
        expectedButton = int(Old_No);
#endif
    QCOMPARE(ret, expectedButton);
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", Old_Yes | Old_Escape, Old_No, 0);
    QCOMPARE(ret, int(Old_Yes));
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Enter;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", Old_Yes | Old_Default, Old_No, 0);
    QCOMPARE(ret, int(Old_Yes));
    QCOMPARE(keyToSend, -1);

#if 0
    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", Old_Yes, Old_No | Old_Default, 0);
    QCOMPARE(ret, -1);
    QCOMPARE(keyToSend, -1);
#endif

    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", Old_Yes | Old_Escape, Old_No | Old_Default, 0);
    QCOMPARE(ret, Old_Yes);
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    ret = QMessageBox::information(0, "title", "text", Old_Yes | Old_Default, Old_No | Old_Escape, 0);
    QCOMPARE(ret, Old_No);
    QCOMPARE(keyToSend, -1);

}

void tst_QMessageBox::instanceBinaryCompat()
{
     QMessageBox mb("Application name here",
                    "Saving the file will overwrite the original file on the disk.\n"
                    "Do you really want to save?",
                    QMessageBox::Information,
                    Old_Yes | Old_Default,
                    Old_No,
                    Old_Cancel | Old_Escape);
    mb.setButtonText(Old_Yes, "Save");
    mb.setButtonText(Old_No, "Discard");
    QCOMPARE(exec(&mb, Qt::Key_Enter), int(Old_Yes));
    QCOMPARE(exec(&mb, Qt::Key_Escape), int(Old_Cancel));
}

void tst_QMessageBox::testSymbols()
{
    return;

    QMessageBox::Icon icon;
    icon = QMessageBox::NoIcon;
    icon = QMessageBox::Information;
    icon = QMessageBox::Warning;
    icon = QMessageBox::Critical;
    icon = QMessageBox::Question;

    QMessageBox mb1;
    QMessageBox mb2(0);
    QMessageBox mb3(&mb1);
    QMessageBox mb3b("title", "text", QMessageBox::Critical, int(QMessageBox::Yes),
                     int(QMessageBox::No), int(QMessageBox::Cancel), &mb1, Qt::Dialog);

    QMessageBox::Button button = QMessageBox::NoButton;
    button = QMessageBox::Ok;
    button = QMessageBox::Cancel;
    button = QMessageBox::Yes;
    button = QMessageBox::No;
    button = QMessageBox::Abort;
    button = QMessageBox::Retry;
    button = QMessageBox::Ignore;
    button = QMessageBox::YesAll;
    button = QMessageBox::NoAll;
    button = QMessageBox::ButtonMask;
    button = QMessageBox::Default;
    button = QMessageBox::Escape;
    button = QMessageBox::FlagMask;

    mb1.setText("Foo");
    QCOMPARE(mb1.text(), "Foo");

    icon = mb1.icon();
    QVERIFY(icon == QMessageBox::NoIcon);
    mb1.setIcon(QMessageBox::Question);
    QVERIFY(mb1.icon() == QMessageBox::Question);

    QPixmap iconPixmap = mb1.iconPixmap();
    mb1.setIconPixmap(iconPixmap);
    QVERIFY(mb1.icon() == QMessageBox::NoIcon);

    QCOMPARE(mb1.buttonText(QMessageBox::Ok), "OK");
    QCOMPARE(mb1.buttonText(QMessageBox::Cancel), QString());
    QCOMPARE(mb1.buttonText(QMessageBox::Ok | QMessageBox::Default), QString());

    mb2.setButtonText(QMessageBox::Cancel, "Foo");
    mb2.setButtonText(QMessageBox::Ok, "Bar");
    mb2.setButtonText(QMessageBox::Ok | QMessageBox::Default, "Baz");

    QCOMPARE(mb2.buttonText(QMessageBox::Cancel), QString());
    QCOMPARE(mb2.buttonText(QMessageBox::Ok), "Bar");

    QVERIFY(mb3b.buttonText(QMessageBox::Yes).endsWith("Yes"));
    QCOMPARE(mb3b.buttonText(QMessageBox::YesAll), QString());
    QCOMPARE(mb3b.buttonText(QMessageBox::Ok), QString());

    mb3b.setButtonText(QMessageBox::Yes, "Blah");
    mb3b.setButtonText(QMessageBox::YesAll, "Zoo");
    mb3b.setButtonText(QMessageBox::Ok, "Zoo");

    QCOMPARE(mb3b.buttonText(QMessageBox::Yes), "Blah");
    QCOMPARE(mb3b.buttonText(QMessageBox::YesAll), QString());
    QCOMPARE(mb3b.buttonText(QMessageBox::Ok), QString());

    QCOMPARE(mb1.textFormat(), Qt::AutoText);
    mb1.setTextFormat(Qt::PlainText);
    QCOMPARE(mb1.textFormat(), Qt::PlainText);

    CONVENIENCE_FUNC_SYMS(information);
    CONVENIENCE_FUNC_SYMS_EXTRA(information);
    CONVENIENCE_FUNC_SYMS(question);
    CONVENIENCE_FUNC_SYMS_EXTRA(question);
    CONVENIENCE_FUNC_SYMS(warning);
    CONVENIENCE_FUNC_SYMS(critical);

    QSize sizeHint = mb1.sizeHint();
    QVERIFY(sizeHint.width() > 20 && sizeHint.height() > 20);

#ifdef QT3_SUPPORT
     //test QT3_SUPPORT stuff

    QMessageBox mb4("title", "text", icon, QMessageBox::Yes, QMessageBox::No | QMessageBox::Default,
                    QMessageBox::Cancel, &mb1, "name", true, Qt::Dialog);
    QMessageBox mb5(&mb1, "name");

    QPixmap pm = QMessageBox::standardIcon(QMessageBox::Question, Qt::GUIStyle(1));
    QPixmap pm2 = QMessageBox::standardIcon(QMessageBox::Question);

    QVERIFY(pm.toImage() == iconPixmap.toImage());
    QVERIFY(pm2.toImage() == iconPixmap.toImage());

    int ret1 = QMessageBox::message("title", "text");
    int ret2 = QMessageBox::message("title", "text", "OK");
    int ret3 = QMessageBox::message("title", "text", "OK", &mb1);
    int ret4 = QMessageBox::message("title", "text", "OK", &mb1, "name");
    qDebug("%d %d %d %d", ret1, ret2, ret3, ret4);

    bool ret5 = QMessageBox::query("title", "text");
    bool ret6 = QMessageBox::query("title", "text", "Ja");
    bool ret7 = QMessageBox::query("title", "text", "Ja", "Nein");
    bool ret8 = QMessageBox::query("title", "text", "Ja", "Nein", &mb1);
    bool ret9 = QMessageBox::query("title", "text", "Ja", "Nein", &mb1, "name");
    qDebug("%d %d %d %d %d", ret5, ret6, ret7, ret8, ret9);

    Q_UNUSED(ret1);
    Q_UNUSED(ret5);

    QPixmap pm3 = QMessageBox::standardIcon(QMessageBox::NoIcon);
    QVERIFY(pm3.isNull());

    pm3 = QMessageBox::standardIcon(QMessageBox::Information);
    QVERIFY(!pm3.isNull());
#endif //QT3_SUPPORT

    QMessageBox::about(&mb1, "title", "text");
    QMessageBox::aboutQt(&mb1);
    QMessageBox::aboutQt(&mb1, "title");
}

void tst_QMessageBox::detailsText()
{
    QMessageBox box;
    QString text("This is the details text.");
    box.setDetailedText(text);
    QCOMPARE(box.detailedText(), text);
}

void tst_QMessageBox::detailsButtonText()
{
    QMessageBox box;
    box.setDetailedText("bla");
    box.open();
    QApplication::postEvent(&box, new QEvent(QEvent::LanguageChange));
    QApplication::processEvents();
    QDialogButtonBox* bb = box.findChild<QDialogButtonBox*>("qt_msgbox_buttonbox");
    QVERIFY(bb); //get the detail button

    QList<QAbstractButton *> list = bb->buttons();
    QAbstractButton* btn = NULL;
    foreach(btn, list) {
        if (btn && (btn->inherits("QPushButton"))) {
            if (btn->text().remove("&") != QMessageBox::tr("OK")
                && btn->text() != QMessageBox::tr("Show Details...")) {
                QFAIL(qPrintable(QString("Unexpected messagebox button text: %1").arg(btn->text())));
            }
        }
    }
}

void tst_QMessageBox::incorrectDefaultButton()
{
    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    //Do not crash here
    QTest::ignoreMessage(QtWarningMsg, "QDialogButtonBox::createButton: Invalid ButtonRole, button not added");
    QMessageBox::question( 0, "", "I've been hit!",QMessageBox::Ok | QMessageBox::Cancel,QMessageBox::Save );
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    QTest::ignoreMessage(QtWarningMsg, "QDialogButtonBox::createButton: Invalid ButtonRole, button not added");
    QMessageBox::question( 0, "", "I've been hit!",QFlag(QMessageBox::Ok | QMessageBox::Cancel),QMessageBox::Save );
    QCOMPARE(keyToSend, -1);

    keyToSend = Qt::Key_Escape;
    sendKeySoon();
    QTest::ignoreMessage(QtWarningMsg, "QDialogButtonBox::createButton: Invalid ButtonRole, button not added");
    QTest::ignoreMessage(QtWarningMsg, "QDialogButtonBox::createButton: Invalid ButtonRole, button not added");
    //do not crash here -> call old function of QMessageBox in this case
    QMessageBox::question( 0, "", "I've been hit!",QMessageBox::Ok | QMessageBox::Cancel,QMessageBox::Save | QMessageBox::Cancel,QMessageBox::Ok);
    QCOMPARE(keyToSend, -1);
}

void tst_QMessageBox::updateSize()
{
    QMessageBox box;
#ifdef Q_WS_S60
    // In S60 messagebox is always occupies maximum width, i.e. screen width
    // so we need to have long enough text to split over several line
    box.setText("This is awesome long text");
#else
    box.setText("This is awesome");
#endif
    box.show();
    QSize oldSize = box.size();
    QString longText;
    for (int i = 0; i < 20; i++)
        longText += box.text();
    box.setText(longText);
    QVERIFY(box.size() != oldSize); // should have grown
    QVERIFY(box.width() > oldSize.width() || box.height() > oldSize.height());
    oldSize = box.size();
#ifndef Q_WS_S60
    // In S60 dialogs buttons are in softkey area -> message box size does not change
    box.setStandardButtons(QMessageBox::StandardButtons(0xFFFF));
    QVERIFY(box.size() != oldSize); // should have grown
    QVERIFY(box.width() > oldSize.width() || box.height() > oldSize.height());
#endif
}

void tst_QMessageBox::setInformativeText()
{
    QMessageBox msgbox(QMessageBox::Warning, "", "", QMessageBox::Ok);
    QString itext = "This is a very long message and it should make the dialog have enough width to fit this message in";
    msgbox.setInformativeText(itext);
    msgbox.show();
    QCOMPARE(msgbox.informativeText(), itext);
    QVERIFY2(msgbox.width() > 190, //verify it's big enough (task181688)
             qPrintable(QString("%1 > 190").arg(msgbox.width())));
}

void tst_QMessageBox::iconPixmap()
{
    QMessageBox messageBox;
    QCOMPARE(messageBox.iconPixmap(), QPixmap());
}

QTEST_MAIN(tst_QMessageBox)
#include "tst_qmessagebox.moc"
