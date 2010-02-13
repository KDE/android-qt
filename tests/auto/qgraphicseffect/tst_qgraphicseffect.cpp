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
#include <QtGui/qdesktopwidget.h>
#include <QtGui/qgraphicseffect.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qstyleoption.h>

#include "../../shared/util.h"
#include <private/qgraphicseffect_p.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QGraphicsEffect : public QObject
{
    Q_OBJECT
public slots:
    void initTestCase();

private slots:
    void setEnabled();
    void source();
    void boundingRectFor();
    void boundingRect();
    void draw();
    void opacity();
    void grayscale();
    void colorize();
    void drawPixmapItem();
    void deviceCoordinateTranslateCaching();
    void inheritOpacity();
};

void tst_QGraphicsEffect::initTestCase()
{}

class CustomItem : public QGraphicsRectItem
{
public:
    CustomItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = 0)
        : QGraphicsRectItem(x, y, width, height, parent), numRepaints(0),
          m_painter(0), m_styleOption(0)
    {}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        m_painter = painter;
        m_styleOption = option;
        ++numRepaints;
        QGraphicsRectItem::paint(painter, option, widget);
    }

    void reset()
    {
        numRepaints = 0;
        m_painter = 0;
        m_styleOption = 0;
    }

    int numRepaints;
    QPainter *m_painter;
    const QStyleOption *m_styleOption;
};

class CustomEffect : public QGraphicsEffect
{
public:
    CustomEffect()
        : QGraphicsEffect(), numRepaints(0), m_margin(10),
          doNothingInDraw(false), m_painter(0), m_styleOption(0), m_source(0), m_opacity(1.0)
    {}

    QRectF boundingRectFor(const QRectF &rect) const
    { return rect.adjusted(-m_margin, -m_margin, m_margin, m_margin); }

    void reset()
    {
        numRepaints = 0;
        m_sourceChangedFlags = QGraphicsEffect::ChangeFlags();
        m_painter = 0;
        m_styleOption = 0;
        m_source = 0;
        m_opacity = 1.0;
    }

    void setMargin(int margin)
    {
        m_margin = margin;
        updateBoundingRect();
    }

    int margin() const
    { return m_margin; }

    void draw(QPainter *painter)
    {
        ++numRepaints;
        if (doNothingInDraw)
            return;
        m_source = source();
        m_painter = painter;
        m_styleOption = source()->styleOption();
        m_opacity = painter->opacity();
        drawSource(painter);
    }

    void sourceChanged(QGraphicsEffect::ChangeFlags flags)
    { m_sourceChangedFlags |= flags; }

    int numRepaints;
    int m_margin;
    QGraphicsEffect::ChangeFlags m_sourceChangedFlags;
    bool doNothingInDraw;
    QPainter *m_painter;
    const QStyleOption *m_styleOption;
    QGraphicsEffectSource *m_source;
    qreal m_opacity;
};

void tst_QGraphicsEffect::setEnabled()
{
    CustomEffect effect;
    QVERIFY(effect.isEnabled());

    effect.setEnabled(false);
    QVERIFY(!effect.isEnabled());
}

void tst_QGraphicsEffect::source()
{
    QPointer<CustomEffect> effect = new CustomEffect;
    QVERIFY(!effect->source());
    QVERIFY(!effect->m_sourceChangedFlags);

    // Install effect on QGraphicsItem.
    QGraphicsItem *item = new QGraphicsRectItem(0, 0, 10, 10);
    item->setGraphicsEffect(effect);
    QVERIFY(effect->source());
    QCOMPARE(effect->source()->graphicsItem(), item);
    QVERIFY(effect->m_sourceChangedFlags & QGraphicsEffect::SourceAttached);
    effect->reset();

    // Make sure disabling/enabling the effect doesn't change the source.
    effect->setEnabled(false);
    QVERIFY(effect->source());
    QCOMPARE(effect->source()->graphicsItem(), item);
    QVERIFY(!effect->m_sourceChangedFlags);
    effect->reset();

    effect->setEnabled(true);
    QVERIFY(effect->source());
    QCOMPARE(effect->source()->graphicsItem(), item);
    QVERIFY(!effect->m_sourceChangedFlags);
    effect->reset();

    // Uninstall effect on QGraphicsItem.
    effect->reset();
    item->setGraphicsEffect(0);
    QVERIFY(!effect);
    effect = new CustomEffect;

    // The item takes ownership and should delete the effect when destroyed.
    item->setGraphicsEffect(effect);
    QPointer<QGraphicsEffectSource> source = effect->source();
    QVERIFY(source);
    QCOMPARE(source->graphicsItem(), item);
    delete item;
    QVERIFY(!effect);
    QVERIFY(!source);
}

void tst_QGraphicsEffect::boundingRectFor()
{
    CustomEffect effect;
    int margin = effect.margin();
    const QRectF source(0, 0, 100, 100);
    QCOMPARE(effect.boundingRectFor(source), source.adjusted(-margin, -margin, margin, margin));

    effect.setMargin(margin = 20);
    QCOMPARE(effect.boundingRectFor(source), source.adjusted(-margin, -margin, margin, margin));
}

