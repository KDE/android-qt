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

#include "findfiledialog.h"

//! [0]
FindFileDialog::FindFileDialog(QTextEdit *editor, QAssistantClient *assistant,
                               QWidget *parent)
    : QDialog(parent)
{
    currentAssistantClient = assistant;
    currentEditor = editor;
//! [0]

    createButtons();
    createComboBoxes();
    createFilesTree();
    createLabels();
    createLayout();

    directoryComboBox->addItem(QDir::toNativeSeparators(QDir::currentPath()));
    fileNameComboBox->addItem("*");
    findFiles();

    setWindowTitle(tr("Find File"));
//! [1]
}
//! [1]

void FindFileDialog::browse()
{
    QString currentDirectory = directoryComboBox->currentText();
    QString newDirectory = QFileDialog::getExistingDirectory(this,
                               tr("Select Directory"), currentDirectory);
    if (!newDirectory.isEmpty()) {
        directoryComboBox->addItem(QDir::toNativeSeparators(newDirectory));
        directoryComboBox->setCurrentIndex(directoryComboBox->count() - 1);
        update();
    }
}

//! [2]
void FindFileDialog::help()
{
    currentAssistantClient->showPage(QLibraryInfo::location(QLibraryInfo::ExamplesPath) +
            QDir::separator() +  "assistant/simpletextviewer/documentation/filedialog.html");
}
//! [2]

void FindFileDialog::openFile(QTreeWidgetItem *item)
{
    if (!item) {
        item = foundFilesTree->currentItem();
        if (!item)
            return;
    }

    QString fileName = item->text(0);
    QString path = directoryComboBox->currentText() + QDir::separator();

    QFile file(path + fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QString data(file.readAll());

        if (fileName.endsWith(".html"))
            currentEditor->setHtml(data);
        else
            currentEditor->setPlainText(data);
    }
    close();
}

void FindFileDialog::update()
{
    findFiles();
    buttonBox->button(QDialogButtonBox::Open)->setEnabled(
            foundFilesTree->topLevelItemCount() > 0);
}

void FindFileDialog::findFiles()
{
    QRegExp filePattern(fileNameComboBox->currentText() + "*");
    filePattern.setPatternSyntax(QRegExp::Wildcard);

    QDir directory(directoryComboBox->currentText());

    QStringList allFiles = directory.entryList(QDir::Files | QDir::NoSymLinks);
    QStringList matchingFiles;

    foreach (QString file, allFiles) {
        if (filePattern.exactMatch(file))
            matchingFiles << file;
    }
    showFiles(matchingFiles);
}

void FindFileDialog::showFiles(const QStringList &files)
{
    foundFilesTree->clear();

    for (int i = 0; i < files.count(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(foundFilesTree);
        item->setText(0, files[i]);
    }

    if (files.count() > 0)
        foundFilesTree->setCurrentItem(foundFilesTree->topLevelItem(0));
}

void FindFileDialog::createButtons()
{
    browseButton = new QToolButton;
    browseButton->setText(tr("..."));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Open
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Help);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(openFile()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
}

void FindFileDialog::createComboBoxes()
{
    directoryComboBox = new QComboBox;
    fileNameComboBox = new QComboBox;

    fileNameComboBox->setEditable(true);
    fileNameComboBox->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Preferred);

    directoryComboBox->setMinimumContentsLength(30);
    directoryComboBox->setSizeAdjustPolicy(
            QComboBox::AdjustToMinimumContentsLength);
    directoryComboBox->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Preferred);

    connect(fileNameComboBox, SIGNAL(editTextChanged(QString)),
            this, SLOT(update()));
    connect(directoryComboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(update()));
}

void FindFileDialog::createFilesTree()
{
    foundFilesTree = new QTreeWidget;
    foundFilesTree->setColumnCount(1);
    foundFilesTree->setHeaderLabels(QStringList(tr("Matching Files")));
    foundFilesTree->setRootIsDecorated(false);
    foundFilesTree->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(foundFilesTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(openFile(QTreeWidgetItem*)));
}

void FindFileDialog::createLabels()
{
    directoryLabel = new QLabel(tr("Search in:"));
    fileNameLabel = new QLabel(tr("File name (including wildcards):"));
}

void FindFileDialog::createLayout()
{
    QHBoxLayout *fileLayout = new QHBoxLayout;
    fileLayout->addWidget(fileNameLabel);
    fileLayout->addWidget(fileNameComboBox);

    QHBoxLayout *directoryLayout = new QHBoxLayout;
    directoryLayout->addWidget(directoryLabel);
    directoryLayout->addWidget(directoryComboBox);
    directoryLayout->addWidget(browseButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(fileLayout);
    mainLayout->addLayout(directoryLayout);
    mainLayout->addWidget(foundFilesTree);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}
