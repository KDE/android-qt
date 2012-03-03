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

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qabstracttextdocumentlayout.h>
#include <qimage.h>
#include <qtextobject.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QAbstractTextDocumentLayout : public QObject
{
Q_OBJECT

public:
    tst_QAbstractTextDocumentLayout();
    virtual ~tst_QAbstractTextDocumentLayout();

private slots:
    void getSetCheck();
    void maximumBlockCount();
};

tst_QAbstractTextDocumentLayout::tst_QAbstractTextDocumentLayout()
{
}

tst_QAbstractTextDocumentLayout::~tst_QAbstractTextDocumentLayout()
{
}

class MyAbstractTextDocumentLayout : public QAbstractTextDocumentLayout
{
    Q_OBJECT
public:
    MyAbstractTextDocumentLayout(QTextDocument *doc)
        : QAbstractTextDocumentLayout(doc)
        , gotFullLayout(false)
        , blockCount(0)
        , changeEvents(0)
    {
    }

    void draw(QPainter *, const PaintContext &) {}
    int hitTest(const QPointF &, Qt::HitTestAccuracy) const { return 0; }
    int pageCount() const { return 0; }
    QSizeF documentSize() const { return QSizeF(); }
    QRectF frameBoundingRect(QTextFrame *) const { return QRectF(); }
    QRectF blockBoundingRect(const QTextBlock &) const { return QRectF(); }
    void documentChanged(int from, int oldLength, int length) {
        ++changeEvents;

        QTextBlock last = document()->lastBlock();
        int lastPos = last.position() + last.length() - 1;
        if (from == 0 && length == lastPos)
            gotFullLayout = true;
    }

    bool gotFullLayout;
    int blockCount;
    int changeEvents;

public slots:
    void blockCountChanged(int bc) { blockCount = bc; }
};

// Testing get/set functions
void tst_QAbstractTextDocumentLayout::getSetCheck()
{
    QTextDocument doc;
    MyAbstractTextDocumentLayout obj1(&doc);
    // QPaintDevice * QAbstractTextDocumentLayout::paintDevice()
    // void QAbstractTextDocumentLayout::setPaintDevice(QPaintDevice *)
    QImage *var1 = new QImage(QSize(10,10), QImage::Format_ARGB32_Premultiplied);
    obj1.setPaintDevice(var1);
    QCOMPARE(static_cast<QPaintDevice *>(var1), obj1.paintDevice());
    obj1.setPaintDevice((QPaintDevice *)0);
    QCOMPARE(static_cast<QPaintDevice *>(0), obj1.paintDevice());
    delete var1;
}

void tst_QAbstractTextDocumentLayout::maximumBlockCount()
{
    QTextDocument doc;
    doc.setMaximumBlockCount(10);

    MyAbstractTextDocumentLayout layout(&doc);
    doc.setDocumentLayout(&layout);
    QObject::connect(&doc, SIGNAL(blockCountChanged(int)), &layout, SLOT(blockCountChanged(int)));

    QTextCursor cursor(&doc);
    for (int i = 0; i < 10; ++i) {
        cursor.insertBlock();
        cursor.insertText("bla");
    }

    QCOMPARE(layout.blockCount, 10);

    layout.gotFullLayout = false;
    layout.changeEvents = 0;
    cursor.insertBlock();
    QCOMPARE(layout.changeEvents, 2);
    cursor.insertText("foo");
    QCOMPARE(layout.changeEvents, 3);
    cursor.insertBlock();
    QCOMPARE(layout.changeEvents, 5);
    cursor.insertText("foo");
    QCOMPARE(layout.changeEvents, 6);

    QVERIFY(!layout.gotFullLayout);

    QCOMPARE(layout.blockCount, 10);
}

QTEST_MAIN(tst_QAbstractTextDocumentLayout)
#include "tst_qabstracttextdocumentlayout.moc"
