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


#include <qfont.h>
#include <qfontdatabase.h>
#include <qfontinfo.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qlist.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QFont : public QObject
{
Q_OBJECT

public:
    tst_QFont();
    virtual ~tst_QFont();

public slots:
    void init();
    void cleanup();
private slots:
    void getSetCheck();
    void exactMatch();
    void compare();
    void resolve();
    void resetFont();
    void isCopyOf();
    void setFontRaw();
    void italicOblique();
    void insertAndRemoveSubstitutions();
    void serializeSpacing();
    void lastResortFont();
};

// Testing get/set functions
void tst_QFont::getSetCheck()
{
    QFont obj1;
    // Style QFont::style()
    // void QFont::setStyle(Style)
    obj1.setStyle(QFont::Style(QFont::StyleNormal));
    QCOMPARE(QFont::Style(QFont::StyleNormal), obj1.style());
    obj1.setStyle(QFont::Style(QFont::StyleItalic));
    QCOMPARE(QFont::Style(QFont::StyleItalic), obj1.style());
    obj1.setStyle(QFont::Style(QFont::StyleOblique));
    QCOMPARE(QFont::Style(QFont::StyleOblique), obj1.style());

    // StyleStrategy QFont::styleStrategy()
    // void QFont::setStyleStrategy(StyleStrategy)
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferDefault));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferDefault), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferBitmap));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferBitmap), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferDevice));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferDevice), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferOutline));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferOutline), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::ForceOutline));
    QCOMPARE(QFont::StyleStrategy(QFont::ForceOutline), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferMatch));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferMatch), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferQuality));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferQuality), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::PreferAntialias));
    QCOMPARE(QFont::StyleStrategy(QFont::PreferAntialias), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::NoAntialias));
    QCOMPARE(QFont::StyleStrategy(QFont::NoAntialias), obj1.styleStrategy());
    obj1.setStyleStrategy(QFont::StyleStrategy(QFont::OpenGLCompatible));
    QCOMPARE(QFont::StyleStrategy(QFont::OpenGLCompatible), obj1.styleStrategy());
}

tst_QFont::tst_QFont()
{
}

tst_QFont::~tst_QFont()
{

}

void tst_QFont::init()
{
// TODO: Add initialization code here.
// This will be executed immediately before each test is run.
}

void tst_QFont::cleanup()
{
// TODO: Add cleanup code here.
// This will be executed immediately after each test is run.
}

