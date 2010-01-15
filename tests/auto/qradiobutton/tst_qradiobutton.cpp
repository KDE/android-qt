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


#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>


class tst_QRadioButton : public QObject
{
Q_OBJECT
public:
    tst_QRadioButton(){};
    virtual ~tst_QRadioButton(){};

private slots:
    void task190739_focus();

private:
};

void tst_QRadioButton::task190739_focus()
{
    QWidget widget;
    QPushButton button1(&widget);
    button1.setText("button1");
    QLineEdit edit(&widget);
    edit.setFocus();

    QRadioButton radio1(&widget);
    radio1.setText("radio1");
    radio1.setFocusPolicy(Qt::TabFocus);
    radio1.setShortcut(QKeySequence("Ctrl+O"));

    QVBoxLayout layout(&widget);
    layout.addWidget(&button1);
    layout.addWidget(&edit);
    layout.addWidget(&radio1);

    widget.show();
    widget.activateWindow();
    QApplication::setActiveWindow(&widget);
    QTest::qWait(100);

    QVERIFY(edit.hasFocus());
    QVERIFY(!radio1.isChecked());

    QTest::keyClick(&edit, Qt::Key_O, Qt::ControlModifier, 20);
    QTest::qWait(200);
    QVERIFY(radio1.isChecked());
    QVERIFY(edit.hasFocus());
    QVERIFY(!radio1.hasFocus());
}




QTEST_MAIN(tst_QRadioButton)
#include "tst_qradiobutton.moc"
