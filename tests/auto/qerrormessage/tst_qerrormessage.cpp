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
#include <QErrorMessage>
#include <QDebug>
#include <QCheckBox>

class tst_QErrorMessage : public QObject
{
    Q_OBJECT

private slots:
    void dontShowAgain();
    void dontShowCategoryAgain();

};

void tst_QErrorMessage::dontShowAgain()
{
    QString plainString = QLatin1String("foo");
    QString htmlString = QLatin1String("foo<br>bar");
    QCheckBox *checkBox = 0;

    QErrorMessage errorMessageDialog(0);

    // show an error with plain string 
    errorMessageDialog.showMessage(plainString);
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    errorMessageDialog.close();

    errorMessageDialog.showMessage(plainString);
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    checkBox->setChecked(false);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(plainString);
    QVERIFY(!errorMessageDialog.isVisible());

    // show an error with an html string
    errorMessageDialog.showMessage(htmlString);
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(!checkBox->isChecked());
    checkBox->setChecked(true);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(htmlString);
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    checkBox->setChecked(false);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(htmlString);
    QVERIFY(!errorMessageDialog.isVisible());
}

void tst_QErrorMessage::dontShowCategoryAgain()
{
    QString htmlString = QLatin1String("foo<br>bar");
    QString htmlString2 = QLatin1String("foo2<br>bar2");
    QCheckBox *checkBox = 0;

    QErrorMessage errorMessageDialog(0);

    errorMessageDialog.showMessage(htmlString,"Cat 1");
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    checkBox->setChecked(true);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(htmlString,"Cat 1");
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    checkBox->setChecked(true);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(htmlString2,"Cat 1");
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    checkBox->setChecked(true);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(htmlString,"Cat 1");
    QVERIFY(errorMessageDialog.isVisible());
    checkBox = qFindChild<QCheckBox*>(&errorMessageDialog);
    QVERIFY(checkBox);
    QVERIFY(checkBox->isChecked());
    checkBox->setChecked(false);
    errorMessageDialog.close();

    errorMessageDialog.showMessage(htmlString2,"Cat 1");
    QVERIFY(!errorMessageDialog.isVisible());

    errorMessageDialog.showMessage(htmlString,"Cat 1");
    QVERIFY(!errorMessageDialog.isVisible());

    errorMessageDialog.showMessage(htmlString);
    QVERIFY(errorMessageDialog.isVisible());

    errorMessageDialog.showMessage(htmlString,"Cat 2");
    QVERIFY(errorMessageDialog.isVisible());
}

QTEST_MAIN(tst_QErrorMessage)
#include "tst_qerrormessage.moc"