void tst_QFont::exactMatch()
{
    QFont font;

    // Check if a non-existing font hasn't an exact match
    font = QFont( "BogusFont", 33 );
    QVERIFY( !font.exactMatch() );

#ifdef Q_WS_WIN
    QSKIP("Exact matching on windows misses a lot because of the sample chars", SkipAll);
    return;
#endif

    QSKIP("This test is bogus on Unix with support for font aliases in fontconfig", SkipAll);
    return;

    QFontDatabase fdb;

    QList<QFontDatabase::WritingSystem> systems = fdb.writingSystems();
    for (int system = 0; system < systems.count(); ++system) {
        QStringList families = fdb.families(systems[system]);
        if (families.isEmpty())
            return;

        QStringList::ConstIterator f_it, f_end = families.end();
        for (f_it = families.begin(); f_it != f_end; ++f_it) {
            const QString &family = *f_it;
            if (family.contains('['))
                continue;

            QStringList styles = fdb.styles(family);
            QVERIFY(!styles.isEmpty());
            QStringList::ConstIterator s_it, s_end = styles.end();
            for (s_it = styles.begin(); s_it != s_end; ++s_it) {
                const QString &style = *s_it;

                if (fdb.isSmoothlyScalable(family, style)) {
                    // smoothly scalable font... don't need to load every pointsize
                    font = fdb.font(family, style, 12);
                    QFontInfo fontinfo(font);

                    if (! fontinfo.exactMatch()) {
                        // Unfortunately, this can fail, since
                        // QFontDatabase does not fill in all font
                        // properties.  Check to make sure that the
                        // test didn't fail for obvious reasons

                        if (fontinfo.family().isEmpty()
                                && fontinfo.pointSize() == 0) {
                            // this is a box rendering engine... this can happen from
                            // time to time, especially on X11 with iso10646-1 or
                            // unknown font encodings
                            continue;
                        }

#ifdef Q_WS_WIN32
                        if (font.family().startsWith("MS ") || fontinfo.family().startsWith("MS ")) {
                            /* qDebug("Family matching skipped for MS-Alias font: %s, fontinfo: %s",
                               font.family().latin1(), fontinfo.family().latin1());
                               */
                        } else
#endif
                        {
                            if (!(font.family() == fontinfo.family()
                                        || fontinfo.family().contains(font.family())
                                        || fontinfo.family().isEmpty())) {
                                qDebug("Test about to fail for font: %s, fontinfo: %s",
                                        font.family().toLatin1().constData(),
                                        fontinfo.family().toLatin1().constData());
                            }
                            QVERIFY(font.family() == fontinfo.family()
                                    || fontinfo.family().contains(font.family())
                                    || fontinfo.family().isEmpty());
                        }
                        if (font.pointSize() != -1) {
                            QVERIFY(font.pointSize() == fontinfo.pointSize());
                        } else {
                            QVERIFY(font.pixelSize() == fontinfo.pixelSize());
                        }
                        QVERIFY(font.italic() == fontinfo.italic());
                        if (font.weight() != fontinfo.weight()) {
                            qDebug("font is %s", font.toString().toLatin1().constData());
                        }
                        QVERIFY(font.weight() == fontinfo.weight());
                    } else {
                        font.setFixedPitch(!fontinfo.fixedPitch());
                        QFontInfo fontinfo1(font);
                        QVERIFY( !fontinfo1.exactMatch() );

                        font.setFixedPitch(fontinfo.fixedPitch());
                        QFontInfo fontinfo2(font);
                        QVERIFY( fontinfo2.exactMatch() );
                    }
                }
#if 0
                // ############## can only work if we have float point sizes in QFD
                else {
                    QList<int> sizes = fdb.pointSizes(family, style);
                    QVERIFY(!sizes.isEmpty());
                    QList<int>::ConstIterator z_it, z_end = sizes.end();
                    for (z_it = sizes.begin(); z_it != z_end; ++z_it) {
                        const int size = *z_it;

                        // Initialize the font, and check if it is an exact match
                        font = fdb.font(family, style, size);
                        QFontInfo fontinfo(font, (QFont::Script) script);

                        if (! fontinfo.exactMatch()) {
                            // Unfortunately, this can fail, since
                            // QFontDatabase does not fill in all font
                            // properties.  Check to make sure that the
                            // test didn't fail for obvious reasons

                            if (fontinfo.family().isEmpty()
                                    && fontinfo.pointSize() == 0) {
                                // this is a box rendering engine... this can happen from
                                // time to time, especially on X11 with iso10646-1 or
                                // unknown font encodings
                                continue;
                            }

                            // no need to skip MS-fonts here it seems
                            if (!(font.family() == fontinfo.family()
                                        || fontinfo.family().contains(font.family())
                                        || fontinfo.family().isEmpty())) {
                                qDebug("Test about to fail for font: %s, fontinfo: %s",
                                        font.family().latin1(), fontinfo.family().latin1());
                            }
                            QVERIFY(font.family() == fontinfo.family()
                                    || fontinfo.family().contains(font.family())
                                    || fontinfo.family().isEmpty());
                            if (font.pointSize() != -1) {
                                QVERIFY(font.pointSize() == fontinfo.pointSize());
                            } else {
                                QVERIFY(font.pixelSize() == fontinfo.pixelSize());
                            }
                            QVERIFY(font.italic() == fontinfo.italic());
                            QVERIFY(font.weight() == fontinfo.weight());
                        } else {
                            font.setFixedPitch(!fontinfo.fixedPitch());
                            QFontInfo fontinfo1(font, (QFont::Script) script);
                            QVERIFY( !fontinfo1.exactMatch() );

                            font.setFixedPitch(fontinfo.fixedPitch());
                            QFontInfo fontinfo2(font, (QFont::Script) script);
                            QVERIFY( fontinfo2.exactMatch() );
                        }
                    }
                }
#endif
            }
        }
    }
}

void tst_QFont::italicOblique()
{
    QFontDatabase fdb;

    QStringList families = fdb.families();
    if (families.isEmpty())
        return;

    QStringList::ConstIterator f_it, f_end = families.end();
    for (f_it = families.begin(); f_it != f_end; ++f_it) {

	QString family = *f_it;
	QStringList styles = fdb.styles(family);
	QVERIFY(!styles.isEmpty());
	QStringList::ConstIterator s_it, s_end = styles.end();
	for (s_it = styles.begin(); s_it != s_end; ++s_it) {
	    QString style = *s_it;

	    if (fdb.isSmoothlyScalable(family, style)) {
		if (style.contains("Oblique")) {
		    style.replace("Oblique", "Italic");
		} else if (style.contains("Italic")) {
		    style.replace("Italic", "Oblique");
		} else {
		    continue;
		}
		QFont f = fdb.font(family, style, 12);
		QVERIFY(f.italic());
	    }
	}
    }
}

