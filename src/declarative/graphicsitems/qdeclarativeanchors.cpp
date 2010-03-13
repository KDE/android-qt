/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativeanchors_p_p.h"

#include "qdeclarativeitem.h"
#include "qdeclarativeitem_p.h"

#include <qdeclarativeinfo.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

//TODO: should we cache relationships, so we don't have to check each time (parent-child or sibling)?
//TODO: support non-parent, non-sibling (need to find lowest common ancestor)

//### const item?
//local position
static qreal position(QDeclarativeItem *item, QDeclarativeAnchorLine::AnchorLine anchorLine)
{
    qreal ret = 0.0;
    switch(anchorLine) {
    case QDeclarativeAnchorLine::Left:
        ret = item->x();
        break;
    case QDeclarativeAnchorLine::Right:
        ret = item->x() + item->width();
        break;
    case QDeclarativeAnchorLine::Top:
        ret = item->y();
        break;
    case QDeclarativeAnchorLine::Bottom:
        ret = item->y() + item->height();
        break;
    case QDeclarativeAnchorLine::HCenter:
        ret = item->x() + item->width()/2;
        break;
    case QDeclarativeAnchorLine::VCenter:
        ret = item->y() + item->height()/2;
        break;
    case QDeclarativeAnchorLine::Baseline:
        ret = item->y() + item->baselineOffset();
        break;
    default:
        break;
    }

    return ret;
}

//position when origin is 0,0
static qreal adjustedPosition(QDeclarativeItem *item, QDeclarativeAnchorLine::AnchorLine anchorLine)
{
    int ret = 0;
    switch(anchorLine) {
    case QDeclarativeAnchorLine::Left:
        ret = 0;
        break;
    case QDeclarativeAnchorLine::Right:
        ret = item->width();
        break;
    case QDeclarativeAnchorLine::Top:
        ret = 0;
        break;
    case QDeclarativeAnchorLine::Bottom:
        ret = item->height();
        break;
    case QDeclarativeAnchorLine::HCenter:
        ret = item->width()/2;
        break;
    case QDeclarativeAnchorLine::VCenter:
        ret = item->height()/2;
        break;
    case QDeclarativeAnchorLine::Baseline:
        ret = item->baselineOffset();
        break;
    default:
        break;
    }

    return ret;
}

/*!
    \internal
    \class QDeclarativeAnchors
    \since 4.7
    \ingroup group_layouts
    \brief The QDeclarativeAnchors class provides a way to lay out items relative to other items.

    \warning Currently, only anchoring to siblings or parent is supported.
*/

QDeclarativeAnchors::QDeclarativeAnchors(QObject *parent)
  : QObject(*new QDeclarativeAnchorsPrivate(0), parent)
{
    qFatal("QDeclarativeAnchors::QDeclarativeAnchors(QObject*) called");
}

QDeclarativeAnchors::QDeclarativeAnchors(QDeclarativeItem *item, QObject *parent)
  : QObject(*new QDeclarativeAnchorsPrivate(item), parent)
{
}

QDeclarativeAnchors::~QDeclarativeAnchors()
{
    Q_D(QDeclarativeAnchors);
    d->remDepend(d->fill);
    d->remDepend(d->centerIn);
    d->remDepend(d->left.item);
    d->remDepend(d->right.item);
    d->remDepend(d->top.item);
    d->remDepend(d->bottom.item);
    d->remDepend(d->vCenter.item);
    d->remDepend(d->hCenter.item);
    d->remDepend(d->baseline.item);
}

void QDeclarativeAnchorsPrivate::fillChanged()
{
    if (!fill || !isItemComplete())
        return;

    if (updatingFill < 2) {
        ++updatingFill;

        if (fill == item->parentItem()) {                         //child-parent
            setItemPos(QPointF(leftMargin, topMargin));
        } else if (fill->parentItem() == item->parentItem()) {   //siblings
            setItemPos(QPointF(fill->x()+leftMargin, fill->y()+topMargin));
        }
        setItemSize(QSizeF(fill->width()-leftMargin-rightMargin, fill->height()-topMargin-bottomMargin));

        --updatingFill;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QDeclarativeAnchors::tr("Possible anchor loop detected on fill.");
    }

}

