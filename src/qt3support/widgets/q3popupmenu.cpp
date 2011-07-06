/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
#include "q3popupmenu.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3PopupMenu
    \brief The Q3PopupMenu class is a thin compatibility wrapper around QMenu.
    \compat

    Use QMenu in new applications. Note that the menu's actions must
    be \l {Q3Action}s.
*/

/*!
    \fn Q3PopupMenu::Q3PopupMenu(QWidget *parent, const char *name)

    Constructs a menu with the given \a parent and \a name.
*/

/*!
    \fn int Q3PopupMenu::exec()

    Pops up the menu and returns the ID of the action that was
    selected.

    \sa QMenu::exec()
*/

/*!
    \fn int Q3PopupMenu::exec(const QPoint & pos, int indexAtPoint)

    Pops up the menu at coordinate \a pos and returns the ID of the
    action that was selected.

    If \a indexAtPoint is specified, the menu will pop up with the
    item at index \a indexAtPoint under the mouse cursor.

    \sa QMenu::exec()
*/


/*!
    \fn void Q3PopupMenu::setFrameRect(QRect)
    \internal
*/

/*!
    \fn QRect Q3PopupMenu::frameRect() const
    \internal
*/
/*!
    \enum Q3PopupMenu::DummyFrame
    \internal

    \value Box
    \value Sunken
    \value Plain
    \value Raised
    \value MShadow
    \value NoFrame
    \value Panel 
    \value StyledPanel
    \value HLine 
    \value VLine 
    \value GroupBoxPanel
    \value WinPanel 
    \value ToolBarPanel 
    \value MenuBarPanel 
    \value PopupPanel 
    \value LineEditPanel 
    \value TabWidgetPanel 
    \value MShape
*/

/*!
    \fn void Q3PopupMenu::setFrameShadow(DummyFrame)
    \internal
*/

/*!
    \fn DummyFrame Q3PopupMenu::frameShadow() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setFrameShape(DummyFrame)
    \internal
*/

/*!
    \fn DummyFrame Q3PopupMenu::frameShape() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setFrameStyle(int)
    \internal
*/

/*!
    \fn int Q3PopupMenu::frameStyle() const
    \internal
*/

/*!
    \fn int Q3PopupMenu::frameWidth() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setLineWidth(int)
    \internal
*/

/*!
    \fn int Q3PopupMenu::lineWidth() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setMargin(int margin)
    \since 4.2

    Sets the width of the margin around the contents of the widget to \a margin.
    
    This function uses QWidget::setContentsMargins() to set the margin.
    \sa margin(), QWidget::setContentsMargins()
*/

/*!
    \fn int Q3PopupMenu::margin() const 
    \since 4.2

    Returns the width of the margin around the contents of the widget.
    
    This function uses QWidget::getContentsMargins() to get the margin.
    \sa setMargin(), QWidget::getContentsMargins()
*/

/*!
    \fn void Q3PopupMenu::setMidLineWidth(int)
    \internal
*/

/*!
    \fn int Q3PopupMenu::midLineWidth() const
    \internal
*/

QT_END_NAMESPACE
