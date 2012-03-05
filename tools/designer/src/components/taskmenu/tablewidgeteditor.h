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

#ifndef TABLEWIDGETEDITOR_H
#define TABLEWIDGETEDITOR_H

#include "ui_tablewidgeteditor.h"

#include "listwidgeteditor.h"

#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QTableWidget;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class FormWindowBase;
class PropertySheetIconValue;

class TableWidgetEditor: public AbstractItemEditor
{
    Q_OBJECT
public:
    explicit TableWidgetEditor(QDesignerFormWindowInterface *form, QDialog *dialog);

    TableWidgetContents fillContentsFromTableWidget(QTableWidget *tableWidget);
    TableWidgetContents contents() const;

private slots:

    void on_tableWidget_currentCellChanged(int currentRow, int currnetCol, int, int);
    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_columnEditor_indexChanged(int idx);
    void on_columnEditor_itemChanged(int idx, int role, const QVariant &v);

    void on_columnEditor_itemInserted(int idx);
    void on_columnEditor_itemDeleted(int idx);
    void on_columnEditor_itemMovedUp(int idx);
    void on_columnEditor_itemMovedDown(int idx);

    void on_rowEditor_indexChanged(int idx);
    void on_rowEditor_itemChanged(int idx, int role, const QVariant &v);

    void on_rowEditor_itemInserted(int idx);
    void on_rowEditor_itemDeleted(int idx);
    void on_rowEditor_itemMovedUp(int idx);
    void on_rowEditor_itemMovedDown(int idx);

    void togglePropertyBrowser();

    void cacheReloaded();

protected:
    virtual void setItemData(int role, const QVariant &v);
    virtual QVariant getItemData(int role) const;

private:
    void setPropertyBrowserVisible(bool v);
    void updateEditor();
    void moveColumnsLeft(int fromColumn, int toColumn);
    void moveColumnsRight(int fromColumn, int toColumn);
    void moveRowsUp(int fromRow, int toRow);
    void moveRowsDown(int fromRow, int toRow);

    Ui::TableWidgetEditor ui;
    ItemListEditor *m_rowEditor;
    ItemListEditor *m_columnEditor;
    bool m_updatingBrowser;
};

class TableWidgetEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TableWidgetEditorDialog(QDesignerFormWindowInterface *form, QWidget *parent);

    TableWidgetContents fillContentsFromTableWidget(QTableWidget *tableWidget);
    TableWidgetContents contents() const;

private:
    TableWidgetEditor m_editor;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TABLEWIDGETEDITOR_H
