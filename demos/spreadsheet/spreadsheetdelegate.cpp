/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "spreadsheetdelegate.h"
#include <QtGui>

SpreadSheetDelegate::SpreadSheetDelegate(QObject *parent)
        : QItemDelegate(parent) {}

QWidget *SpreadSheetDelegate::createEditor(QWidget *parent,
                                          const QStyleOptionViewItem &,
                                          const QModelIndex &index) const
{
    if (index.column() == 1) {
        QDateTimeEdit *editor = new QDateTimeEdit(parent);
        editor->setDisplayFormat("dd/M/yyyy");
        editor->setCalendarPopup(true);
        return editor;
    }

    QLineEdit *editor = new QLineEdit(parent);

    // create a completer with the strings in the column as model
    QStringList allStrings;
    for (int i = 1; i<index.model()->rowCount(); i++) {
        QString strItem(index.model()->data(index.sibling(i, index.column()),
            Qt::EditRole).toString());

        if (!allStrings.contains(strItem))
            allStrings.append(strItem);
    }

    QCompleter *autoComplete = new QCompleter(allStrings);
    editor->setCompleter(autoComplete);
    connect(editor, SIGNAL(editingFinished()),
        this, SLOT(commitAndCloseEditor()));
    return editor;
}

void SpreadSheetDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void SpreadSheetDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
    } else {
        QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
        if (dateEditor) {
            dateEditor->setDate(QDate::fromString(
            index.model()->data(index, Qt::EditRole).toString(),
            "d/M/yyyy"));
        }
    }
}

void SpreadSheetDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) {
        model->setData(index, edit->text());
    } else {
        QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
        if (dateEditor) {
            model->setData(index, dateEditor->date().toString("dd/M/yyyy"));
        }
    }
}

