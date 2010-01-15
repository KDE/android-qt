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

#include "preferencesdialog.h"
#include "filternamedialog.h"
#include "installdialog.h"
#include "fontpanel.h"
#include "centralwidget.h"
#include "aboutdialog.h"

#include <QtAlgorithms>

#include <QtGui/QHeaderView>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QFontDatabase>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

#include <QtHelp/QHelpEngineCore>

QT_BEGIN_NAMESPACE

PreferencesDialog::PreferencesDialog(QHelpEngineCore *helpEngine, QWidget *parent)
    : QDialog(parent)
    , m_helpEngine(helpEngine)
    , m_appFontChanged(false)
    , m_browserFontChanged(false)
{
    m_ui.setupUi(this);

    connect(m_ui.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
        this, SLOT(applyChanges()));
    connect(m_ui.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
        this, SLOT(reject()));

    QLatin1String key("EnableFilterFunctionality");
    m_hideFiltersTab = !m_helpEngine->customValue(key, true).toBool();

    key = QLatin1String("EnableDocumentationManager");
    m_hideDocsTab = !m_helpEngine->customValue(key, true).toBool();

    if (!m_hideFiltersTab) {
        m_ui.attributeWidget->header()->hide();
        m_ui.attributeWidget->setRootIsDecorated(false);

        connect(m_ui.attributeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updateFilterMap()));

        connect(m_ui.filterWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this,
            SLOT(updateAttributes(QListWidgetItem*)));

        connect(m_ui.filterAddButton, SIGNAL(clicked()), this,
            SLOT(addFilter()));
        connect(m_ui.filterRemoveButton, SIGNAL(clicked()), this,
            SLOT(removeFilter()));

        updateFilterPage();
    } else {
        m_ui.tabWidget->removeTab(m_ui.tabWidget->indexOf(m_ui.filtersTab));
    }

    if (!m_hideDocsTab) {
        connect(m_ui.docAddButton, SIGNAL(clicked()), this,
            SLOT(addDocumentationLocal()));
        connect(m_ui.docRemoveButton, SIGNAL(clicked()), this,
            SLOT(removeDocumentation()));

        m_docsBackup = m_helpEngine->registeredDocumentations();
        m_ui.registeredDocsListWidget->addItems(m_docsBackup);
    } else {
        m_ui.tabWidget->removeTab(m_ui.tabWidget->indexOf(m_ui.docsTab));
    }

    updateFontSettingsPage();
    updateOptionsPage();
}

PreferencesDialog::~PreferencesDialog()
{
    QLatin1String key("");
    if (m_appFontChanged) {
        key = QLatin1String("appFont");
        m_helpEngine->setCustomValue(key, m_appFontPanel->selectedFont());

        key = QLatin1String("useAppFont");
        m_helpEngine->setCustomValue(key, m_appFontPanel->isChecked());

        key = QLatin1String("appWritingSystem");
        m_helpEngine->setCustomValue(key, m_appFontPanel->writingSystem());
    }

    if (m_browserFontChanged) {
        key = QLatin1String("browserFont");
        m_helpEngine->setCustomValue(key, m_browserFontPanel->selectedFont());

        key = QLatin1String("useBrowserFont");
        m_helpEngine->setCustomValue(key, m_browserFontPanel->isChecked());

        key = QLatin1String("browserWritingSystem");
        m_helpEngine->setCustomValue(key, m_browserFontPanel->writingSystem());
    }

    if (m_appFontChanged || m_browserFontChanged) {
        emit updateApplicationFont();
        emit updateBrowserFont();
    }

    QString homePage = m_ui.homePageLineEdit->text();
    if (homePage.isEmpty())
        homePage = QLatin1String("help");
    m_helpEngine->setCustomValue(QLatin1String("homepage"), homePage);

    int option = m_ui.helpStartComboBox->currentIndex();
    m_helpEngine->setCustomValue(QLatin1String("StartOption"), option);
}

void PreferencesDialog::showDialog()
{
    if (exec() != Accepted)
        m_appFontChanged = m_browserFontChanged = false;
}