void tst_QGraphicsEffect::boundingRect()
{
    // No source; empty bounding rect.
    CustomEffect *effect = new CustomEffect;
    QCOMPARE(effect->boundingRect(), QRectF());

    // Install effect on QGraphicsItem.
    QRectF itemRect(0, 0, 100, 100);
    QGraphicsRectItem *item = new QGraphicsRectItem;
    item->setRect(itemRect);
    item->setGraphicsEffect(effect);
    int margin = effect->margin();
    QCOMPARE(effect->boundingRect(), itemRect.adjusted(-margin, -margin, margin, margin));
    QCOMPARE(effect->boundingRect(), effect->boundingRectFor(itemRect));

    // Make sure disabling/enabling the effect doesn't change the bounding rect.
    effect->setEnabled(false);
    QCOMPARE(effect->boundingRect(), itemRect.adjusted(-margin, -margin, margin, margin));
    QCOMPARE(effect->boundingRect(), effect->boundingRectFor(itemRect));
    effect->setEnabled(true);
    QCOMPARE(effect->boundingRect(), itemRect.adjusted(-margin, -margin, margin, margin));
    QCOMPARE(effect->boundingRect(), effect->boundingRectFor(itemRect));

    // Change effect margins.
    effect->setMargin(margin = 20);
    QCOMPARE(effect->boundingRect(), itemRect.adjusted(-margin, -margin, margin, margin));
    QCOMPARE(effect->boundingRect(), effect->boundingRectFor(itemRect));

    // Uninstall effect on QGraphicsItem.
    QPointer<CustomEffect> ptr = effect;
    item->setGraphicsEffect(0);
    QVERIFY(!ptr);

    delete item;
}

void tst_QGraphicsEffect::draw()
{
    QGraphicsScene scene;
    CustomItem *item = new CustomItem(0, 0, 100, 100);
    scene.addItem(item);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);
    QTRY_VERIFY(item->numRepaints > 0);
    item->reset();

    // Make sure installing the effect triggers a repaint.
    CustomEffect *effect = new CustomEffect;
    item->setGraphicsEffect(effect);
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 1);
    QCOMPARE(item->numRepaints, 1);

    // Make sure QPainter* and QStyleOptionGraphicsItem* stays persistent
    // during QGraphicsEffect::draw/QGraphicsItem::paint.
    QVERIFY(effect->m_painter);
    QCOMPARE(effect->m_painter, item->m_painter);
    QCOMPARE(effect->m_styleOption, item->m_styleOption);
    // Make sure QGraphicsEffect::source is persistent.
    QCOMPARE(effect->m_source, effect->source());
    effect->reset();
    item->reset();

    // Make sure updating the source triggers a repaint.
    item->update();
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 1);
    QCOMPARE(item->numRepaints, 1);
    QVERIFY(effect->m_sourceChangedFlags & QGraphicsEffect::SourceInvalidated);
    effect->reset();
    item->reset();

    // Make sure changing the effect's bounding rect triggers a repaint.
    effect->setMargin(20);
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 1);
    QCOMPARE(item->numRepaints, 1);
    effect->reset();
    item->reset();

    // Make sure change the item's bounding rect triggers a repaint.
    item->setRect(0, 0, 50, 50);
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 1);
    QCOMPARE(item->numRepaints, 1);
    QVERIFY(effect->m_sourceChangedFlags & QGraphicsEffect::SourceBoundingRectChanged);
    effect->reset();
    item->reset();

    // Make sure the effect is the one to issue a repaint of the item.
    effect->doNothingInDraw = true;
    item->update();
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 1);
    QCOMPARE(item->numRepaints, 0);
    effect->doNothingInDraw = false;
    effect->reset();
    item->reset();

    // Make sure we update the source when disabling/enabling the effect.
    effect->setEnabled(false);
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 0);
    QCOMPARE(item->numRepaints, 1);
    effect->reset();
    item->reset();

    effect->setEnabled(true);
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 1);
    QCOMPARE(item->numRepaints, 1);
    effect->reset();
    item->reset();

    // Effect is already enabled; nothing should happen.
    effect->setEnabled(true);
    QTest::qWait(50);
    QCOMPARE(effect->numRepaints, 0);
    QCOMPARE(item->numRepaints, 0);

    // Make sure uninstalling an effect triggers a repaint.
    QPointer<CustomEffect> ptr = effect;
    item->setGraphicsEffect(0);
    QVERIFY(!ptr);
    QTest::qWait(50);
    QCOMPARE(item->numRepaints, 1);
}

void tst_QGraphicsEffect::opacity()
{
    // Make sure the painter's opacity is correct in QGraphicsEffect::draw.
    QGraphicsScene scene;
    CustomItem *item = new CustomItem(0, 0, 100, 100);
    item->setOpacity(0.5);
    CustomEffect *effect = new CustomEffect;
    item->setGraphicsEffect(effect);
    scene.addItem(item);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);
    QTRY_VERIFY(effect->numRepaints > 0);
    QCOMPARE(effect->m_opacity, qreal(0.5));
}

