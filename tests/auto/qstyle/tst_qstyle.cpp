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
#include <qlayout.h>
#include "qstyle.h"
#include <qevent.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qstyleoption.h>
#include <qscrollbar.h>
#include <qprogressbar.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>

#include <qplastiquestyle.h>
#include <qwindowsstyle.h>
#include <qcdestyle.h>
#include <qmotifstyle.h>
#include <qcommonstyle.h>
#include <qproxystyle.h>
#include <qstylefactory.h>

#include <qimagereader.h>
#include <qimagewriter.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qmdiarea.h>

#if defined(Q_OS_SYMBIAN)
#define SRCDIR "."
#endif

#include <QCleanlooksStyle>

#ifdef Q_WS_MAC
#include <QMacStyle>
#endif

#ifdef Q_WS_WIN
#include <QWindowsXPStyle>
#include <QWindowsVistaStyle>
#endif

#ifdef Q_OS_WINCE
#include <QWindowsCEStyle>
#endif

#ifdef Q_OS_WINCE_WM
#include <QWindowsMobileStyle>
#include <windows.h>

static bool qt_wince_is_smartphone() {
    wchar_t tszPlatform[64];
    if (SystemParametersInfo(SPI_GETPLATFORMTYPE,
                             sizeof(tszPlatform)/sizeof(*tszPlatform),tszPlatform,0))
      if (0 == _tcsicmp(reinterpret_cast<const wchar_t *> (QString::fromLatin1("Smartphone").utf16()), tszPlatform))
            return true;
    return false;
}
#endif

#ifdef Q_WS_S60
#include <qs60style.h>
#endif

#include <qwidget.h>

//TESTED_CLASS=
//TESTED_FILES=gui/styles/qstyle.h gui/styles/qstyle.cpp gui/styles/qplastiquestyle.cpp gui/styles/qwindowsstyle.cpp gui/styles/qwindowsxpstyle.cpp gui/styles/qwindowsvistastyle.cpp gui/styles/qmotifstyle.cpp gui/styles/qs60style.cpp

class tst_QStyle : public QObject
{
    Q_OBJECT
public:
    tst_QStyle();
    virtual ~tst_QStyle();
private:
    void testAllFunctions(QStyle *);
    void testScrollBarSubControls(QStyle *);
    void testPainting(QStyle *style, const QString &platform);
private slots:
    void drawItemPixmap();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testMotifStyle();
    void testPlastiqueStyle();
    void testWindowsStyle();
    void testCDEStyle();
    void testWindowsXPStyle();
    void testWindowsVistaStyle();
    void testCleanlooksStyle();
    void testMacStyle();
    void testWindowsCEStyle();
    void testWindowsMobileStyle();
    void testS60Style();
    void testStyleFactory();
    void testProxyStyle();
    void pixelMetric();
    void progressBarChangeStyle();
    void defaultFont();
    void testDrawingShortcuts();
private:
    void lineUpLayoutTest(QStyle *);
    QWidget *testWidget;
};


tst_QStyle::tst_QStyle()
{
    testWidget = 0;
}

tst_QStyle::~tst_QStyle()
{
}

class MyWidget : public QWidget
{
public:
    MyWidget( QWidget* QWidget=0, const char* name=0 );
protected:
    void paintEvent( QPaintEvent* );
};

void tst_QStyle::init()
{
    testWidget = new MyWidget( 0, "testObject");
}

void tst_QStyle::cleanup()
{
    delete testWidget;
    testWidget = 0;
}

void tst_QStyle::initTestCase()
{
}

void tst_QStyle::cleanupTestCase()
{
}

void tst_QStyle::testStyleFactory()
{
    QStringList keys = QStyleFactory::keys();
#ifndef QT_NO_STYLE_MOTIF
    QVERIFY(keys.contains("Motif"));
#endif
#ifndef QT_NO_STYLE_CLEANLOOKS
    QVERIFY(keys.contains("Cleanlooks"));
#endif
#ifndef QT_NO_STYLE_PLASTIQUE
    QVERIFY(keys.contains("Plastique"));
#endif
#ifndef QT_NO_STYLE_CDE
    QVERIFY(keys.contains("CDE"));
#endif
#ifndef QT_NO_STYLE_WINDOWS
    QVERIFY(keys.contains("Windows"));
#endif
#ifndef QT_NO_STYLE_MOTIF
    QVERIFY(keys.contains("Motif"));
#endif
#ifdef Q_WS_WIN
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_XP &&
        (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based))
        QVERIFY(keys.contains("WindowsXP"));
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA &&
        (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based))
        QVERIFY(keys.contains("WindowsVista"));