void PreferencesDialog::updateFilterPage()
{
    if (!m_helpEngine)
        return;

    m_ui.filterWidget->clear();
    m_ui.attributeWidget->clear();

    QHelpEngineCore help(m_helpEngine->collectionFile(), 0);
    help.setupData();
    m_filterMapBackup.clear();
    const QStringList filters = help.customFilters();
    foreach (const QString &filter, filters) {
        QStringList atts = help.filterAttributes(filter);
        m_filterMapBackup.insert(filter, atts);
        if (!m_filterMap.contains(filter))
            m_filterMap.insert(filter, atts);
    }

    m_ui.filterWidget->addItems(m_filterMap.keys());

    foreach (const QString &a, help.filterAttributes())
        new QTreeWidgetItem(m_ui.attributeWidget, QStringList() << a);

    if (m_filterMap.keys().count())
        m_ui.filterWidget->setCurrentRow(0);
}

void PreferencesDialog::updateAttributes(QListWidgetItem *item)
{
    QStringList checkedList;
    if (item)
        checkedList = m_filterMap.value(item->text());
    QTreeWidgetItem *itm;
    for (int i = 0; i < m_ui.attributeWidget->topLevelItemCount(); ++i) {
        itm = m_ui.attributeWidget->topLevelItem(i);
        if (checkedList.contains(itm->text(0)))
            itm->setCheckState(0, Qt::Checked);
        else
            itm->setCheckState(0, Qt::Unchecked);
    }
}

void PreferencesDialog::updateFilterMap()
{
    if (!m_ui.filterWidget->currentItem())
        return;
    QString filter = m_ui.filterWidget->currentItem()->text();
    if (!m_filterMap.contains(filter))
        return;

    QStringList newAtts;
    QTreeWidgetItem *itm = 0;
    for (int i = 0; i < m_ui.attributeWidget->topLevelItemCount(); ++i) {
        itm = m_ui.attributeWidget->topLevelItem(i);
        if (itm->checkState(0) == Qt::Checked)
            newAtts.append(itm->text(0));
    }
    m_filterMap[filter] = newAtts;
}

void PreferencesDialog::addFilter()
{
    FilterNameDialog dia(this);
    if (dia.exec() == QDialog::Rejected)
        return;

    QString filterName = dia.filterName();
    if (!m_filterMap.contains(filterName)) {
        m_filterMap.insert(filterName, QStringList());
        m_ui.filterWidget->addItem(filterName);
    }

    QList<QListWidgetItem*> lst = m_ui.filterWidget
        ->findItems(filterName, Qt::MatchCaseSensitive);
    m_ui.filterWidget->setCurrentItem(lst.first());
}

void PreferencesDialog::removeFilter()
{
    QListWidgetItem *item =
        m_ui.filterWidget ->takeItem(m_ui.filterWidget->currentRow());
    if (!item)
        return;

    m_filterMap.remove(item->text());
    m_removedFilters.append(item->text());
    delete item;
    if (m_ui.filterWidget->count())
        m_ui.filterWidget->setCurrentRow(0);
}

void PreferencesDialog::addDocumentationLocal()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Add Documentation"), QString(), tr("Qt Compressed Help Files (*.qch)"));
    if (fileNames.isEmpty())
        return;

    QStringList invalidFiles;
    QStringList alreadyRegistered;
    foreach (const QString &fileName, fileNames) {
        const QString nameSpace = QHelpEngineCore::namespaceName(fileName);
        if (nameSpace.isEmpty()) {
            invalidFiles.append(fileName);
            continue;
        }

        if (m_ui.registeredDocsListWidget->findItems(nameSpace,
            Qt::MatchFixedString).count()) {
                alreadyRegistered.append(nameSpace);
                continue;
        }

        m_helpEngine->registerDocumentation(fileName);
        m_ui.registeredDocsListWidget->addItem(nameSpace);
        m_regDocs.append(nameSpace);
        m_unregDocs.removeAll(nameSpace);
    }

    if (!invalidFiles.isEmpty() || !alreadyRegistered.isEmpty()) {
        QString message;
        if (!alreadyRegistered.isEmpty()) {
            foreach (const QString &ns, alreadyRegistered) {
                message += tr("The namespace %1 is already registered!")
                    .arg(QString("<b>%1</b>").arg(ns)) + QLatin1String("<br>");
            }
            if (!invalidFiles.isEmpty())
                message.append(QLatin1String("<br>"));
        }

        if (!invalidFiles.isEmpty()) {
            message += tr("The specified file is not a valid Qt Help File!");
            message.append(QLatin1String("<ul>"));
            foreach (const QString &file, invalidFiles)
                message += QLatin1String("<li>") + file + QLatin1String("</li>");
            message.append(QLatin1String("</ul>"));
        }
        QMessageBox::warning(this, tr("Add Documentation"), message);
    }

    updateFilterPage();
}