void QDeclarativeAnchorsPrivate::centerInChanged()
{
    if (!centerIn || fill || !isItemComplete())
        return;

    if (updatingCenterIn < 2) {
        ++updatingCenterIn;

        if (centerIn == item->parentItem()) {
            QPointF p((item->parentItem()->width() - item->width()) / 2. + hCenterOffset,
                      (item->parentItem()->height() - item->height()) / 2. + vCenterOffset);
            setItemPos(p);

        } else if (centerIn->parentItem() == item->parentItem()) {

            QPointF p(centerIn->x() + (centerIn->width() - item->width()) / 2. + hCenterOffset,
                      centerIn->y() + (centerIn->height() - item->height()) / 2. + vCenterOffset);
            setItemPos(p);
        }

        --updatingCenterIn;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QDeclarativeAnchors::tr("Possible anchor loop detected on centerIn.");
    }
}

void QDeclarativeAnchorsPrivate::clearItem(QDeclarativeItem *item)
{
    if (!item)
        return;
    if (fill == item)
        fill = 0;
    if (centerIn == item)
        centerIn = 0;
    if (left.item == item) {
        left.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasLeftAnchor;
    }
    if (right.item == item) {
        right.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasRightAnchor;
    }
    if (top.item == item) {
        top.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasTopAnchor;
    }
    if (bottom.item == item) {
        bottom.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasBottomAnchor;
    }
    if (vCenter.item == item) {
        vCenter.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasVCenterAnchor;
    }
    if (hCenter.item == item) {
        hCenter.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasHCenterAnchor;
    }
    if (baseline.item == item) {
        baseline.item = 0;
        usedAnchors &= ~QDeclarativeAnchors::HasBaselineAnchor;
    }
}

void QDeclarativeAnchorsPrivate::addDepend(QDeclarativeItem *item)
{
    if (!item)
        return;
    QDeclarativeItemPrivate *p =
        static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(item));
    p->addItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
}

void QDeclarativeAnchorsPrivate::remDepend(QDeclarativeItem *item)
{
    if (!item)
        return;
    QDeclarativeItemPrivate *p =
        static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(item));
    p->removeItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
}

bool QDeclarativeAnchorsPrivate::isItemComplete() const
{
    return componentComplete;
}

void QDeclarativeAnchors::classBegin()
{
    Q_D(QDeclarativeAnchors);
    d->componentComplete = false;
}

void QDeclarativeAnchors::componentComplete()
{
    Q_D(QDeclarativeAnchors);
    d->componentComplete = true;
}

void QDeclarativeAnchorsPrivate::setItemHeight(qreal v)
{
    updatingMe = true;
    item->setHeight(v);
    updatingMe = false;
}

void QDeclarativeAnchorsPrivate::setItemWidth(qreal v)
{
    updatingMe = true;
    item->setWidth(v);
    updatingMe = false;
}

void QDeclarativeAnchorsPrivate::setItemX(qreal v)
{
    updatingMe = true;
    item->setX(v);
    updatingMe = false;
}

void QDeclarativeAnchorsPrivate::setItemY(qreal v)
{
    updatingMe = true;
    item->setY(v);
    updatingMe = false;
}

void QDeclarativeAnchorsPrivate::setItemPos(const QPointF &v)
{
    updatingMe = true;
    item->setPos(v);
    updatingMe = false;
}

void QDeclarativeAnchorsPrivate::setItemSize(const QSizeF &v)
{
    updatingMe = true;
    item->setSize(v);
    updatingMe = false;
}

void QDeclarativeAnchorsPrivate::updateMe()
{
    if (updatingMe) {
        updatingMe = false;
        return;
    }

    fillChanged();
    centerInChanged();
    updateHorizontalAnchors();
    updateVerticalAnchors();
}

void QDeclarativeAnchorsPrivate::updateOnComplete()
{
    fillChanged();
    centerInChanged();
    updateHorizontalAnchors();
    updateVerticalAnchors();
}

void QDeclarativeAnchorsPrivate::itemGeometryChanged(QDeclarativeItem *, const QRectF &newG, const QRectF &oldG)
{
    fillChanged();
    centerInChanged();

    if (newG.x() != oldG.x() || newG.width() != oldG.width())
        updateHorizontalAnchors();
    if (newG.y() != oldG.y() || newG.height() != oldG.height())
        updateVerticalAnchors();
}

