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


// Horrible hack, but this get this out of the way for now
// Carlos Duclos, 2007-12-11
#if !defined(Q_WS_MAC)

#include <QtTest/QtTest>
#include <QtGui/QtGui>
#include <private/qtextengine_p.h>

#include "bidireorderstring.h"


//TESTED_CLASS=
//TESTED_FILES=gui/widgets/qcombobox.h gui/widgets/qcombobox.cpp

class tst_QComplexText : public QObject
{
Q_OBJECT

public:
    tst_QComplexText();
    virtual ~tst_QComplexText();


public slots:
    void init();
    void cleanup();
private slots:
    void bidiReorderString_data();
    void bidiReorderString();
    void bidiCursor_qtbug2795();
    void bidiCursor_PDF();
};

tst_QComplexText::tst_QComplexText()
{
}

tst_QComplexText::~tst_QComplexText()
{

}

void tst_QComplexText::init()
{
// This will be executed immediately before each test is run.
// TODO: Add initialization code here.
}

void tst_QComplexText::cleanup()
{
// This will be executed immediately after each test is run.
// TODO: Add cleanup code here.
}

void tst_QComplexText::bidiReorderString_data()
{
    QTest::addColumn<QString>("logical");
    QTest::addColumn<QString>("VISUAL");
    QTest::addColumn<int>("basicDir");

    const LV *data = logical_visual;
    while ( data->name ) {
	//next we fill it with data
	QTest::newRow( data->name )
	    << QString::fromUtf8( data->logical )
	    << QString::fromUtf8( data->visual )
	    << (int) data->basicDir;
	data++;
    }
}

void tst_QComplexText::bidiReorderString()
{
    QFETCH( QString, logical );
    QFETCH( int,  basicDir );

    // replace \n with Unicode newline. The new algorithm ignores \n
    logical.replace(QChar('\n'), QChar(0x2028));

    QTextEngine e(logical, QFont());
    e.option.setTextDirection((QChar::Direction)basicDir == QChar::DirL ? Qt::LeftToRight : Qt::RightToLeft);
    e.itemize();
    quint8 levels[256];
    int visualOrder[256];
    int nitems = e.layoutData->items.size();
    int i;
    for (i = 0; i < nitems; ++i) {
	//qDebug("item %d bidiLevel=%d", i,  e.items[i].analysis.bidiLevel);
	levels[i] = e.layoutData->items[i].analysis.bidiLevel;
    }
    e.bidiReorder(nitems, levels, visualOrder);

    QString visual;
    for (i = 0; i < nitems; ++i) {
	QScriptItem &si = e.layoutData->items[visualOrder[i]];
	QString sub = logical.mid(si.position, e.length(visualOrder[i]));
	if (si.analysis.bidiLevel % 2) {
	    // reverse sub
	    QChar *a = (QChar *)sub.unicode();
	    QChar *b = a + sub.length() - 1;
	    while (a < b) {
		QChar tmp = *a;
		*a = *b;
		*b = tmp;
		++a;
		--b;
	    }
	    a = (QChar *)sub.unicode();
	    b = a + sub.length();
	    while (a<b) {
		*a = a->mirroredChar();
		++a;
	    }
	}
	visual += sub;
    }
    // replace Unicode newline back with  \n to compare.
    visual.replace(QChar(0x2028), QChar('\n'));

    QTEST(visual, "VISUAL");
}

void tst_QComplexText::bidiCursor_qtbug2795()
{
    QString str = QString::fromUtf8("الجزيرة نت");
    QTextLayout l1(str);

    l1.beginLayout();
    QTextLine line1 = l1.createLine();
    l1.endLayout();

    qreal x1 = line1.cursorToX(0) - line1.cursorToX(str.size());

    str.append("1");
    QTextLayout l2(str);
    l2.beginLayout();
    QTextLine line2 = l2.createLine();
    l2.endLayout();

    qreal x2 = line2.cursorToX(0) - line2.cursorToX(str.size());

    // The cursor should remain at the same position after a digit is appended
    QVERIFY(x1 == x2);
}

void tst_QComplexText::bidiCursor_PDF()
{
    QString str = QString::fromUtf8("\342\200\252hello\342\200\254");
    QTextLayout layout(str);

    layout.beginLayout();
    QTextLine line = layout.createLine();
    layout.endLayout();

    int size = str.size();

    QVERIFY(line.cursorToX(size) == line.cursorToX(size - 1));
}

QTEST_MAIN(tst_QComplexText)
#include "tst_qcomplextext.moc"

#endif // Q_WS_MAC

