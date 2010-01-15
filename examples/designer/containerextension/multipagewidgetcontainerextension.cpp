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

#include "multipagewidgetcontainerextension.h"
#include "multipagewidget.h"

//! [0]
MultiPageWidgetContainerExtension::MultiPageWidgetContainerExtension(MultiPageWidget *widget,
                                                                     QObject *parent)
    :QObject(parent)
{
    myWidget = widget;
}
//! [0]

//! [1]
void MultiPageWidgetContainerExtension::addWidget(QWidget *widget)
{
    myWidget->addPage(widget);
}
//! [1]

//! [2]
int MultiPageWidgetContainerExtension::count() const
{
    return myWidget->count();
}
//! [2]

//! [3]
int MultiPageWidgetContainerExtension::currentIndex() const
{
    return myWidget->currentIndex();
}
//! [3]

//! [4]
void MultiPageWidgetContainerExtension::insertWidget(int index, QWidget *widget)
{
    myWidget->insertPage(index, widget);
}
//! [4]

//! [5]
void MultiPageWidgetContainerExtension::remove(int index)
{
    myWidget->removePage(index);
}
//! [5]

//! [6]
void MultiPageWidgetContainerExtension::setCurrentIndex(int index)
{
    myWidget->setCurrentIndex(index);
}
//! [6]

//! [7]
QWidget* MultiPageWidgetContainerExtension::widget(int index) const
{
    return myWidget->widget(index);
}
//! [7]