QDeclarativeItem *QDeclarativeAnchors::fill() const
{
    Q_D(const QDeclarativeAnchors);
    return d->fill;
}

void QDeclarativeAnchors::setFill(QDeclarativeItem *f)
{
    Q_D(QDeclarativeAnchors);
    if (d->fill == f)
        return;

    if (!f) {
        d->remDepend(d->fill);
        d->fill = f;
        emit fillChanged();
        return;
    }
    if (f != d->item->parentItem() && f->parentItem() != d->item->parentItem()){
        qmlInfo(d->item) << tr("Cannot anchor to an item that isn't a parent or sibling.");
        return;
    }
    d->remDepend(d->fill);
    d->fill = f;
    d->addDepend(d->fill);
    emit fillChanged();
    d->fillChanged();
}

void QDeclarativeAnchors::resetFill()
{
    setFill(0);
}

QDeclarativeItem *QDeclarativeAnchors::centerIn() const
{
    Q_D(const QDeclarativeAnchors);
    return d->centerIn;
}

void QDeclarativeAnchors::setCenterIn(QDeclarativeItem* c)
{
    Q_D(QDeclarativeAnchors);
    if (d->centerIn == c)
        return;

    if (!c) {
        d->remDepend(d->centerIn);
        d->centerIn = c;
        emit centerInChanged();
        return;
    }
    if (c != d->item->parentItem() && c->parentItem() != d->item->parentItem()){
        qmlInfo(d->item) << tr("Cannot anchor to an item that isn't a parent or sibling.");
        return;
    }

    d->remDepend(d->centerIn);
    d->centerIn = c;
    d->addDepend(d->centerIn);
    emit centerInChanged();
    d->centerInChanged();
}

void QDeclarativeAnchors::resetCenterIn()
{
    setCenterIn(0);
}

bool QDeclarativeAnchorsPrivate::calcStretch(const QDeclarativeAnchorLine &edge1,
                                    const QDeclarativeAnchorLine &edge2,
                                    int offset1,
                                    int offset2,
                                    QDeclarativeAnchorLine::AnchorLine line,
                                    int &stretch)
{
    bool edge1IsParent = (edge1.item == item->parentItem());
    bool edge2IsParent = (edge2.item == item->parentItem());
    bool edge1IsSibling = (edge1.item->parentItem() == item->parentItem());
    bool edge2IsSibling = (edge2.item->parentItem() == item->parentItem());

    bool invalid = false;
    if ((edge2IsParent && edge1IsParent) || (edge2IsSibling && edge1IsSibling)) {
        stretch = ((int)position(edge2.item, edge2.anchorLine) + offset2)
                    - ((int)position(edge1.item, edge1.anchorLine) + offset1);
    } else if (edge2IsParent && edge1IsSibling) {
        stretch = ((int)position(edge2.item, edge2.anchorLine) + offset2)
                    - ((int)position(item->parentItem(), line)
                    + (int)position(edge1.item, edge1.anchorLine) + offset1);
    } else if (edge2IsSibling && edge1IsParent) {
        stretch = ((int)position(item->parentItem(), line) + (int)position(edge2.item, edge2.anchorLine) + offset2)
                    - ((int)position(edge1.item, edge1.anchorLine) + offset1);
    } else
        invalid = true;

    return invalid;
}

