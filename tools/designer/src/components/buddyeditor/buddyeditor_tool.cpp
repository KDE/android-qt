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

#include "buddyeditor_tool.h"
#include "buddyeditor.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

BuddyEditorTool::BuddyEditorTool(QDesignerFormWindowInterface *formWindow, QObject *parent)
    : QDesignerFormWindowToolInterface(parent),
      m_formWindow(formWindow),
      m_action(new QAction(tr("Edit Buddies"), this))
{
}

BuddyEditorTool::~BuddyEditorTool()
{
}

QDesignerFormEditorInterface *BuddyEditorTool::core() const
{
    return m_formWindow->core();
}

QDesignerFormWindowInterface *BuddyEditorTool::formWindow() const
{
    return m_formWindow;
}

bool BuddyEditorTool::handleEvent(QWidget *widget, QWidget *managedWidget, QEvent *event)
{
    Q_UNUSED(widget);
    Q_UNUSED(managedWidget);
    Q_UNUSED(event);

    return false;
}

QWidget *BuddyEditorTool::editor() const
{
    if (!m_editor) {
        Q_ASSERT(formWindow() != 0);
        m_editor = new BuddyEditor(formWindow(), 0);
        connect(formWindow(), SIGNAL(mainContainerChanged(QWidget*)), m_editor, SLOT(setBackground(QWidget*)));
        connect(formWindow(), SIGNAL(changed()),
                    m_editor, SLOT(updateBackground()));
    }

    return m_editor;
}

void BuddyEditorTool::activated()
{
    m_editor->enableUpdateBackground(true);
}

void BuddyEditorTool::deactivated()
{
    m_editor->enableUpdateBackground(false);
}

QAction *BuddyEditorTool::action() const
{
    return m_action;
}

QT_END_NAMESPACE
