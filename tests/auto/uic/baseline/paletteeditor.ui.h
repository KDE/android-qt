/*
*********************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the autotests of the Qt Toolkit.
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading UI file 'paletteeditor.ui'
**
** Created: Fri Sep 4 10:17:14 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include "previewframe.h"
#include "qtcolorbutton.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_PaletteEditor
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *advancedBox;
    QGridLayout *gridLayout;
    QtColorButton *buildButton;
    QTreeView *paletteView;
    QRadioButton *detailsRadio;
    QRadioButton *computeRadio;
    QLabel *label;
    QGroupBox *GroupBox126;
    QGridLayout *gridLayout1;
    QRadioButton *disabledRadio;
    QRadioButton *inactiveRadio;
    QRadioButton *activeRadio;
    qdesigner_internal::PreviewFrame *previewFrame;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__PaletteEditor)
    {
        if (qdesigner_internal__PaletteEditor->objectName().isEmpty())
            qdesigner_internal__PaletteEditor->setObjectName(QString::fromUtf8("qdesigner_internal__PaletteEditor"));
        qdesigner_internal__PaletteEditor->resize(365, 409);
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qdesigner_internal__PaletteEditor->sizePolicy().hasHeightForWidth());
        qdesigner_internal__PaletteEditor->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(qdesigner_internal__PaletteEditor);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        advancedBox = new QGroupBox(qdesigner_internal__PaletteEditor);
        advancedBox->setObjectName(QString::fromUtf8("advancedBox"));
        advancedBox->setMinimumSize(QSize(0, 0));
        advancedBox->setMaximumSize(QSize(16777215, 16777215));
        gridLayout = new QGridLayout(advancedBox);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        buildButton = new QtColorButton(advancedBox);
        buildButton->setObjectName(QString::fromUtf8("buildButton"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(13));
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buildButton->sizePolicy().hasHeightForWidth());
        buildButton->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(buildButton, 0, 1, 1, 1);

        paletteView = new QTreeView(advancedBox);
        paletteView->setObjectName(QString::fromUtf8("paletteView"));
        paletteView->setMinimumSize(QSize(0, 200));

        gridLayout->addWidget(paletteView, 1, 0, 1, 4);

        detailsRadio = new QRadioButton(advancedBox);
        detailsRadio->setObjectName(QString::fromUtf8("detailsRadio"));

        gridLayout->addWidget(detailsRadio, 0, 3, 1, 1);

        computeRadio = new QRadioButton(advancedBox);
        computeRadio->setObjectName(QString::fromUtf8("computeRadio"));
        computeRadio->setChecked(true);

        gridLayout->addWidget(computeRadio, 0, 2, 1, 1);

        label = new QLabel(advancedBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);


        vboxLayout->addWidget(advancedBox);

        GroupBox126 = new QGroupBox(qdesigner_internal__PaletteEditor);
        GroupBox126->setObjectName(QString::fromUtf8("GroupBox126"));
        QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(GroupBox126->sizePolicy().hasHeightForWidth());
        GroupBox126->setSizePolicy(sizePolicy2);
        gridLayout1 = new QGridLayout(GroupBox126);
#ifndef Q_OS_MAC
        gridLayout1->setSpacing(6);
#endif
        gridLayout1->setContentsMargins(8, 8, 8, 8);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        disabledRadio = new QRadioButton(GroupBox126);
        disabledRadio->setObjectName(QString::fromUtf8("disabledRadio"));

        gridLayout1->addWidget(disabledRadio, 0, 2, 1, 1);

        inactiveRadio = new QRadioButton(GroupBox126);
        inactiveRadio->setObjectName(QString::fromUtf8("inactiveRadio"));

        gridLayout1->addWidget(inactiveRadio, 0, 1, 1, 1);

        activeRadio = new QRadioButton(GroupBox126);
        activeRadio->setObjectName(QString::fromUtf8("activeRadio"));
        activeRadio->setChecked(true);

        gridLayout1->addWidget(activeRadio, 0, 0, 1, 1);

        previewFrame = new qdesigner_internal::PreviewFrame(GroupBox126);
        previewFrame->setObjectName(QString::fromUtf8("previewFrame"));
        sizePolicy.setHeightForWidth(previewFrame->sizePolicy().hasHeightForWidth());
        previewFrame->setSizePolicy(sizePolicy);

        gridLayout1->addWidget(previewFrame, 1, 0, 1, 3);


        vboxLayout->addWidget(GroupBox126);

        buttonBox = new QDialogButtonBox(qdesigner_internal__PaletteEditor);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(qdesigner_internal__PaletteEditor);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__PaletteEditor, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__PaletteEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(qdesigner_internal__PaletteEditor);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__PaletteEditor)
    {
        qdesigner_internal__PaletteEditor->setWindowTitle(QApplication::translate("qdesigner_internal::PaletteEditor", "Edit Palette", 0, QApplication::UnicodeUTF8));
        advancedBox->setTitle(QApplication::translate("qdesigner_internal::PaletteEditor", "Tune Palette", 0, QApplication::UnicodeUTF8));
        buildButton->setText(QString());
        detailsRadio->setText(QApplication::translate("qdesigner_internal::PaletteEditor", "Show Details", 0, QApplication::UnicodeUTF8));
        computeRadio->setText(QApplication::translate("qdesigner_internal::PaletteEditor", "Compute Details", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("qdesigner_internal::PaletteEditor", "Quick", 0, QApplication::UnicodeUTF8));
        GroupBox126->setTitle(QApplication::translate("qdesigner_internal::PaletteEditor", "Preview", 0, QApplication::UnicodeUTF8));
        disabledRadio->setText(QApplication::translate("qdesigner_internal::PaletteEditor", "Disabled", 0, QApplication::UnicodeUTF8));
        inactiveRadio->setText(QApplication::translate("qdesigner_internal::PaletteEditor", "Inactive", 0, QApplication::UnicodeUTF8));
        activeRadio->setText(QApplication::translate("qdesigner_internal::PaletteEditor", "Active", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class PaletteEditor: public Ui_PaletteEditor {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // PALETTEEDITOR_H
