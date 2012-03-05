/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef LIVESELECTIONTOOL_H
#define LIVESELECTIONTOOL_H

#include "abstractliveedittool.h"
#include "liverubberbandselectionmanipulator.h"
#include "livesingleselectionmanipulator.h"
#include "liveselectionindicator.h"

#include <QtCore/QList>
#include <QtCore/QTime>

QT_FORWARD_DECLARE_CLASS(QGraphicsItem)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)
QT_FORWARD_DECLARE_CLASS(QKeyEvent)
QT_FORWARD_DECLARE_CLASS(QAction)

namespace QmlJSDebugger {

class LiveSelectionTool : public AbstractLiveEditTool
{
    Q_OBJECT

public:
    LiveSelectionTool(QDeclarativeViewInspector* editorView);
    ~LiveSelectionTool();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *) {}
    void hoverMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *) {}
    void wheelEvent(QWheelEvent *event);

    void itemsAboutToRemoved(const QList<QGraphicsItem*> &) {}
//    QVariant itemChange(const QList<QGraphicsItem*> &itemList,
//                        QGraphicsItem::GraphicsItemChange change,
//                        const QVariant &value );

//    void update();

    void clear();

    void selectedItemsChanged(const QList<QGraphicsItem*> &itemList);

    void selectUnderPoint(QMouseEvent *event);

    void setSelectOnlyContentItems(bool selectOnlyContentItems);

    void setRubberbandSelectionMode(bool value);

private slots:
    void contextMenuElementSelected();
    void contextMenuElementHovered(QAction *action);
    void repaintBoundingRects();

private:
    void createContextMenu(const QList<QGraphicsItem*> &itemList, QPoint globalPos);
    LiveSingleSelectionManipulator::SelectionType getSelectionType(Qt::KeyboardModifiers modifiers);
    bool alreadySelected(const QList<QGraphicsItem*> &itemList) const;

private:
    bool m_rubberbandSelectionMode;
    LiveRubberBandSelectionManipulator m_rubberbandSelectionManipulator;
    LiveSingleSelectionManipulator m_singleSelectionManipulator;
    LiveSelectionIndicator m_selectionIndicator;
    //ResizeIndicator m_resizeIndicator;
    QTime m_mousePressTimer;
    bool m_selectOnlyContentItems;

    QList<QWeakPointer<QGraphicsObject> > m_selectedItemList;

    QList<QGraphicsItem*> m_contextMenuItemList;
};

} // namespace QmlJSDebugger

#endif // LIVESELECTIONTOOL_H
