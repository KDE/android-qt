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

#ifndef TABORDEREDITOR_TOOL_H
#define TABORDEREDITOR_TOOL_H

#include "tabordereditor_global.h"

#include <QtCore/QPointer>

#include <QtDesigner/QDesignerFormWindowToolInterface>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QAction;

namespace qdesigner_internal {

class TabOrderEditor;

class QT_TABORDEREDITOR_EXPORT TabOrderEditorTool: public QDesignerFormWindowToolInterface
{
    Q_OBJECT
public:
    explicit TabOrderEditorTool(QDesignerFormWindowInterface *formWindow, QObject *parent = 0);
    virtual ~TabOrderEditorTool();

    virtual QDesignerFormEditorInterface *core() const;
    virtual QDesignerFormWindowInterface *formWindow() const;

    virtual QWidget *editor() const;
    virtual QAction *action() const;

    virtual void activated();
    virtual void deactivated();

    virtual bool handleEvent(QWidget *widget, QWidget *managedWidget, QEvent *event);

private:
    QDesignerFormWindowInterface *m_formWindow;
    mutable QPointer<TabOrderEditor> m_editor;
    QAction *m_action;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TABORDEREDITOR_TOOL_H