void tst_QGraphicsEffect::grayscale()
{
    if (qApp->desktop()->depth() < 24) {
        QSKIP("Test only works on 32 bit displays", SkipAll);
        return;
    }

    QGraphicsScene scene(0, 0, 100, 100);

    QGraphicsRectItem *item = scene.addRect(0, 0, 50, 50);
    item->setPen(Qt::NoPen);
    item->setBrush(QColor(122, 193, 66)); // Qt light green

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
    effect->setColor(Qt::black);
    item->setGraphicsEffect(effect);

    QPainter painter;
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);

    image.fill(0);
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(10, 10), qRgb(148, 148, 148));

    effect->setStrength(0.5);

    image.fill(0);
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(10, 10), qRgb(135, 171, 107));

    effect->setStrength(0.0);

    image.fill(0);
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(10, 10), qRgb(122, 193, 66));
}

void tst_QGraphicsEffect::colorize()
{
    if (qApp->desktop()->depth() < 24) {
        QSKIP("Test only works on 32 bit displays", SkipAll);
        return;
    }

    QGraphicsScene scene(0, 0, 100, 100);

    QGraphicsRectItem *item = scene.addRect(0, 0, 50, 50);
    item->setPen(Qt::NoPen);
    item->setBrush(QColor(122, 193, 66)); // Qt light green

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect;
    effect->setColor(QColor(102, 153, 51)); // Qt dark green
    item->setGraphicsEffect(effect);

    QPainter painter;
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);

    image.fill(0);
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(10, 10), qRgb(191, 212, 169));

    effect->setStrength(0.5);

    image.fill(0);
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(10, 10), qRgb(156, 203, 117));

    effect->setStrength(0.0);

    image.fill(0);
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(10, 10), qRgb(122, 193, 66));
}

class PixmapItemEffect : public QGraphicsEffect
{
public:
    PixmapItemEffect(const QPixmap &source)
        : QGraphicsEffect()
        , pixmap(source)
        , repaints(0)
    {}

    QRectF boundingRectFor(const QRectF &rect) const
    { return rect; }

    void draw(QPainter *painter)
    {
        QVERIFY(sourcePixmap(Qt::LogicalCoordinates).pixmapData() == pixmap.pixmapData());
        QVERIFY((painter->worldTransform().type() <= QTransform::TxTranslate) == (sourcePixmap(Qt::DeviceCoordinates).pixmapData() == pixmap.pixmapData()));

        ++repaints;
    }
    QPixmap pixmap;
    int repaints;
};

void tst_QGraphicsEffect::drawPixmapItem()
{
    QImage image(32, 32, QImage::Format_RGB32);
    QPainter p(&image);
    p.fillRect(0, 0, 32, 16, Qt::blue);
    p.fillRect(0, 16, 32, 16, Qt::red);
    p.end();

    QGraphicsScene scene;
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene.addItem(item);

    PixmapItemEffect *effect = new PixmapItemEffect(item->pixmap());
    item->setGraphicsEffect(effect);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    item->rotate(180);
    QTest::qWait(50);

    QTRY_VERIFY(effect->repaints >= 2);
}

class DeviceEffect : public QGraphicsEffect
{
public:
    QRectF boundingRectFor(const QRectF &rect) const
    { return rect; }

    void draw(QPainter *painter)
    {
        QPoint offset;
        QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, QGraphicsEffect::NoPad);

        if (pixmap.isNull())
            return;

        painter->save();
        painter->setWorldTransform(QTransform());
        painter->drawPixmap(offset, pixmap);
        painter->restore();
    }
};

void tst_QGraphicsEffect::deviceCoordinateTranslateCaching()
{
    QGraphicsScene scene;
    CustomItem *item = new CustomItem(0, 0, 10, 10);
    scene.addItem(item);
    scene.setSceneRect(0, 0, 50, 0);

    item->setGraphicsEffect(new DeviceEffect);
    item->setPen(Qt::NoPen);
    item->setBrush(Qt::red);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QTRY_VERIFY(item->numRepaints >= 1);
    int numRepaints = item->numRepaints;

    item->translate(10, 0);
    QTest::qWait(50);

    QVERIFY(item->numRepaints == numRepaints);
}

void tst_QGraphicsEffect::inheritOpacity()
{
    QGraphicsScene scene;
    QGraphicsRectItem *rectItem = new QGraphicsRectItem(0, 0, 10, 10);
    CustomItem *item = new CustomItem(0, 0, 10, 10, rectItem);

    scene.addItem(rectItem);

    item->setGraphicsEffect(new DeviceEffect);
    item->setPen(Qt::NoPen);
    item->setBrush(Qt::red);

    rectItem->setOpacity(0.5);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWaitForWindowShown(&view);

    QTRY_VERIFY(item->numRepaints >= 1);

    int numRepaints = item->numRepaints;

    rectItem->setOpacity(1);
    QTest::qWait(50);

    // item should have been rerendered due to opacity changing
    QTRY_VERIFY(item->numRepaints > numRepaints);
}

QTEST_MAIN(tst_QGraphicsEffect)
#include "tst_qgraphicseffect.moc"