#endif

    foreach (QString styleName , keys) {
        QStyle *style = QStyleFactory::create(styleName);
        QVERIFY2(style != 0, qPrintable(QString::fromLatin1("Fail to load style '%1'").arg(styleName)));
        delete style;
    }
}

class CustomProxy : public QProxyStyle
{
    virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                            const QWidget *widget = 0) const
    {
        if (metric == QStyle::PM_ButtonIconSize)
            return 13;
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
};

void tst_QStyle::testProxyStyle()
{
    QProxyStyle *proxyStyle = new QProxyStyle();
    QVERIFY(proxyStyle->baseStyle());
    QStyle *style = new QWindowsStyle;
    QVERIFY(style->proxy() == style);

    proxyStyle->setBaseStyle(style);
    QVERIFY(style->proxy() == proxyStyle);
    QVERIFY(style->parent() == proxyStyle);
    QVERIFY(proxyStyle->baseStyle() == style);

    testAllFunctions(proxyStyle);
    proxyStyle->setBaseStyle(0);
    QVERIFY(proxyStyle->baseStyle());
    qApp->setStyle(proxyStyle);

    QProxyStyle doubleProxy(new QProxyStyle(new QWindowsStyle()));
    testAllFunctions(&doubleProxy);

    CustomProxy customStyle;
    QLineEdit edit;
    edit.setStyle(&customStyle);
    QVERIFY(!customStyle.parent());
    QVERIFY(edit.style()->pixelMetric(QStyle::PM_ButtonIconSize) == 13);
}

void tst_QStyle::drawItemPixmap()
{
    testWidget->resize(300, 300);
    testWidget->show();

    QPixmap p(QString(SRCDIR) + "/task_25863.png", "PNG");
    QPixmap actualPix = QPixmap::grabWidget(testWidget);

#ifdef Q_OS_SYMBIAN
    // QPixmap cannot be assumed to be exactly same, unless it is created from exactly same content.
    // In Symbian, pixmap format might get "optimized" depending on how QPixmap is created.
    // Therefore, force the content to specific format and compare QImages.
    // Then re-create the QPixmaps and compare those.
    QImage i1 = p.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    p = QPixmap::fromImage(i1);
    QImage i2 = actualPix.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    actualPix = QPixmap::fromImage(i2);
    QVERIFY(i1 == i2);
#endif
    QVERIFY(pixmapsAreEqual(&actualPix,&p));
    testWidget->hide();
}

