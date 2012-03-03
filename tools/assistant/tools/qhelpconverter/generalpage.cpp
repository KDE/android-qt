/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtGui/QMessageBox>
#include "generalpage.h"

QT_BEGIN_NAMESPACE

GeneralPage::GeneralPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("General Settings"));
    setSubTitle(tr("Specify the namespace and the virtual "
        "folder for the documentation."));

    m_ui.setupUi(this);
    connect(m_ui.namespaceLineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(completeChanged()));
    connect(m_ui.folderLineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(completeChanged()));

    m_ui.namespaceLineEdit->setText(QLatin1String("mycompany.com"));
    m_ui.folderLineEdit->setText(QLatin1String("product_1.0"));

    registerField(QLatin1String("namespaceName"), m_ui.namespaceLineEdit);
    registerField(QLatin1String("virtualFolder"), m_ui.folderLineEdit);
}

bool GeneralPage::isComplete() const
{
    if (m_ui.namespaceLineEdit->text().isEmpty()
        || m_ui.folderLineEdit->text().isEmpty())
        return false;
    return true;
}

bool GeneralPage::validatePage()
{
    QString s = m_ui.namespaceLineEdit->text();
    if (s.contains(QLatin1Char('/')) || s.contains(QLatin1Char('\\'))) {
        QMessageBox::critical(this, tr("Namespace Error"),
            tr("The namespace contains some invalid characters."));
        return false;
    }
    s = m_ui.folderLineEdit->text();
    if (s.contains(QLatin1Char('/')) || s.contains(QLatin1Char('\\'))) {
        QMessageBox::critical(this, tr("Virtual Folder Error"),
            tr("The virtual folder contains some invalid characters."));
        return false;
    }
    return true;
}

QT_END_NAMESPACE
