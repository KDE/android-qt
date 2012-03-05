/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QDebug>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "listitem.h"
#include "theme.h"

struct ItemData
{
    QHash<ListItem::TextPos, QString> texts;
    QHash<ListItem::TextPos, QFont> fonts;
    QHash<ListItem::IconItemPos, QString> icons;
    QHash<ListItem::IconItemPos, qreal> iconRotations;
    QHash<ListItem::IconItemPos, bool> iconSmoothTransformations;
    QHash<ListItem::IconItemPos, bool> iconOpacityEffets;
    QPen borderPen;
    QBrush backgroundBrush;
    qreal backgroundOpacity;
    QSize rounding;
};
Q_DECLARE_METATYPE(ItemData);

ListItem::ListItem(QGraphicsWidget *parent)
  : GvbWidget(parent),
    m_txtlayout(new QGraphicsGridLayout()),
    m_layout(new QGraphicsLinearLayout(Qt::Horizontal)),
    m_liconlayout(new QGraphicsLinearLayout(Qt::Horizontal)),
    m_riconlayout(new QGraphicsLinearLayout(Qt::Horizontal))
    ,m_fonts()
    ,m_borderPen(Qt::NoPen)
    ,m_backgroundBrush(QBrush())
    ,m_backgroundOpacity(1.0)
    ,m_rounding(0.0, 0.0)
{ 
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setContentsMargins(0,4,4,0);
    m_layout->setContentsMargins(0,0,0,0); 

    m_txtlayout->setContentsMargins(0,8,0,8);
    m_liconlayout->setContentsMargins(8,8,8,8);
    m_riconlayout->setContentsMargins(0,8,4,8);
    
    m_layout->insertItem(0, m_liconlayout);
    m_layout->insertItem(1, m_txtlayout);
    m_layout->insertItem(2, m_riconlayout);

    m_layout->setStretchFactor(m_liconlayout, 1);
    m_layout->setStretchFactor(m_txtlayout, 5);
    m_layout->setStretchFactor(m_riconlayout, 1);

    setFlag(QGraphicsItem::ItemClipsToShape);
    setLayout(m_layout);
}

ListItem::~ListItem() 
{
    if ( !m_liconlayout->parentLayoutItem() )
        delete m_liconlayout;

    if ( !m_riconlayout->parentLayoutItem() )
        delete m_riconlayout;
}

void ListItem::setIcon( IconItem *iconItem, const IconItemPos iconPos )
{
    if (iconPos == LeftIcon) {
        if (m_liconlayout->count() > 0 && m_liconlayout->itemAt(0)) {
            delete m_liconlayout->itemAt(0);
            m_liconlayout->addItem( iconItem );
        }
        else {
            m_liconlayout->addItem( iconItem );
        }
        m_liconlayout->itemAt(0)->setMaximumSize(58,58);
    }
    else if (iconPos == RightIcon) {
        if (m_riconlayout->count() > 0 && m_riconlayout->itemAt(0)) {
            delete m_riconlayout->itemAt(0);
            m_riconlayout->addItem( iconItem );
        }
        else {
            m_riconlayout->addItem( iconItem );
        }
        m_riconlayout->itemAt(0)->setMaximumSize(22,22);
    }
    m_layout->invalidate();
}

IconItem* ListItem::icon( const IconItemPos iconPos )  const
{
    QGraphicsLayoutItem* item = 0;

    if (iconPos == LeftIcon && m_liconlayout->count() > 0) {
        item = m_liconlayout->itemAt(0);
    } 
    else if (iconPos == RightIcon && m_riconlayout->count() > 0) {
        item = m_riconlayout->itemAt(0);
    }

    if (item) {
        IconItem* titem = static_cast<IconItem *>(item);
        return titem;
    }
    return 0;
}

QVariant ListItem::data(int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    ItemData data;

    if (text(ListItem::FirstPos).size() > 0) {
        data.texts[ListItem::FirstPos] = text(ListItem::FirstPos);
        data.fonts[ListItem::FirstPos] = m_fonts[ListItem::FirstPos];
    }
    if (text(ListItem::SecondPos).size() > 0) {
        data.texts[ListItem::SecondPos] = text(ListItem::SecondPos);
        data.fonts[ListItem::SecondPos] = m_fonts[ListItem::SecondPos];
    }
    if (text(ListItem::ThirdPos).size() > 0) {
        data.texts[ListItem::ThirdPos] = text(ListItem::ThirdPos);
        data.fonts[ListItem::ThirdPos] = m_fonts[ListItem::ThirdPos];
    }
    if (text(ListItem::LastPos).size() > 0) {
        data.texts[ListItem::LastPos] = text(ListItem::LastPos);
        data.fonts[ListItem::LastPos] = m_fonts[ListItem::LastPos];
    }

    if (icon(ListItem::LeftIcon)) {
        data.icons[ListItem::LeftIcon] = icon(ListItem::LeftIcon)->fileName();
        data.iconRotations[ListItem::LeftIcon] = icon(ListItem::LeftIcon)->rotation();
        data.iconSmoothTransformations[ListItem::LeftIcon] = icon(ListItem::LeftIcon)->isSmoothTransformationEnabled();
#if (QT_VERSION >= 0x040600)
        data.iconOpacityEffets[ListItem::LeftIcon] = icon(ListItem::LeftIcon)->isOpacityEffectEnabled();
#endif
    }

    if (icon(ListItem::RightIcon)) {
        data.icons[ListItem::RightIcon] = icon(ListItem::RightIcon)->fileName();
        data.iconRotations[ListItem::RightIcon] = icon(ListItem::RightIcon)->rotation();
        data.iconSmoothTransformations[ListItem::RightIcon] = icon(ListItem::RightIcon)->isSmoothTransformationEnabled();
#if (QT_VERSION >= 0x040600)
        data.iconOpacityEffets[ListItem::RightIcon] = icon(ListItem::RightIcon)->isOpacityEffectEnabled();
#endif
    }

    data.borderPen = m_borderPen;
    data.backgroundBrush = m_backgroundBrush;
    data.backgroundOpacity = m_backgroundOpacity;
    data.rounding = m_rounding;

    QVariant var;
    var.setValue(data);
    return var;
}