void QDeclarativeAnchorsPrivate::updateVerticalAnchors()
{
    if (fill || centerIn || !isItemComplete())
        return;

    if (updatingVerticalAnchor < 2) {
        ++updatingVerticalAnchor;
        if (usedAnchors & QDeclarativeAnchors::HasTopAnchor) {
            //Handle stretching
            bool invalid = true;
            int height = 0;
            if (usedAnchors & QDeclarativeAnchors::HasBottomAnchor) {
                invalid = calcStretch(top, bottom, topMargin, -bottomMargin, QDeclarativeAnchorLine::Top, height);
            } else if (usedAnchors & QDeclarativeAnchors::HasVCenterAnchor) {
                invalid = calcStretch(top, vCenter, topMargin, vCenterOffset, QDeclarativeAnchorLine::Top, height);
                height *= 2;
            }
            if (!invalid)
                setItemHeight(height);

            //Handle top
            if (top.item == item->parentItem()) {
                setItemY(adjustedPosition(top.item, top.anchorLine) + topMargin);
            } else if (top.item->parentItem() == item->parentItem()) {
                setItemY(position(top.item, top.anchorLine) + topMargin);
            }
        } else if (usedAnchors & QDeclarativeAnchors::HasBottomAnchor) {
            //Handle stretching (top + bottom case is handled above)
            if (usedAnchors & QDeclarativeAnchors::HasVCenterAnchor) {
                int height = 0;
                bool invalid = calcStretch(vCenter, bottom, vCenterOffset, -bottomMargin,
                                              QDeclarativeAnchorLine::Top, height);
                if (!invalid)
                    setItemHeight(height*2);
            }

            //Handle bottom
            if (bottom.item == item->parentItem()) {
                setItemY(adjustedPosition(bottom.item, bottom.anchorLine) - item->height() - bottomMargin);
            } else if (bottom.item->parentItem() == item->parentItem()) {
                setItemY(position(bottom.item, bottom.anchorLine) - item->height() - bottomMargin);
            }
        } else if (usedAnchors & QDeclarativeAnchors::HasVCenterAnchor) {
            //(stetching handled above)

            //Handle vCenter
            if (vCenter.item == item->parentItem()) {
                setItemY(adjustedPosition(vCenter.item, vCenter.anchorLine)
                              - item->height()/2 + vCenterOffset);
            } else if (vCenter.item->parentItem() == item->parentItem()) {
                setItemY(position(vCenter.item, vCenter.anchorLine) - item->height()/2 + vCenterOffset);
            }
        } else if (usedAnchors & QDeclarativeAnchors::HasBaselineAnchor) {
            //Handle baseline
            if (baseline.item == item->parentItem()) {
                setItemY(adjustedPosition(baseline.item, baseline.anchorLine)
                        - item->baselineOffset() + baselineOffset);
            } else if (baseline.item->parentItem() == item->parentItem()) {
                setItemY(position(baseline.item, baseline.anchorLine)
                        - item->baselineOffset() + baselineOffset);
            }
        }
        --updatingVerticalAnchor;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QDeclarativeAnchors::tr("Possible anchor loop detected on vertical anchor.");
    }
}

void QDeclarativeAnchorsPrivate::updateHorizontalAnchors()
{
    if (fill || centerIn || !isItemComplete())
        return;

    if (updatingHorizontalAnchor < 2) {
        ++updatingHorizontalAnchor;

        if (usedAnchors & QDeclarativeAnchors::HasLeftAnchor) {
            //Handle stretching
            bool invalid = true;
            int width = 0;
            if (usedAnchors & QDeclarativeAnchors::HasRightAnchor) {
                invalid = calcStretch(left, right, leftMargin, -rightMargin, QDeclarativeAnchorLine::Left, width);
            } else if (usedAnchors & QDeclarativeAnchors::HasHCenterAnchor) {
                invalid = calcStretch(left, hCenter, leftMargin, hCenterOffset, QDeclarativeAnchorLine::Left, width);
                width *= 2;
            }
            if (!invalid)
                setItemWidth(width);

            //Handle left
            if (left.item == item->parentItem()) {
                setItemX(adjustedPosition(left.item, left.anchorLine) + leftMargin);
            } else if (left.item->parentItem() == item->parentItem()) {
                setItemX(position(left.item, left.anchorLine) + leftMargin);
            }
        } else if (usedAnchors & QDeclarativeAnchors::HasRightAnchor) {
            //Handle stretching (left + right case is handled in updateLeftAnchor)
            if (usedAnchors & QDeclarativeAnchors::HasHCenterAnchor) {
                int width = 0;
                bool invalid = calcStretch(hCenter, right, hCenterOffset, -rightMargin,
                                              QDeclarativeAnchorLine::Left, width);
                if (!invalid)
                    setItemWidth(width*2);
            }

            //Handle right
            if (right.item == item->parentItem()) {
                setItemX(adjustedPosition(right.item, right.anchorLine) - item->width() - rightMargin);
            } else if (right.item->parentItem() == item->parentItem()) {
                setItemX(position(right.item, right.anchorLine) - item->width() - rightMargin);
            }
        } else if (usedAnchors & QDeclarativeAnchors::HasHCenterAnchor) {
            //Handle hCenter
            if (hCenter.item == item->parentItem()) {
                setItemX(adjustedPosition(hCenter.item, hCenter.anchorLine) - item->width()/2 + hCenterOffset);
            } else if (hCenter.item->parentItem() == item->parentItem()) {
                setItemX(position(hCenter.item, hCenter.anchorLine) - item->width()/2 + hCenterOffset);
            }
        }

        --updatingHorizontalAnchor;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QDeclarativeAnchors::tr("Possible anchor loop detected on horizontal anchor.");
    }
}