void tst_QStyle::testAllFunctions(QStyle *style)
{
    QStyleOption opt;
    opt.init(testWidget);

    testWidget->setStyle(style);

    //Tests styleHint with default arguments for potential crashes
    for ( int hint = 0 ; hint < int(QStyle::SH_Menu_Mask); ++hint) {
        style->styleHint(QStyle::StyleHint(hint));
        style->styleHint(QStyle::StyleHint(hint), &opt, testWidget);
    }

    //Tests pixelMetric with default arguments for potential crashes
    for ( int pm = 0 ; pm < int(QStyle::PM_LayoutVerticalSpacing); ++pm) {
        style->pixelMetric(QStyle::PixelMetric(pm));
        style->pixelMetric(QStyle::PixelMetric(pm), &opt, testWidget);
    }

    //Tests drawControl with default arguments for potential crashes
    for ( int control = 0 ; control < int(QStyle::CE_ColumnViewGrip); ++control) {
        QPixmap surface(QSize(200, 200));
        QPainter painter(&surface);
        style->drawControl(QStyle::ControlElement(control), &opt, &painter, 0);
    }

    //Tests drawComplexControl with default arguments for potential crashes
    {
        QPixmap surface(QSize(200, 200));
        QPainter painter(&surface);
        QStyleOptionComboBox copt1;
        copt1.init(testWidget);

        QStyleOptionGroupBox copt2;
        copt2.init(testWidget);
        QStyleOptionSizeGrip copt3;
        copt3.init(testWidget);
        QStyleOptionSlider copt4;
        copt4.init(testWidget);
        copt4.minimum = 0;
        copt4.maximum = 100;
        copt4.tickInterval = 25;
        copt4.sliderValue = 50;
        QStyleOptionSpinBox copt5;
        copt5.init(testWidget);
        QStyleOptionTitleBar copt6;
        copt6.init(testWidget);
        QStyleOptionToolButton copt7;
        copt7.init(testWidget);
        QStyleOptionQ3ListView copt8;
        copt8.init(testWidget);
        copt8.items << QStyleOptionQ3ListViewItem();
        QStyleOptionComplex copt9;
        copt9.initFrom(testWidget);

        style->drawComplexControl(QStyle::CC_SpinBox, &copt5, &painter, 0);
        style->drawComplexControl(QStyle::CC_ComboBox, &copt1, &painter, 0);
        style->drawComplexControl(QStyle::CC_ScrollBar, &copt4, &painter, 0);
        style->drawComplexControl(QStyle::CC_Slider, &copt4, &painter, 0);
        style->drawComplexControl(QStyle::CC_ToolButton, &copt7, &painter, 0);
        style->drawComplexControl(QStyle::CC_TitleBar, &copt6, &painter, 0);
        style->drawComplexControl(QStyle::CC_GroupBox, &copt2, &painter, 0);
        style->drawComplexControl(QStyle::CC_Dial, &copt4, &painter, 0);
#ifdef QT3_SUPPORT
        style->drawComplexControl(QStyle::CC_Q3ListView, &copt8, &painter, 0);
        style->drawComplexControl(QStyle::CC_MdiControls, &copt9, &painter, 0);
#endif
    }

    //Check standard pixmaps/icons
    for ( int i = 0 ; i < int(QStyle::SP_ToolBarVerticalExtensionButton); ++i) {
        QPixmap pixmap = style->standardPixmap(QStyle::StandardPixmap(i));
        if (pixmap.isNull()) {
            qWarning("missing StandardPixmap: %d", i);
        }
        QIcon icn = style->standardIcon(QStyle::StandardPixmap(i));
        if (icn.isNull()) {
            qWarning("missing StandardIcon: %d", i);
        }
    }

    style->itemPixmapRect(QRect(0, 0, 100, 100), Qt::AlignHCenter, QPixmap(200, 200));
    style->itemTextRect(QFontMetrics(qApp->font()), QRect(0, 0, 100, 100), Qt::AlignHCenter, true, QString("Test"));

    testScrollBarSubControls(style);
}

void tst_QStyle::testScrollBarSubControls(QStyle* style)
{
#ifdef Q_OS_WINCE_WM
    if (qobject_cast<QWindowsMobileStyle*>(style) && qt_wince_is_smartphone())
        QSKIP("SmartPhone doesn't have scrollbar subcontrols.", SkipAll);
#endif

    QScrollBar scrollBar;
    scrollBar.show();
    const QStyleOptionSlider opt = qt_qscrollbarStyleOption(&scrollBar);
    foreach (int subControl, QList<int>() << 1 << 2 << 4 << 8) {

#ifdef Q_WS_S60
// in s60style add line and sub line have been removed.
        if (subControl == QStyle::SC_ScrollBarAddLine || subControl == QStyle::SC_ScrollBarSubLine )
            continue;
#endif
        QRect sr = testWidget->style()->subControlRect(QStyle::CC_ScrollBar, &opt,
                                    QStyle::SubControl(subControl), &scrollBar);
        QVERIFY(sr.isNull() == false);
    }
}

void tst_QStyle::testPlastiqueStyle()
{
#if !defined(QT_NO_STYLE_PLASTIQUE)
    QPlastiqueStyle pstyle;
    testAllFunctions(&pstyle);
    lineUpLayoutTest(&pstyle);
#else
    QSKIP("No Plastique style", SkipAll);
#endif
}

void tst_QStyle::testCleanlooksStyle()
{
#if !defined(QT_NO_STYLE_CLEANLOOKS)
    QCleanlooksStyle cstyle;
    testAllFunctions(&cstyle);
    lineUpLayoutTest(&cstyle);
#else
    QSKIP("No Cleanlooks style", SkipAll);
#endif
}

