/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdesignerundostack.h"

#include <QtGui/QUndoStack>
#include <QtGui/QUndoCommand>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QDesignerUndoStack::QDesignerUndoStack(QObject *parent) :
    QObject(parent),
    m_undoStack(new QUndoStack),
    m_fakeDirty(false)
{
    connect(m_undoStack, SIGNAL(indexChanged(int)), this, SIGNAL(changed()));
}

QDesignerUndoStack::~QDesignerUndoStack()
{ // QUndoStack is managed by the QUndoGroup
}

void QDesignerUndoStack::push(QUndoCommand * cmd)
{
    m_undoStack->push(cmd);
}

void QDesignerUndoStack::beginMacro(const QString &text)
{
    m_undoStack->beginMacro(text);
}

void QDesignerUndoStack::endMacro()
{
    m_undoStack->endMacro();
}

int  QDesignerUndoStack::index() const
{
    return m_undoStack->index();
}

const QUndoStack *QDesignerUndoStack::qundoStack() const
{
    return m_undoStack;
}
QUndoStack *QDesignerUndoStack::qundoStack()
{
    return m_undoStack;
}

bool QDesignerUndoStack::isDirty() const
{
    return m_fakeDirty || !m_undoStack->isClean();
}

void QDesignerUndoStack::setDirty(bool v)
{
    if (isDirty() == v)
        return;
    if (v) {
        m_fakeDirty = true;
        emit changed();
    } else {
        m_fakeDirty = false;
        m_undoStack->setClean();
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