QDeclarativeAnchorLine QDeclarativeAnchors::top() const
{
    Q_D(const QDeclarativeAnchors);
    return d->top;
}

void QDeclarativeAnchors::setTop(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkVAnchorValid(edge) || d->top == edge)
        return;

    d->usedAnchors |= HasTopAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasTopAnchor;
        return;
    }

    d->remDepend(d->top.item);
    d->top = edge;
    d->addDepend(d->top.item);
    emit topChanged();
    d->updateVerticalAnchors();
}

void QDeclarativeAnchors::resetTop()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasTopAnchor;
    d->remDepend(d->top.item);
    d->top = QDeclarativeAnchorLine();
    emit topChanged();
    d->updateVerticalAnchors();
}

QDeclarativeAnchorLine QDeclarativeAnchors::bottom() const
{
    Q_D(const QDeclarativeAnchors);
    return d->bottom;
}

void QDeclarativeAnchors::setBottom(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkVAnchorValid(edge) || d->bottom == edge)
        return;

    d->usedAnchors |= HasBottomAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasBottomAnchor;
        return;
    }

    d->remDepend(d->bottom.item);
    d->bottom = edge;
    d->addDepend(d->bottom.item);
    emit bottomChanged();
    d->updateVerticalAnchors();
}

void QDeclarativeAnchors::resetBottom()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasBottomAnchor;
    d->remDepend(d->bottom.item);
    d->bottom = QDeclarativeAnchorLine();
    emit bottomChanged();
    d->updateVerticalAnchors();
}

QDeclarativeAnchorLine QDeclarativeAnchors::verticalCenter() const
{
    Q_D(const QDeclarativeAnchors);
    return d->vCenter;
}

void QDeclarativeAnchors::setVerticalCenter(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkVAnchorValid(edge) || d->vCenter == edge)
        return;

    d->usedAnchors |= HasVCenterAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasVCenterAnchor;
        return;
    }

    d->remDepend(d->vCenter.item);
    d->vCenter = edge;
    d->addDepend(d->vCenter.item);
    emit verticalCenterChanged();
    d->updateVerticalAnchors();
}

void QDeclarativeAnchors::resetVerticalCenter()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasVCenterAnchor;
    d->remDepend(d->vCenter.item);
    d->vCenter = QDeclarativeAnchorLine();
    emit verticalCenterChanged();
    d->updateVerticalAnchors();
}

QDeclarativeAnchorLine QDeclarativeAnchors::baseline() const
{
    Q_D(const QDeclarativeAnchors);
    return d->baseline;
}

void QDeclarativeAnchors::setBaseline(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkVAnchorValid(edge) || d->baseline == edge)
        return;

    d->usedAnchors |= HasBaselineAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasBaselineAnchor;
        return;
    }

    d->remDepend(d->baseline.item);
    d->baseline = edge;
    d->addDepend(d->baseline.item);
    emit baselineChanged();
    d->updateVerticalAnchors();
}

void QDeclarativeAnchors::resetBaseline()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasBaselineAnchor;
    d->remDepend(d->baseline.item);
    d->baseline = QDeclarativeAnchorLine();
    emit baselineChanged();
    d->updateVerticalAnchors();
}

QDeclarativeAnchorLine QDeclarativeAnchors::left() const
{
    Q_D(const QDeclarativeAnchors);
    return d->left;
}