void ListItem::setData(const QVariant &value, int role)
{
    if (role != Qt::DisplayRole)
        return;

    ItemData data = value.value<ItemData>();
    QList<ListItem::TextPos> textkeys = data.texts.keys();

    for( int i = 0; i<textkeys.count(); ++i) {
        setText(data.texts[textkeys.at(i)],textkeys.at(i));
        setFont(data.fonts[textkeys.at(i)], textkeys.at(i));
    }

    QList<ListItem::IconItemPos> iconkeys = data.icons.keys();
    for( int i = 0; i<iconkeys.count(); ++i) {
        IconItem *iconItem = icon(iconkeys.at(i));
        if (iconItem)
            iconItem->setFileName(data.icons[iconkeys.at(i)]);
        else {
            IconItem *iconItem = new IconItem(data.icons[iconkeys.at(i)], this);
            setIcon(iconItem, iconkeys.at(i));
        }
    }

    if (icon(ListItem::LeftIcon)) {
        icon(ListItem::LeftIcon)->setRotation(data.iconRotations[ListItem::LeftIcon]);
        icon(ListItem::LeftIcon)->setSmoothTransformationEnabled(data.iconSmoothTransformations[ListItem::LeftIcon]);
#if (QT_VERSION >= 0x040600)
        icon(ListItem::LeftIcon)->setOpacityEffectEnabled(data.iconOpacityEffets[ListItem::LeftIcon]);
#endif
    }

    if (icon(ListItem::RightIcon)) {
        icon(ListItem::RightIcon)->setRotation(data.iconRotations[ListItem::RightIcon]);
        icon(ListItem::RightIcon)->setSmoothTransformationEnabled(data.iconSmoothTransformations[ListItem::RightIcon]);
#if (QT_VERSION >= 0x040600)
        icon(ListItem::RightIcon)->setOpacityEffectEnabled(data.iconOpacityEffets[ListItem::RightIcon]);
#endif
    }

    m_borderPen = data.borderPen;
    m_backgroundBrush = data.backgroundBrush;
    m_backgroundOpacity = data.backgroundOpacity;
    m_rounding = data.rounding;
}

void ListItem::setText(const QString str, const TextPos position)
{
    QGraphicsLayoutItem * item = 0;

    if (m_txtlayout->rowCount() > position && position >= 0)
        item = m_txtlayout->itemAt(position, 0);

    if (!item) {
        Label *label = new Label(str,this);
        m_txtlayout->addItem(label, position, 0);
        if (m_fonts.contains(position))
            label->setFont(m_fonts[position]);
    }
    else {
        Label *titem = static_cast<Label *>(item);
        titem->setText(str);
    }
}

void ListItem::setFont(const QFont font, const TextPos position)
{
    m_fonts.insert(position, font);
    QGraphicsLayoutItem * item = 0;

    if (m_txtlayout->rowCount() > position && position >= 0)
        item = m_txtlayout->itemAt(position, 0);

    if (item) {
        Label *titem = static_cast<Label *>(item);
        titem->setFont(font);
    }
}

QString ListItem::text(const TextPos position) const
{
    QGraphicsLayoutItem * item = 0;

    if (m_txtlayout->rowCount() > position && position >= 0)
        item = m_txtlayout->itemAt(position, 0);

    if (item) {
        Label *titem = static_cast<Label *>(item);
        return titem->text();
    }
    return "";
}

void ListItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    const int penWidth = m_borderPen.width();
    QRectF r = rect();
    r.adjust(penWidth, penWidth, -penWidth, -penWidth);

    if (m_borderPen != Qt::NoPen)
    {
        painter->setPen(m_borderPen);
        painter->drawRoundedRect(r, m_rounding.width(), m_rounding.height());
    }

    if (m_backgroundBrush != Qt::NoBrush)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_backgroundBrush);
        painter->setOpacity(m_backgroundOpacity);
        painter->drawRoundedRect(r, m_rounding.width(), m_rounding.height());
    }
}