void tst_QStyle::testWindowsStyle()
{
    QWindowsStyle wstyle;
    testAllFunctions(&wstyle);
    lineUpLayoutTest(&wstyle);

    // Tests drawing indeterminate progress with 0 size: QTBUG-15973
    QStyleOptionProgressBar pb;
    pb.rect = QRect(0,0,-9,0);
    QPixmap surface(QSize(200, 200));
    QPainter painter(&surface);
    wstyle.drawControl(QStyle::CE_ProgressBar, &pb, &painter, 0);
}

void tst_QStyle::testWindowsXPStyle()
{
#if defined(Q_WS_WIN) && !defined(QT_NO_STYLE_WINDOWSXP)
    QWindowsXPStyle xpstyle;
    testAllFunctions(&xpstyle);
    lineUpLayoutTest(&xpstyle);
#else
    QSKIP("No WindowsXP style", SkipAll);
#endif
}

void writeImage(const QString &fileName, QImage image)
{
    QImageWriter imageWriter(fileName);
    imageWriter.setFormat("png");
    qDebug() << "result " << imageWriter.write(image);
}

QImage readImage(const QString &fileName)
{
    QImageReader reader(fileName);
    return reader.read();
}


void tst_QStyle::testWindowsVistaStyle()
{
#if defined(Q_WS_WIN) && !defined(QT_NO_STYLE_WINDOWSVISTA)
    QWindowsVistaStyle vistastyle;
    testAllFunctions(&vistastyle);

    if (QSysInfo::WindowsVersion == QSysInfo::WV_VISTA)
        testPainting(&vistastyle, "vista");
    else if (QSysInfo::WindowsVersion == QSysInfo::WV_XP)
        testPainting(&vistastyle, "xp");
#endif
}

void comparePixmap(const QString &filename, const QPixmap &pixmap)
{
    QImage oldFile = readImage(filename);
    QPixmap oldPixmap = QPixmap::fromImage(oldFile);
    if (!oldFile.isNull())
        QVERIFY(pixmapsAreEqual(&pixmap, &oldPixmap));
    else
        writeImage(filename, pixmap.toImage());
}

void tst_QStyle::testPainting(QStyle *style, const QString &platform)
{
qDebug("TEST PAINTING");
    //Test Menu
    QString fileName = "images/" + platform + "/menu.png";
    QMenu menu;
    menu.setStyle(style);
    menu.show();
    menu.addAction(new QAction("Test 1", &menu));
    menu.addAction(new QAction("Test 2", &menu));
    QPixmap pixmap = QPixmap::grabWidget(&menu);
    comparePixmap(fileName, pixmap);

    //Push button
    fileName = "images/" + platform + "/button.png";
    QPushButton button("OK");
    button.setStyle(style);
    button.show();
    pixmap = QPixmap::grabWidget(&button);
    button.hide();
    comparePixmap(fileName, pixmap);

    //Push button
    fileName = "images/" + platform + "/radiobutton.png";
    QRadioButton radiobutton("Check");
    radiobutton.setStyle(style);
    radiobutton.show();
    pixmap = QPixmap::grabWidget(&radiobutton);
    radiobutton.hide();
    comparePixmap(fileName, pixmap);

    //Combo box
    fileName = "images/" + platform + "/combobox.png";
    QComboBox combobox;
    combobox.setStyle(style);
    combobox.addItem("Test 1");
    combobox.addItem("Test 2");
    combobox.show();
    pixmap = QPixmap::grabWidget(&combobox);
    combobox.hide();
    comparePixmap(fileName, pixmap);

    //Spin box
    fileName = "images/" + platform + "/spinbox.png";
    QDoubleSpinBox spinbox;
    spinbox.setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    spinbox.setStyle(style);
    spinbox.show();
    pixmap = QPixmap::grabWidget(&spinbox);
    spinbox.hide();
    comparePixmap(fileName, pixmap);
    QLocale::setDefault(QLocale::system());

    //Slider
    fileName = "images/" + platform + "/slider.png";
    QSlider slider;
    slider.setStyle(style);
    slider.show();
    pixmap = QPixmap::grabWidget(&slider);
    slider.hide();
    comparePixmap(fileName, pixmap);

    //Line edit
    fileName = "images/" + platform + "/lineedit.png";
    QLineEdit lineedit("Test text");
    lineedit.setStyle(style);
    lineedit.show();
    pixmap = QPixmap::grabWidget(&lineedit);
    lineedit.hide();
    comparePixmap(fileName, pixmap);

    //MDI
    fileName = "images/" + platform + "/mdi.png";
    QMdiArea mdiArea;
    mdiArea.addSubWindow(new QWidget(&mdiArea));
    mdiArea.resize(200, 200);
    mdiArea.setStyle(style);
    mdiArea.show();
    pixmap = QPixmap::grabWidget(&mdiArea);
    mdiArea.hide();
    comparePixmap(fileName, pixmap);

    // QToolButton
    fileName = "images/" + platform + "/toolbutton.png";
    QToolButton tb;
    tb.setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tb.setText("AaQqPpXx");
    tb.setIcon(style->standardPixmap(QStyle::SP_DirHomeIcon));
    tb.setStyle(style);
    tb.show();
    pixmap = QPixmap::grabWidget(&tb);
    tb.hide();
    comparePixmap(fileName, pixmap);

}

