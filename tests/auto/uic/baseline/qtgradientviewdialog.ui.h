/*
*********************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the autotests of the Qt Toolkit.
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading UI file 'qtgradientviewdialog.ui'
**
** Created: Fri Sep 4 10:17:14 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef QTGRADIENTVIEWDIALOG_H
#define QTGRADIENTVIEWDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "qtgradientview.h"

QT_BEGIN_NAMESPACE

class Ui_QtGradientViewDialog
{
public:
    QVBoxLayout *vboxLayout;
    QtGradientView *gradientView;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QtGradientViewDialog)
    {
        if (QtGradientViewDialog->objectName().isEmpty())
            QtGradientViewDialog->setObjectName(QString::fromUtf8("QtGradientViewDialog"));
        QtGradientViewDialog->resize(178, 72);
        vboxLayout = new QVBoxLayout(QtGradientViewDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        gradientView = new QtGradientView(QtGradientViewDialog);
        gradientView->setObjectName(QString::fromUtf8("gradientView"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gradientView->sizePolicy().hasHeightForWidth());
        gradientView->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(gradientView);

        buttonBox = new QDialogButtonBox(QtGradientViewDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(QtGradientViewDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), QtGradientViewDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), QtGradientViewDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QtGradientViewDialog);
    } // setupUi

    void retranslateUi(QDialog *QtGradientViewDialog)
    {
        QtGradientViewDialog->setWindowTitle(QApplication::translate("QtGradientViewDialog", "Select Gradient", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtGradientViewDialog: public Ui_QtGradientViewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // QTGRADIENTVIEWDIALOG_H