void PreferencesDialog::removeDocumentation()
{
    bool foundBefore = false;
    CentralWidget* widget = CentralWidget::instance();
    QMap<int, QString> openedDocList = widget->currentSourceFileList();
    QStringList values(openedDocList.values());

    QList<QListWidgetItem*> l = m_ui.registeredDocsListWidget->selectedItems();
    foreach (QListWidgetItem* item, l) {
        const QString& ns = item->text();
        if (!foundBefore && values.contains(ns)) {
            if (0 == QMessageBox::information(this, tr("Remove Documentation"),
                tr("Some documents currently opened in Assistant reference the "
                   "documentation you are attempting to remove. Removing the "
                   "documentation will close those documents."), tr("Cancel"),
                tr("OK"))) return;
            foundBefore = true;
        }

        m_unregDocs.append(ns);
        m_TabsToClose += openedDocList.keys(ns);
        delete m_ui.registeredDocsListWidget->takeItem(
            m_ui.registeredDocsListWidget->row(item));
    }

    if (m_ui.registeredDocsListWidget->count()) {
        m_ui.registeredDocsListWidget->setCurrentRow(0,
            QItemSelectionModel::ClearAndSelect);
    }
}

void PreferencesDialog::applyChanges()
{
    bool filtersWereChanged = false;
    if (!m_hideFiltersTab) {
        if (m_filterMap.count() != m_filterMapBackup.count()) {
            filtersWereChanged = true;
        } else {
            QMapIterator<QString, QStringList> it(m_filterMapBackup);
            while (it.hasNext() && !filtersWereChanged) {
                it.next();
                if (!m_filterMap.contains(it.key())) {
                    filtersWereChanged = true;
                } else {
                    QStringList a = it.value();
                    QStringList b = m_filterMap.value(it.key());
                    if (a.count() != b.count()) {
                        filtersWereChanged = true;
                    } else {
                        QStringList::const_iterator i(a.constBegin());
                        while (i != a.constEnd()) {
                            if (!b.contains(*i)) {
                                filtersWereChanged = true;
                                break;
                            }
                            ++i;
                        }
                    }
                }
            }
        }
    }

    if (filtersWereChanged) {
        foreach (const QString &filter, m_removedFilters)
            m_helpEngine->removeCustomFilter(filter);
        QMapIterator<QString, QStringList> it(m_filterMap);
        while (it.hasNext()) {
            it.next();
            m_helpEngine->addCustomFilter(it.key(), it.value());
        }
    }

    qSort(m_TabsToClose);
    CentralWidget* widget = CentralWidget::instance();
    for (int i = m_TabsToClose.count(); --i >= 0;)
        widget->closeTabAt(m_TabsToClose.at(i));
    if (widget->availableHelpViewer()== 0)
        widget->setSource(QUrl(QLatin1String("about:blank")));

    if (m_unregDocs.count()) {
        foreach (const QString &doc, m_unregDocs)
            m_helpEngine->unregisterDocumentation(doc);
    }

    if (filtersWereChanged || m_regDocs.count() || m_unregDocs.count())
        m_helpEngine->setupData();

    accept();
}

