/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "tabordereditor_tool.h"
#include "tabordereditor.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtCore/QEvent>
#include <QtGui/QAction>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

TabOrderEditorTool::TabOrderEditorTool(QDesignerFormWindowInterface *formWindow, QObject *parent)
    : QDesignerFormWindowToolInterface(parent),
      m_formWindow(formWindow),
      m_action(new QAction(tr("Edit Tab Order"), this))
{
}

TabOrderEditorTool::~TabOrderEditorTool()
{
}

QDesignerFormEditorInterface *TabOrderEditorTool::core() const
{
    return m_formWindow->core();
}

QDesignerFormWindowInterface *TabOrderEditorTool::formWindow() const
{
    return m_formWindow;
}

bool TabOrderEditorTool::handleEvent(QWidget *widget, QWidget *managedWidget, QEvent *event)
{
    Q_UNUSED(widget);
    Q_UNUSED(managedWidget);

    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
        return true;

    return false;
}

QWidget *TabOrderEditorTool::editor() const
{
    if (!m_editor) {
        Q_ASSERT(formWindow() != 0);
        m_editor = new TabOrderEditor(formWindow(), 0);
        connect(formWindow(), SIGNAL(mainContainerChanged(QWidget*)), m_editor, SLOT(setBackground(QWidget*)));
    }

    return m_editor;
}

void TabOrderEditorTool::activated()
{
    connect(formWindow(), SIGNAL(changed()),
                m_editor, SLOT(updateBackground()));
}

void TabOrderEditorTool::deactivated()
{
    disconnect(formWindow(), SIGNAL(changed()),
                m_editor, SLOT(updateBackground()));
}

QAction *TabOrderEditorTool::action() const
{
    return m_action;
}

QT_END_NAMESPACE
