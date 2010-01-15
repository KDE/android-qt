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

//! [0]
#include <QtUiTools>
//! [0]
#include <QtGui>

#include "calculatorform.h"

//! [1]
CalculatorForm::CalculatorForm(QWidget *parent)
    : QWidget(parent)
{
    QUiLoader loader;

    QFile file(":/forms/calculatorform.ui");
    file.open(QFile::ReadOnly);
    QWidget *formWidget = loader.load(&file, this);
    file.close();
//! [1]

//! [2]
    ui_inputSpinBox1 = qFindChild<QSpinBox*>(this, "inputSpinBox1");
    ui_inputSpinBox2 = qFindChild<QSpinBox*>(this, "inputSpinBox2");
    ui_outputWidget = qFindChild<QLabel*>(this, "outputWidget");
//! [2]

//! [3]
    QMetaObject::connectSlotsByName(this);
//! [3]

//! [4]
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(formWidget);
    setLayout(layout);

    setWindowTitle(tr("Calculator Builder"));
}
//! [4]

//! [5]
void CalculatorForm::on_inputSpinBox1_valueChanged(int value)
{
    ui_outputWidget->setText(QString::number(value + ui_inputSpinBox2->value()));
}
//! [5] //! [6]

//! [6] //! [7]
void CalculatorForm::on_inputSpinBox2_valueChanged(int value)
{
    ui_outputWidget->setText(QString::number(value + ui_inputSpinBox1->value()));
}
//! [7]
