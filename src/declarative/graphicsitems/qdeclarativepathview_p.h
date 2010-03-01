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

#ifndef QDECLARATIVEPATHVIEW_H
#define QDECLARATIVEPATHVIEW_H

#include "qdeclarativeitem.h"
#include "qdeclarativepath_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativePathViewPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativePathView : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QDeclarativePath *path READ path WRITE setPath)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal snapPosition READ snapPosition WRITE setSnapPosition)
    Q_PROPERTY(qreal dragMargin READ dragMargin WRITE setDragMargin)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(int pathItemCount READ pathItemCount WRITE setPathItemCount)

public:
    QDeclarativePathView(QDeclarativeItem *parent=0);
    virtual ~QDeclarativePathView();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativePath *path() const;
    void setPath(QDeclarativePath *);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    qreal offset() const;
    void setOffset(qreal offset);

    qreal snapPosition() const;
    void setSnapPosition(qreal pos);

    qreal dragMargin() const;
    void setDragMargin(qreal margin);

    int count() const;

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int pathItemCount() const;
    void setPathItemCount(int);

    static QObject *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void currentIndexChanged();
    void offsetChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);
    bool sceneEventFilter(QGraphicsItem *, QEvent *);
    void componentComplete();

private Q_SLOTS:
    void refill();
    void ticked();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void modelReset();
    void createdItem(int index, QDeclarativeItem *item);
    void destroyingItem(QDeclarativeItem *item);

private:
    friend class QDeclarativePathViewAttached;
    static QHash<QObject*, QObject*> attachedProperties;
    Q_DISABLE_COPY(QDeclarativePathView)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativePathView)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePathView)
QML_DECLARE_TYPEINFO(QDeclarativePathView, QML_HAS_ATTACHED_PROPERTIES)
QT_END_HEADER

#endif // QDECLARATIVEPATHVIEW_H
