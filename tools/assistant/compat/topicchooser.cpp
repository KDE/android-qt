/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "topicchooser.h"

#include <QLabel>
#include <QListWidget>
#include <QPushButton>

QT_BEGIN_NAMESPACE

TopicChooser::TopicChooser(QWidget *parent, const QStringList &lnkNames,
                            const QStringList &lnks, const QString &title)
    : QDialog(parent), links(lnks), linkNames(lnkNames)
{
    ui.setupUi(this);

    ui.label->setText(tr("Choose a topic for <b>%1</b>").arg(title));
    ui.listbox->addItems(linkNames);
    if (ui.listbox->count() != 0)
        ui.listbox->setCurrentRow(0);
    ui.listbox->setFocus();
}

QString TopicChooser::link() const
{
    if (ui.listbox->currentRow() == -1)
        return QString();
    QString s = ui.listbox->item(ui.listbox->currentRow())->text();
    if (s.isEmpty())
        return s;
    int i = linkNames.indexOf(s);
    return links[i];
}

QString TopicChooser::getLink(QWidget *parent, const QStringList &lnkNames,
                              const QStringList &lnks, const QString &title)
{
    TopicChooser *dlg = new TopicChooser(parent, lnkNames, lnks, title);
    QString lnk;
    if (dlg->exec() == QDialog::Accepted)
        lnk = dlg->link();
    delete dlg;
    return lnk;
}

void TopicChooser::on_buttonDisplay_clicked()
{
    accept();
}

void TopicChooser::on_buttonCancel_clicked()
{
    reject();
}

void TopicChooser::on_listbox_itemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item);
    accept();
}

QT_END_NAMESPACE
