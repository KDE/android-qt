/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FILTERPAGE_H
#define FILTERPAGE_H

#include <QtGui/QWizardPage>
#include "ui_filterpage.h"

QT_BEGIN_NAMESPACE

struct CustomFilter
{
    QString name;
    QStringList filterAttributes;
};

class FilterPage : public QWizardPage
{
    Q_OBJECT

public:
    FilterPage(QWidget *parent = 0);
    QStringList filterAttributes() const;
    QList<CustomFilter> customFilters() const;

private slots:
    void addFilter();
    void removeFilter();

private:
    bool validatePage();

    Ui::FilterPage m_ui;
    QStringList m_filterAttributes;
    QList<CustomFilter> m_customFilters;
};

QT_END_NAMESPACE

#endif