void tst_QStyle::testMacStyle()
{
#ifdef Q_WS_MAC
    QMacStyle mstyle;
    testAllFunctions(&mstyle);
#endif
}

void tst_QStyle::testMotifStyle()
{
#if !defined(QT_NO_STYLE_MOTIF)
    QMotifStyle mstyle;
    testAllFunctions(&mstyle);
#else
    QSKIP("No Motif style", SkipAll);
#endif
}

void tst_QStyle::testCDEStyle()
{
#if !defined(QT_NO_STYLE_CDE)
    QCDEStyle cstyle;
    testAllFunctions(&cstyle);
#else
    QSKIP("No CDE style", SkipAll);
#endif
}

void tst_QStyle::testWindowsCEStyle()
{
#if defined(Q_OS_WINCE)
    QWindowsCEStyle cstyle;
    testAllFunctions(&cstyle);
#else
    QSKIP("No WindowsCEStyle style", SkipAll);
#endif
}

void tst_QStyle::testWindowsMobileStyle()
{
#if defined(Q_OS_WINCE_WM)
    QWindowsMobileStyle cstyle;
    testAllFunctions(&cstyle);
#else
    QSKIP("No WindowsMobileStyle style", SkipAll);
#endif
}

void tst_QStyle::testS60Style()
    {
#if defined(Q_WS_S60)
    QS60Style cstyle;
    testAllFunctions(&cstyle);
#else
    QSKIP("No S60Style style", SkipAll);
#endif
    }

// Helper class...

MyWidget::MyWidget( QWidget* parent, const char* name )
    : QWidget( parent )
{
    setObjectName(name);
}

void MyWidget::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    QPixmap big(400,400);
    big.fill(Qt::green);
    style()->drawItemPixmap(&p, rect(), Qt::AlignCenter, big);
}


class Qt42Style : public QWindowsStyle
{
    Q_OBJECT
public:
    Qt42Style() : QWindowsStyle()
    {
        margin_toplevel = 10;
        margin = 5;
        spacing = 0;
    }

    virtual int pixelMetric(PixelMetric metric, const QStyleOption * option = 0,
                            const QWidget * widget = 0 ) const;

    int margin_toplevel;
    int margin;
    int spacing;

};

int Qt42Style::pixelMetric(PixelMetric metric, const QStyleOption * option /*= 0*/,
                                   const QWidget * widget /*= 0*/ ) const
{
    switch (metric) {
        case QStyle::PM_DefaultTopLevelMargin:
            return margin_toplevel;
        break;
        case QStyle::PM_DefaultChildMargin:
            return margin;
        break;
        case QStyle::PM_DefaultLayoutSpacing:
            return spacing;
        break;
        default:
            break;
    }
    return QWindowsStyle::pixelMetric(metric, option, widget);
}


void tst_QStyle::pixelMetric()
{
    Qt42Style *style = new Qt42Style();
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultTopLevelMargin), 10);
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultChildMargin), 5);
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultLayoutSpacing), 0);

    style->margin_toplevel = 0;
    style->margin = 0;
    style->spacing = 0;
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultTopLevelMargin), 0);
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultChildMargin), 0);
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultLayoutSpacing), 0);

    style->margin_toplevel = -1;
    style->margin = -1;
    style->spacing = -1;
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultTopLevelMargin), -1);
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultChildMargin), -1);
    QCOMPARE(style->pixelMetric(QStyle::PM_DefaultLayoutSpacing), -1);

    delete style;
}