void QDeclarativeAnchors::setLeft(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkHAnchorValid(edge) || d->left == edge)
        return;

    d->usedAnchors |= HasLeftAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HasLeftAnchor;
        return;
    }

    d->remDepend(d->left.item);
    d->left = edge;
    d->addDepend(d->left.item);
    emit leftChanged();
    d->updateHorizontalAnchors();
}

void QDeclarativeAnchors::resetLeft()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasLeftAnchor;
    d->remDepend(d->left.item);
    d->left = QDeclarativeAnchorLine();
    emit leftChanged();
    d->updateHorizontalAnchors();
}

QDeclarativeAnchorLine QDeclarativeAnchors::right() const
{
    Q_D(const QDeclarativeAnchors);
    return d->right;
}

void QDeclarativeAnchors::setRight(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkHAnchorValid(edge) || d->right == edge)
        return;

    d->usedAnchors |= HasRightAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HasRightAnchor;
        return;
    }

    d->remDepend(d->right.item);
    d->right = edge;
    d->addDepend(d->right.item);
    emit rightChanged();
    d->updateHorizontalAnchors();
}

void QDeclarativeAnchors::resetRight()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasRightAnchor;
    d->remDepend(d->right.item);
    d->right = QDeclarativeAnchorLine();
    emit rightChanged();
    d->updateHorizontalAnchors();
}

QDeclarativeAnchorLine QDeclarativeAnchors::horizontalCenter() const
{
    Q_D(const QDeclarativeAnchors);
    return d->hCenter;
}

void QDeclarativeAnchors::setHorizontalCenter(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchors);
    if (!d->checkHAnchorValid(edge) || d->hCenter == edge)
        return;

    d->usedAnchors |= HasHCenterAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HasHCenterAnchor;
        return;
    }

    d->remDepend(d->hCenter.item);
    d->hCenter = edge;
    d->addDepend(d->hCenter.item);
    emit horizontalCenterChanged();
    d->updateHorizontalAnchors();
}

void QDeclarativeAnchors::resetHorizontalCenter()
{
    Q_D(QDeclarativeAnchors);
    d->usedAnchors &= ~HasHCenterAnchor;
    d->remDepend(d->hCenter.item);
    d->hCenter = QDeclarativeAnchorLine();
    emit horizontalCenterChanged();
    d->updateHorizontalAnchors();
}

qreal QDeclarativeAnchors::leftMargin() const
{
    Q_D(const QDeclarativeAnchors);
    return d->leftMargin;
}

void QDeclarativeAnchors::setLeftMargin(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->leftMargin == offset)
        return;
    d->leftMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateHorizontalAnchors();
    emit leftMarginChanged();
}

qreal QDeclarativeAnchors::rightMargin() const
{
    Q_D(const QDeclarativeAnchors);
    return d->rightMargin;
}

void QDeclarativeAnchors::setRightMargin(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->rightMargin == offset)
        return;
    d->rightMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateHorizontalAnchors();
    emit rightMarginChanged();
}

qreal QDeclarativeAnchors::margins() const
{
    Q_D(const QDeclarativeAnchors);
    return d->margins;
}

void QDeclarativeAnchors::setMargins(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->margins == offset)
        return;
    //###Is it significantly faster to set them directly so we can call fillChanged only once?
    if(!d->rightMargin || d->rightMargin == d->margins)
        setRightMargin(offset);
    if(!d->leftMargin || d->leftMargin == d->margins)
        setLeftMargin(offset);
    if(!d->topMargin || d->topMargin == d->margins)
        setTopMargin(offset);
    if(!d->bottomMargin || d->bottomMargin == d->margins)
        setBottomMargin(offset);
    d->margins = offset;
    emit marginsChanged();

}

qreal QDeclarativeAnchors::horizontalCenterOffset() const
{
    Q_D(const QDeclarativeAnchors);
    return d->hCenterOffset;
}

void QDeclarativeAnchors::setHorizontalCenterOffset(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->hCenterOffset == offset)
        return;
    d->hCenterOffset = offset;
    if(d->centerIn)
        d->centerInChanged();
    else
        d->updateHorizontalAnchors();
    emit horizontalCenterOffsetChanged();
}

qreal QDeclarativeAnchors::topMargin() const
{
    Q_D(const QDeclarativeAnchors);
    return d->topMargin;
}

