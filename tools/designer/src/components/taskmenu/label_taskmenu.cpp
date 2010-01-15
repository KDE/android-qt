/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "label_taskmenu.h"
#include "inplace_editor.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QTextDocument>

static const char *textPropertyC = "text";

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// -------- LabelTaskMenuInlineEditor
class LabelTaskMenuInlineEditor : public  TaskMenuInlineEditor
{
public:
    LabelTaskMenuInlineEditor(QLabel *button, QObject *parent);

protected:
    virtual QRect editRectangle() const;
};

LabelTaskMenuInlineEditor::LabelTaskMenuInlineEditor(QLabel *w, QObject *parent) :
      TaskMenuInlineEditor(w, ValidationRichText, QLatin1String(textPropertyC), parent)
{
}

QRect LabelTaskMenuInlineEditor::editRectangle() const
{
    QStyleOptionButton opt;
    opt.init(widget());
    return opt.rect;
}

// --------------- LabelTaskMenu

LabelTaskMenu::LabelTaskMenu(QLabel *label, QObject *parent)
    : QDesignerTaskMenu(label, parent),
      m_label(label),
      m_editRichTextAction(new QAction(tr("Change rich text..."), this)),
      m_editPlainTextAction(new QAction(tr("Change plain text..."), this))
{
    LabelTaskMenuInlineEditor *editor = new LabelTaskMenuInlineEditor(label, this);
    connect(m_editPlainTextAction, SIGNAL(triggered()), editor, SLOT(editText()));
    m_taskActions.append(m_editPlainTextAction);

    connect(m_editRichTextAction, SIGNAL(triggered()), this, SLOT(editRichText()));
    m_taskActions.append(m_editRichTextAction);

    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    m_taskActions.append(sep);
}

QAction *LabelTaskMenu::preferredEditAction() const
{
    if (m_label->textFormat () == Qt::PlainText) return m_editPlainTextAction;
    return Qt::mightBeRichText(m_label->text()) ? m_editRichTextAction : m_editPlainTextAction;
}

QList<QAction*> LabelTaskMenu::taskActions() const
{
    return m_taskActions + QDesignerTaskMenu::taskActions();
}

void LabelTaskMenu::editRichText()
{
    changeTextProperty(QLatin1String(textPropertyC), QString(), MultiSelectionMode, m_label->textFormat());
}

}
QT_END_NAMESPACE