void tst_QFont::compare()
{
    QFont font;
    {
	QFont font2 = font;
	font2.setPointSize( 24 );
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
    }
    {
	QFont font2 = font;
	font2.setPixelSize( 24 );
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
    }

    font.setPointSize(12);
    font.setItalic(false);
    font.setWeight(QFont::Normal);
    font.setUnderline(false);
    font.setStrikeOut(false);
    font.setOverline(false);
    {
	QFont font2 = font;
	font2.setPointSize( 24 );
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
    }
    {
	QFont font2 = font;
	font2.setPixelSize( 24 );
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
    }
    {
	QFont font2 = font;

	font2.setItalic(true);
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
	font2.setItalic(false);
	QVERIFY( font == font2 );
    QVERIFY(!(font < font2));

	font2.setWeight(QFont::Bold);
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
	font2.setWeight(QFont::Normal);
	QVERIFY( font == font2 );
    QVERIFY(!(font < font2));

	font.setUnderline(true);
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
	font.setUnderline(false);
	QVERIFY( font == font2 );
    QVERIFY(!(font < font2));

	font.setStrikeOut(true);
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
	font.setStrikeOut(false);
	QVERIFY( font == font2 );
    QVERIFY(!(font < font2));

	font.setOverline(true);
	QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
	font.setOverline(false);
	QVERIFY( font == font2 );
    QVERIFY(!(font < font2));

        font.setCapitalization(QFont::SmallCaps);
        QVERIFY( font != font2 );
    QCOMPARE(font < font2,!(font2 < font));
        font.setCapitalization(QFont::MixedCase);
        QVERIFY( font == font2 );
    QVERIFY(!(font < font2));
    }

#if defined(Q_WS_X11)
    {
	QFont font1, font2;
	font1.setRawName("-Adobe-Helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1");
	font2.setRawName("-Adobe-Helvetica-medium-r-normal--24-240-75-75-p-130-iso8859-1");
	QVERIFY(font1 != font2);
    }
#endif
}

void tst_QFont::resolve()
{
    QFont font;
    font.setPointSize(font.pointSize() * 2);
    font.setItalic(false);
    font.setWeight(QFont::Normal);
    font.setUnderline(false);
    font.setStrikeOut(false);
    font.setOverline(false);
    font.setStretch(QFont::Unstretched);

    QFont font1;
    font1.setWeight(QFont::Bold);
    QFont font2 = font1.resolve(font);

    QVERIFY(font2.weight() == font1.weight());

    QVERIFY(font2.pointSize() == font.pointSize());
    QVERIFY(font2.italic() == font.italic());
    QVERIFY(font2.underline() == font.underline());
    QVERIFY(font2.overline() == font.overline());
    QVERIFY(font2.strikeOut() == font.strikeOut());
    QVERIFY(font2.stretch() == font.stretch());

    QFont font3;
    font3.setStretch(QFont::UltraCondensed);
    QFont font4 = font3.resolve(font1).resolve(font);

    QVERIFY(font4.stretch() == font3.stretch());

    QVERIFY(font4.weight() == font.weight());
    QVERIFY(font4.pointSize() == font.pointSize());
    QVERIFY(font4.italic() == font.italic());
    QVERIFY(font4.underline() == font.underline());
    QVERIFY(font4.overline() == font.overline());
    QVERIFY(font4.strikeOut() == font.strikeOut());


    QFont f1,f2,f3;
    f2.setPointSize(45);
    f3.setPointSize(55);

    QFont f4 = f1.resolve(f2);
    QCOMPARE(f4.pointSize(), 45);
    f4 = f4.resolve(f3);
    QCOMPARE(f4.pointSize(), 55);
}

void tst_QFont::resetFont()
{
    QWidget parent;
    QFont parentFont = parent.font();
    parentFont.setPointSize(parentFont.pointSize() + 2);
    parent.setFont(parentFont);

    QWidget *child = new QWidget(&parent);

    QFont childFont = child->font();
    childFont.setBold(!childFont.bold());
    child->setFont(childFont);

    QVERIFY(parentFont.resolve() != 0);
    QVERIFY(childFont.resolve() != 0);
    QVERIFY(childFont != parentFont);

    child->setFont(QFont()); // reset font

    QVERIFY(child->font().resolve() == 0);
    QVERIFY(child->font().pointSize() == parent.font().pointSize());
    QVERIFY(parent.font().resolve() != 0);
}

