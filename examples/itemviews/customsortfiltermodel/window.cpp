/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtGui>

#include "mysortfilterproxymodel.h"
#include "window.h"

//! [0]
Window::Window()
{
    proxyModel = new MySortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
//! [0]

//! [1]
    sourceView = new QTreeView;
    sourceView->setRootIsDecorated(false);
    sourceView->setAlternatingRowColors(true);
//! [1]

    QHBoxLayout *sourceLayout = new QHBoxLayout;
//! [2]
    sourceLayout->addWidget(sourceView);
    sourceGroupBox = new QGroupBox(tr("Original Model"));
    sourceGroupBox->setLayout(sourceLayout);
//! [2]

//! [3]
    filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));
    filterCaseSensitivityCheckBox->setChecked(true);

    filterPatternLineEdit = new QLineEdit;
    filterPatternLineEdit->setText("Grace|Sports");

    filterPatternLabel = new QLabel(tr("&Filter pattern:"));
    filterPatternLabel->setBuddy(filterPatternLineEdit);

    filterSyntaxComboBox = new QComboBox;
    filterSyntaxComboBox->addItem(tr("Regular expression"), QRegExp::RegExp);
    filterSyntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);

    fromDateEdit = new QDateEdit;
    fromDateEdit->setDate(QDate(1970, 01, 01));
    fromLabel = new QLabel(tr("F&rom:"));
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit;
    toDateEdit->setDate(QDate(2099, 12, 31));
    toLabel = new QLabel(tr("&To:"));
    toLabel->setBuddy(toDateEdit);

    connect(filterPatternLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFilterChanged()));
    connect(filterSyntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(textFilterChanged()));
    connect(filterCaseSensitivityCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(textFilterChanged()));
    connect(fromDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(dateFilterChanged()));
    connect(toDateEdit, SIGNAL(dateChanged(QDate)),
//! [3] //! [4]
            this, SLOT(dateFilterChanged()));
//! [4]

//! [5]
    proxyView = new QTreeView;
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    proxyView->setSortingEnabled(true);
    proxyView->sortByColumn(1, Qt::AscendingOrder);

    QGridLayout *proxyLayout = new QGridLayout;
    proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
    proxyLayout->addWidget(filterPatternLabel, 1, 0);
    proxyLayout->addWidget(filterPatternLineEdit, 1, 1);
    proxyLayout->addWidget(filterSyntaxComboBox, 1, 2);
    proxyLayout->addWidget(filterCaseSensitivityCheckBox, 2, 0, 1, 3);
    proxyLayout->addWidget(fromLabel, 3, 0);
    proxyLayout->addWidget(fromDateEdit, 3, 1, 1, 2);
    proxyLayout->addWidget(toLabel, 4, 0);
    proxyLayout->addWidget(toDateEdit, 4, 1, 1, 2);

    proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));
    proxyGroupBox->setLayout(proxyLayout);
//! [5]

//! [6]
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(sourceGroupBox);
    mainLayout->addWidget(proxyGroupBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Custom Sort/Filter Model"));
    resize(500, 450);
}
//! [6]

//! [7]
void Window::setSourceModel(QAbstractItemModel *model)
{
    proxyModel->setSourceModel(model);
    sourceView->setModel(model);
}
//! [7]

//! [8]
void Window::textFilterChanged()
{
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(filterSyntaxComboBox->itemData(
                    filterSyntaxComboBox->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            filterCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive
                                                       : Qt::CaseInsensitive;

    QRegExp regExp(filterPatternLineEdit->text(), caseSensitivity, syntax);
    proxyModel->setFilterRegExp(regExp);
}
//! [8]

//! [9]
void Window::dateFilterChanged()
{
    proxyModel->setFilterMinimumDate(fromDateEdit->date());
    proxyModel->setFilterMaximumDate(toDateEdit->date());
}
//! [9]
