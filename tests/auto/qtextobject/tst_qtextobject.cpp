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

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qtextobject.h>
#include <qtextdocument.h>
#include <qtextedit.h>
#include <qtextcursor.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QTextObject : public QObject
{
Q_OBJECT

public:
    tst_QTextObject();
    virtual ~tst_QTextObject();

private slots:
    void getSetCheck();
    void testStandAloneTextObject();
};

tst_QTextObject::tst_QTextObject()
{
}

tst_QTextObject::~tst_QTextObject()
{
}

// Testing get/set functions
void tst_QTextObject::getSetCheck()
{
    QTextEdit edit;
    QTextFrame obj1(edit.document());
    // QTextFrameLayoutData * QTextFrame::layoutData()
    // void QTextFrame::setLayoutData(QTextFrameLayoutData *)
    QTextFrameLayoutData *var1 = new QTextFrameLayoutData;
    obj1.setLayoutData(var1);
    QCOMPARE(var1, obj1.layoutData());
    obj1.setLayoutData((QTextFrameLayoutData *)0);
    QCOMPARE((QTextFrameLayoutData *)0, obj1.layoutData());
    // delete var1; // No delete, since QTextFrame takes ownership

    QTextBlock obj2 = edit.textCursor().block();
    // QTextBlockUserData * QTextBlock::userData()
    // void QTextBlock::setUserData(QTextBlockUserData *)
    QTextBlockUserData *var2 = new QTextBlockUserData;
    obj2.setUserData(var2);
    QCOMPARE(var2, obj2.userData());
    obj2.setUserData((QTextBlockUserData *)0);
    QCOMPARE((QTextBlockUserData *)0, obj2.userData());

    // int QTextBlock::userState()
    // void QTextBlock::setUserState(int)
    obj2.setUserState(0);
    QCOMPARE(0, obj2.userState());
    obj2.setUserState(INT_MIN);
    QCOMPARE(INT_MIN, obj2.userState());
    obj2.setUserState(INT_MAX);
    QCOMPARE(INT_MAX, obj2.userState());
}

class TestTextObject : public QTextObject
{
public:
    TestTextObject(QTextDocument *document) : QTextObject(document) {}
};

void tst_QTextObject::testStandAloneTextObject()
{
    QTextDocument document;
    TestTextObject textObject(&document);

    QCOMPARE(textObject.document(), &document);
    // don't crash
    textObject.format();
    textObject.formatIndex();
    QCOMPARE(textObject.objectIndex(), -1);
}

QTEST_MAIN(tst_QTextObject)
#include "tst_qtextobject.moc"
