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

#include "printpanel.h"

//! [0]
PrintPanel::PrintPanel(QWidget *parent)
    : QWidget(parent)
{
/*
    QLabel *label = new QLabel(tr("<b>TROLL PRINT</b>"));
    label->setAlignment(Qt::AlignCenter);
*/
//! [0]

//! [1]
    twoSidedGroupBox = new QGroupBox(tr("2-sided"));
    twoSidedEnabledRadio = new QRadioButton(tr("Enabled"));
    twoSidedDisabledRadio = new QRadioButton(tr("Disabled"));
//! [1] //! [2]
    twoSidedDisabledRadio->setChecked(true);

    colorsGroupBox = new QGroupBox(tr("Colors"));
    colorsEnabledRadio = new QRadioButton(tr("Enabled"));
    colorsDisabledRadio = new QRadioButton(tr("Disabled"));
//! [2]
    colorsDisabledRadio->setChecked(true);

    QHBoxLayout *twoSidedLayout = new QHBoxLayout;
    twoSidedLayout->addWidget(twoSidedEnabledRadio);
    twoSidedLayout->addWidget(twoSidedDisabledRadio);
    twoSidedGroupBox->setLayout(twoSidedLayout);

    QHBoxLayout *colorsLayout = new QHBoxLayout;
    colorsLayout->addWidget(colorsEnabledRadio);
    colorsLayout->addWidget(colorsDisabledRadio);
    colorsGroupBox->setLayout(colorsLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
/*
    mainLayout->addWidget(label);
*/
    mainLayout->addWidget(twoSidedGroupBox);
    mainLayout->addWidget(colorsGroupBox);
    setLayout(mainLayout);
}