void tst_QFont::isCopyOf()
{
    QFont font;
    QVERIFY(font.isCopyOf(QApplication::font()));

    QFont font2("bogusfont",  23);
    QVERIFY(! font2.isCopyOf(QApplication::font()));

    QFont font3 = font;
    QVERIFY(font3.isCopyOf(font));

    font3.setPointSize(256);
    QVERIFY(!font3.isCopyOf(font));
    font3.setPointSize(font.pointSize());
    QVERIFY(!font3.isCopyOf(font));
}

void tst_QFont::setFontRaw()
{
#ifndef Q_WS_X11
    QSKIP("Only tested on X11", SkipAll);
#else
    QFont f;
    f.setRawName("-*-fixed-bold-r-normal--0-0-*-*-*-0-iso8859-1");
//     qDebug("font family: %s", f.family().utf8());
    QFontDatabase fdb;
    QStringList families = fdb.families();
    bool found = false;
    for (int i = 0; i < families.size(); ++i) {
        QString str = families.at(i);
        if (str.contains('['))
            str = str.left(str.indexOf('[')-1);
        if (str.toLower() == "fixed")
            found = true;
    }
    if (!found) {
        QSKIP("Fixed font not available.", SkipSingle);
    }
    QCOMPARE(QFontInfo(f).family().left(5).toLower(), QString("fixed"));
#endif
}

void tst_QFont::insertAndRemoveSubstitutions()
{
    QFont::removeSubstitution("BogusFontFamily");
    // make sure it is empty before we start
    QVERIFY(QFont::substitutes("BogusFontFamily").isEmpty());
    QVERIFY(QFont::substitutes("bogusfontfamily").isEmpty());

    // inserting Foo
    QFont::insertSubstitution("BogusFontFamily", "Foo");
    QCOMPARE(QFont::substitutes("BogusFontFamily").count(), 1);
    QCOMPARE(QFont::substitutes("bogusfontfamily").count(), 1);

    // inserting Bar and Baz
    QStringList moreFonts;
    moreFonts << "Bar" << "Baz";
    QFont::insertSubstitutions("BogusFontFamily", moreFonts);
    QCOMPARE(QFont::substitutes("BogusFontFamily").count(), 3);
    QCOMPARE(QFont::substitutes("bogusfontfamily").count(), 3);

    QFont::removeSubstitution("BogusFontFamily");
    // make sure it is empty again
    QVERIFY(QFont::substitutes("BogusFontFamily").isEmpty());
    QVERIFY(QFont::substitutes("bogusfontfamily").isEmpty());
}


static QFont copyFont(const QFont &font1) // copy using a QDataStream
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream ds(&buffer);
    ds << font1;
    buffer.close();

    buffer.open(QIODevice::ReadOnly);
    QFont font2;
    ds >> font2;
    return font2;
}

void tst_QFont::serializeSpacing()
{
    QFont font;
    QCOMPARE(font.letterSpacing(), 0.);
    QCOMPARE(font.wordSpacing(), 0.);

    font.setLetterSpacing(QFont::AbsoluteSpacing, 105);
    QCOMPARE(font.letterSpacing(), 105.);
    QCOMPARE(font.letterSpacingType(), QFont::AbsoluteSpacing);
    QCOMPARE(font.wordSpacing(), 0.);
    QFont font2 = copyFont(font);
    QCOMPARE(font2.letterSpacing(), 105.);
    QCOMPARE(font2.letterSpacingType(), QFont::AbsoluteSpacing);
    QCOMPARE(font2.wordSpacing(), 0.);

    font.setWordSpacing(50.0);
    QCOMPARE(font.letterSpacing(), 105.);
    QCOMPARE(font.wordSpacing(), 50.);

    QFont font3 = copyFont(font);
    QCOMPARE(font3.letterSpacing(), 105.);
    QCOMPARE(font3.letterSpacingType(), QFont::AbsoluteSpacing);
    QCOMPARE(font3.wordSpacing(), 50.);
}

void tst_QFont::lastResortFont()
{
#ifdef Q_WS_QWS
    QSKIP("QFont::lastResortFont() may abort with qFatal() on QWS", SkipAll);
    // ...if absolutely no font is found. Just as ducumented for QFont::lastResortFont().
    // This happens on our CI machines which run QWS autotests.
#endif
    QFont font;
    QVERIFY(!font.lastResortFont().isEmpty());
}

QTEST_MAIN(tst_QFont)
#include "tst_qfont.moc"