void tst_QStyle::progressBarChangeStyle()
{
#if !defined(QT_NO_STYLE_PLASTIQUE) && !defined(QT_NO_STYLE_WINDOWS)
    //test a crashing situation (task 143530)
    //where changing the styles and deleting a progressbar would crash

    QWindowsStyle style1;
    QPlastiqueStyle style2;

    QProgressBar *progress=new QProgressBar;
    progress->setStyle(&style1);

    progress->show();

    progress->setStyle(&style2);

    QTest::qWait(100);
    delete progress;

    QTest::qWait(100);

    //before the correction, there would be a crash here
#elif !defined(QT_NO_STYLE_S60) && !defined(QT_NO_STYLE_WINDOWS)
    //test a crashing situation (task 143530)
    //where changing the styles and deleting a progressbar would crash

    QWindowsStyle style1;
    QS60Style style2;

    QProgressBar *progress=new QProgressBar;
    progress->setStyle(&style1);

    progress->show();

    progress->setStyle(&style2);

    QTest::qWait(100);
    delete progress;

    QTest::qWait(100);

    //before the correction, there would be a crash here
#else
    QSKIP("Either style Plastique or Windows or S60 missing", SkipAll);
#endif
}

void tst_QStyle::lineUpLayoutTest(QStyle *style)
{
    QWidget widget;
    QHBoxLayout layout;
    QFont font;
    font.setPointSize(9); //Plastique is lined up for odd numbers...
    widget.setFont(font);
    QSpinBox spinbox(&widget);
    QLineEdit lineedit(&widget);
    QComboBox combo(&widget);
    combo.setEditable(true);
    layout.addWidget(&spinbox);
    layout.addWidget(&lineedit);
    layout.addWidget(&combo);
    widget.setLayout(&layout);
        widget.setStyle(style);
        // propagate the style.
        foreach (QWidget *w, qFindChildren<QWidget *>(&widget))
            w->setStyle(style);
    widget.show();
        QTest::qWait( 500 );

    QVERIFY(qAbs(spinbox.height() - lineedit.height()) <= 1);
    QVERIFY(qAbs(spinbox.height() - combo.height()) <= 1);
}

void tst_QStyle::defaultFont()
{
    QFont defaultFont = qApp->font();
    QFont pointFont = defaultFont;
    pointFont.setPixelSize(9);
    qApp->setFont(pointFont);
    QPushButton button;
    button.show();
    qApp->processEvents();
    qApp->setFont(defaultFont);
}

class DrawTextStyle : public QProxyStyle
{
    Q_OBJECT
public:
    DrawTextStyle(QStyle *base = 0) : QProxyStyle(), alignment(0) { setBaseStyle(base); }
    void drawItemText(QPainter *painter, const QRect &rect,
                              int flags, const QPalette &pal, bool enabled,
                              const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const
    {
        DrawTextStyle *that = (DrawTextStyle *)this;
        that->alignment = flags;
        QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
    }
    int alignment;
};

void tst_QStyle::testDrawingShortcuts()
{
    {   
        QWidget w;
        QToolButton *tb = new QToolButton(&w);
        tb->setText("&abc");
        DrawTextStyle *dts = new DrawTextStyle;
        w.show();
        tb->setStyle(dts);
        QPixmap::grabWidget(tb);
        QStyleOptionToolButton sotb;
        sotb.initFrom(tb);
        bool showMnemonic = dts->styleHint(QStyle::SH_UnderlineShortcut, &sotb, tb);
        QVERIFY(dts->alignment & (showMnemonic ? Qt::TextShowMnemonic : Qt::TextHideMnemonic));
        delete dts;
    }
    {
        QToolBar w;
        QToolButton *tb = new QToolButton(&w);
        tb->setText("&abc");
        DrawTextStyle *dts = new DrawTextStyle;
        w.addWidget(tb);
        w.show();
        tb->setStyle(dts);
        QPixmap::grabWidget(tb);
        QStyleOptionToolButton sotb;
        sotb.initFrom(tb);
        bool showMnemonic = dts->styleHint(QStyle::SH_UnderlineShortcut, &sotb, tb);
        QVERIFY(dts->alignment & (showMnemonic ? Qt::TextShowMnemonic : Qt::TextHideMnemonic));
        delete dts;
     }   
}

QTEST_MAIN(tst_QStyle)
#include "tst_qstyle.moc"