void PreferencesDialog::updateFontSettingsPage()
{
    m_browserFontPanel = new FontPanel(this);
    m_browserFontPanel->setCheckable(true);
    m_ui.stackedWidget_2->insertWidget(0, m_browserFontPanel);

    m_appFontPanel = new FontPanel(this);
    m_appFontPanel->setCheckable(true);
    m_ui.stackedWidget_2->insertWidget(1, m_appFontPanel);

    m_ui.stackedWidget_2->setCurrentIndex(0);

    const QString customSettings(tr("Use custom settings"));
    m_appFontPanel->setTitle(customSettings);

    QLatin1String key = QLatin1String("appFont");
    QFont font = qVariantValue<QFont>(m_helpEngine->customValue(key));
    m_appFontPanel->setSelectedFont(font);

    key = QLatin1String("appWritingSystem");
    QFontDatabase::WritingSystem system = static_cast<QFontDatabase::WritingSystem>
        (m_helpEngine->customValue(key).toInt());
    m_appFontPanel->setWritingSystem(system);

    key = QLatin1String("useAppFont");
    m_appFontPanel->setChecked(m_helpEngine->customValue(key).toBool());

    m_browserFontPanel->setTitle(customSettings);

    key = QLatin1String("browserFont");
    font = qVariantValue<QFont>(m_helpEngine->customValue(key));
    m_browserFontPanel->setSelectedFont(font);

    key = QLatin1String("browserWritingSystem");
    system = static_cast<QFontDatabase::WritingSystem>
        (m_helpEngine->customValue(key).toInt());
    m_browserFontPanel->setWritingSystem(system);

    key = QLatin1String("useBrowserFont");
    m_browserFontPanel->setChecked(m_helpEngine->customValue(key).toBool());

    connect(m_appFontPanel, SIGNAL(toggled(bool)), this,
        SLOT(appFontSettingToggled(bool)));
    connect(m_browserFontPanel, SIGNAL(toggled(bool)), this,
        SLOT(browserFontSettingToggled(bool)));

    QList<QComboBox*> allCombos = qFindChildren<QComboBox*>(m_appFontPanel);
    foreach (QComboBox* box, allCombos) {
        connect(box, SIGNAL(currentIndexChanged(int)), this,
            SLOT(appFontSettingChanged(int)));
    }

    allCombos = qFindChildren<QComboBox*>(m_browserFontPanel);
    foreach (QComboBox* box, allCombos) {
        connect(box, SIGNAL(currentIndexChanged(int)), this,
            SLOT(browserFontSettingChanged(int)));
    }
}

void PreferencesDialog::appFontSettingToggled(bool on)
{
    Q_UNUSED(on)
    m_appFontChanged = true;
}

void PreferencesDialog::appFontSettingChanged(int index)
{
    Q_UNUSED(index)
    m_appFontChanged = true;
}

void PreferencesDialog::browserFontSettingToggled(bool on)
{
    Q_UNUSED(on)
    m_browserFontChanged = true;
}

void PreferencesDialog::browserFontSettingChanged(int index)
{
    Q_UNUSED(index)
    m_browserFontChanged = true;
}

void PreferencesDialog::updateOptionsPage()
{
    QString homepage = m_helpEngine->customValue(QLatin1String("homepage"),
        QLatin1String("")).toString();

    if (homepage.isEmpty()) {
        homepage = m_helpEngine->customValue(QLatin1String("defaultHomepage"),
            QLatin1String("help")).toString();
    }
    m_ui.homePageLineEdit->setText(homepage);

    int option = m_helpEngine->customValue(QLatin1String("StartOption"),
        ShowLastPages).toInt();
    m_ui.helpStartComboBox->setCurrentIndex(option);

    connect(m_ui.blankPageButton, SIGNAL(clicked()), this, SLOT(setBlankPage()));
    connect(m_ui.currentPageButton, SIGNAL(clicked()), this, SLOT(setCurrentPage()));
    connect(m_ui.defaultPageButton, SIGNAL(clicked()), this, SLOT(setDefaultPage()));
}

void PreferencesDialog::setBlankPage()
{
    m_ui.homePageLineEdit->setText(QLatin1String("about:blank"));
}

void PreferencesDialog::setCurrentPage()
{
    QString homepage = CentralWidget::instance()->currentSource().toString();
    if (homepage.isEmpty())
        homepage = QLatin1String("help");

    m_ui.homePageLineEdit->setText(homepage);
}

void PreferencesDialog::setDefaultPage()
{
    QString homepage = m_helpEngine->customValue(QLatin1String("defaultHomepage"),
        QLatin1String("help")).toString();
    m_ui.homePageLineEdit->setText(homepage);
}

QT_END_NAMESPACE
