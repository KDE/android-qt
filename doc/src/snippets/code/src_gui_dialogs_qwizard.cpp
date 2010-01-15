/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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
int LicenseWizard::nextId() const
{
    switch (currentId()) {
    case Page_Intro:
        if (field("intro.evaluate").toBool()) {
            return Page_Evaluate;
        } else {
            return Page_Register;
        }
    case Page_Evaluate:
        return Page_Conclusion;
    case Page_Register:
        if (field("register.upgradeKey").toString().isEmpty()) {
            return Page_Details;
        } else {
            return Page_Conclusion;
        }
    case Page_Details:
        return Page_Conclusion;
    case Page_Conclusion:
    default:
        return -1;
    }
}
//! [0]


//! [1]
MyWizard::MyWizard(QWidget *parent)
    : QWizard(parent)
{
    ...
    QList<QWizard::WizardButton> layout;
    layout << QWizard::Stretch << QWizard::BackButton << QWizard::CancelButton
           << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    ...
}
//! [1]
