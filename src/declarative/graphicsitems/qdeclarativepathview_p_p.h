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

#ifndef QDECLARATIVEPATHVIEW_P_H
#define QDECLARATIVEPATHVIEW_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarativepathview_p.h"

#include "qdeclarativeitem_p.h"
#include "qdeclarativevisualitemmodel_p.h"

#include <qdeclarative.h>
#include <qdeclarativeanimation_p_p.h>

#include <qdatetime.h>

QT_BEGIN_NAMESPACE

typedef struct PathViewItem{
    int index;
    QDeclarativeItem* item;
}PathViewItem;

class QDeclarativePathViewPrivate : public QDeclarativeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePathView)

public:
    QDeclarativePathViewPrivate()
      : path(0), currentIndex(0), startPc(0), lastDist(0)
        , lastElapsed(0), stealMouse(false), ownModel(false), activeItem(0)
        , snapPos(0), dragMargin(0), moveOffset(this, &QDeclarativePathViewPrivate::setOffset)
        , firstIndex(0), pathItems(-1), pathOffset(0), requestedIndex(-1)
        , moveReason(Other)
    {
    }

    void init()
    {
        Q_Q(QDeclarativePathView);
        _offset = 0;
        q->setAcceptedMouseButtons(Qt::LeftButton);
        q->setFlag(QGraphicsItem::ItemIsFocusScope);
        q->setFiltersChildEvents(true);
        q->connect(&tl, SIGNAL(updated()), q, SLOT(ticked()));
    }

    QDeclarativeItem *getItem(int modelIndex);
    void releaseItem(QDeclarativeItem *item);

    bool isValid() const {
        return model && model->count() > 0 && model->isValid() && path;
    }

    int calcCurrentIndex();
    void updateCurrent();
    static void fixOffsetCallback(void*);
    void fixOffset();
    void setOffset(qreal offset);
    void regenerate();
    void updateItem(QDeclarativeItem *, qreal);
    void snapToCurrent();
    QPointF pointNear(const QPointF &point, qreal *nearPercent=0) const;

    QDeclarativePath *path;
    int currentIndex;
    qreal startPc;
    QPointF startPoint;
    qreal lastDist;
    int lastElapsed;
    qreal _offset;
    bool stealMouse : 1;
    bool ownModel : 1;
    QTime lastPosTime;
    QPointF lastPos;
    QDeclarativeItem *activeItem;
    qreal snapPos;
    qreal dragMargin;
    QDeclarativeTimeLine tl;
    QDeclarativeTimeLineValueProxy<QDeclarativePathViewPrivate> moveOffset;
    int firstIndex;
    int pathItems;
    int pathOffset;
    int requestedIndex;
    QList<QDeclarativeItem *> items;
    QGuard<QDeclarativeVisualModel> model;
    QVariant modelVariant;
    enum MovementReason { Other, Key, Mouse };
    MovementReason moveReason;
};

QT_END_NAMESPACE

#endif
