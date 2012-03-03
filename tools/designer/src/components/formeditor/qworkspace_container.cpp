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

#include "qworkspace_container.h"
#include "qmdiarea_container.h"

#include <QtGui/QWorkspace>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QWorkspaceContainer::QWorkspaceContainer(QWorkspace *widget, QObject *parent)
    : QObject(parent),
      m_workspace(widget)
{
}

int QWorkspaceContainer::count() const
{
    return m_workspace->windowList(QWorkspace::CreationOrder).count();
}

QWidget *QWorkspaceContainer::widget(int index) const
{
    if (index < 0)
        return 0;
    return m_workspace->windowList(QWorkspace::CreationOrder).at(index);
}

int QWorkspaceContainer::currentIndex() const
{
    if (QWidget *aw = m_workspace->activeWindow())
        return m_workspace->windowList(QWorkspace::CreationOrder).indexOf(aw);
    return -1;
}

void QWorkspaceContainer::setCurrentIndex(int index)
{
    m_workspace->setActiveWindow(m_workspace->windowList(QWorkspace::CreationOrder).at(index));
}

void QWorkspaceContainer::addWidget(QWidget *widget)
{
    QWidget *frame = m_workspace->addWindow(widget, Qt::Window);
    frame->show();
    m_workspace->cascade();
    QMdiAreaContainer::positionNewMdiChild(m_workspace, frame);
}

void QWorkspaceContainer::insertWidget(int, QWidget *widget)
{
    addWidget(widget);
}

void QWorkspaceContainer::remove(int /* index */)
{
    // nothing to do here, reparenting to formwindow is apparently sufficient
}
}

QT_END_NAMESPACE