void QDeclarativeAnchors::setTopMargin(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->topMargin == offset)
        return;
    d->topMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateVerticalAnchors();
    emit topMarginChanged();
}

qreal QDeclarativeAnchors::bottomMargin() const
{
    Q_D(const QDeclarativeAnchors);
    return d->bottomMargin;
}

void QDeclarativeAnchors::setBottomMargin(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->bottomMargin == offset)
        return;
    d->bottomMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateVerticalAnchors();
    emit bottomMarginChanged();
}

qreal QDeclarativeAnchors::verticalCenterOffset() const
{
    Q_D(const QDeclarativeAnchors);
    return d->vCenterOffset;
}

void QDeclarativeAnchors::setVerticalCenterOffset(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->vCenterOffset == offset)
        return;
    d->vCenterOffset = offset;
    if(d->centerIn)
        d->centerInChanged();
    else
        d->updateVerticalAnchors();
    emit verticalCenterOffsetChanged();
}

qreal QDeclarativeAnchors::baselineOffset() const
{
    Q_D(const QDeclarativeAnchors);
    return d->baselineOffset;
}

void QDeclarativeAnchors::setBaselineOffset(qreal offset)
{
    Q_D(QDeclarativeAnchors);
    if (d->baselineOffset == offset)
        return;
    d->baselineOffset = offset;
    d->updateVerticalAnchors();
    emit baselineOffsetChanged();
}

QDeclarativeAnchors::UsedAnchors QDeclarativeAnchors::usedAnchors() const
{
    Q_D(const QDeclarativeAnchors);
    return d->usedAnchors;
}

bool QDeclarativeAnchorsPrivate::checkHValid() const
{
    if (usedAnchors & QDeclarativeAnchors::HasLeftAnchor &&
        usedAnchors & QDeclarativeAnchors::HasRightAnchor &&
        usedAnchors & QDeclarativeAnchors::HasHCenterAnchor) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot specify left, right, and hcenter anchors.");
        return false;
    }

    return true;
}

bool QDeclarativeAnchorsPrivate::checkHAnchorValid(QDeclarativeAnchorLine anchor) const
{
    if (!anchor.item) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor to a null item.");
        return false;
    } else if (anchor.anchorLine & QDeclarativeAnchorLine::Vertical_Mask) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor a horizontal edge to a vertical edge.");
        return false;
    } else if (anchor.item != item->parentItem() && anchor.item->parentItem() != item->parentItem()){
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor to an item that isn't a parent or sibling.");
        return false;
    } else if (anchor.item == item) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor item to self.");
        return false;
    }

    return true;
}

bool QDeclarativeAnchorsPrivate::checkVValid() const
{
    if (usedAnchors & QDeclarativeAnchors::HasTopAnchor &&
        usedAnchors & QDeclarativeAnchors::HasBottomAnchor &&
        usedAnchors & QDeclarativeAnchors::HasVCenterAnchor) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot specify top, bottom, and vcenter anchors.");
        return false;
    } else if (usedAnchors & QDeclarativeAnchors::HasBaselineAnchor &&
               (usedAnchors & QDeclarativeAnchors::HasTopAnchor ||
                usedAnchors & QDeclarativeAnchors::HasBottomAnchor ||
                usedAnchors & QDeclarativeAnchors::HasVCenterAnchor)) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Baseline anchor cannot be used in conjunction with top, bottom, or vcenter anchors.");
        return false;
    }

    return true;
}

bool QDeclarativeAnchorsPrivate::checkVAnchorValid(QDeclarativeAnchorLine anchor) const
{
    if (!anchor.item) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor to a null item.");
        return false;
    } else if (anchor.anchorLine & QDeclarativeAnchorLine::Horizontal_Mask) {
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor a vertical edge to a horizontal edge.");
        return false;
    } else if (anchor.item != item->parentItem() && anchor.item->parentItem() != item->parentItem()){
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor to an item that isn't a parent or sibling.");
        return false;
    } else if (anchor.item == item){
        qmlInfo(item) << QDeclarativeAnchors::tr("Cannot anchor item to self.");
        return false;
    }

    return true;
}

#include <moc_qdeclarativeanchors_p.cpp>

QT_END_NAMESPACE

