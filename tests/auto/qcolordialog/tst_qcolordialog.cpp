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
#include <QtGui/QtGui>

#include "../../shared/util.h"

//TESTED_CLASS=
//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QtTestEventThread)

class tst_QColorDialog : public QObject
{
    Q_OBJECT
public:
    tst_QColorDialog();
    virtual ~tst_QColorDialog();

#ifndef Q_WS_MAC
public slots:
    void postKeyReturn();
private slots:
    void defaultOkButton();
#endif

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void native_activeModalWidget();
    void task247349_alpha();
};

class TestNativeDialog : public QColorDialog
{
    Q_OBJECT
public:
    QWidget *m_activeModalWidget;

    TestNativeDialog(QWidget *parent = 0)
        : QColorDialog(parent), m_activeModalWidget(0)
    {
        QTimer::singleShot(1, this, SLOT(test_activeModalWidgetSignal()));
    }

public slots:
    void test_activeModalWidgetSignal()
    {
        m_activeModalWidget = qApp->activeModalWidget();
    }
};

tst_QColorDialog::tst_QColorDialog()
{
}

tst_QColorDialog::~tst_QColorDialog()
{
}

void tst_QColorDialog::native_activeModalWidget()
{
    // Check that QApplication::activeModalWidget retruns the
    // color dialog when it is executing, even when using a native
    // dialog:
    TestNativeDialog d;
    QTimer::singleShot(1000, &d, SLOT(hide()));
    d.exec();
    QVERIFY(&d == d.m_activeModalWidget);
}

void tst_QColorDialog::initTestCase()
{
}

void tst_QColorDialog::cleanupTestCase()
{
}

void tst_QColorDialog::init()
{
}

void tst_QColorDialog::cleanup()
{
}

#ifndef Q_WS_MAC
//copied from QFontDialogTest
void tst_QColorDialog::postKeyReturn() {
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i=0; i<list.count(); ++i) {
        QColorDialog *dialog = qobject_cast<QColorDialog *>(list[i]);
        if (dialog) {
            QTest::keyClick( list[i], Qt::Key_Return, Qt::NoModifier );
            return;
        }
    }
}

void tst_QColorDialog::defaultOkButton()
{
    bool ok = false;
    QTimer::singleShot(500, this, SLOT(postKeyReturn()));
    QColorDialog::getRgba(0xffffffff, &ok);
    QVERIFY(ok);
}
#endif

void tst_QColorDialog::task247349_alpha()
{
    QColorDialog dialog;
    dialog.setOption(QColorDialog::ShowAlphaChannel, true);
    int alpha = 0x17;
    dialog.setCurrentColor(QColor(0x01, 0x02, 0x03, alpha));
    QCOMPARE(alpha, dialog.currentColor().alpha());
    QCOMPARE(alpha, qAlpha(dialog.currentColor().rgba()));
}

QTEST_MAIN(tst_QColorDialog)
#include "tst_qcolordialog.moc"
