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

#include "window.h"
#include "colorlisteditor.h"

//! [0]
Window::Window()
{
    QItemEditorFactory *factory = new QItemEditorFactory;

    QItemEditorCreatorBase *colorListCreator =
        new QStandardItemEditorCreator<ColorListEditor>();

    factory->registerEditor(QVariant::Color, colorListCreator);

    QItemEditorFactory::setDefaultFactory(factory);

    createGUI();
}
//! [0]

void Window::createGUI()
{
    QList<QPair<QString, QColor> > list;
    list << QPair<QString, QColor>(tr("Alice"), QColor("aliceblue")) <<
            QPair<QString, QColor>(tr("Neptun"), QColor("aquamarine")) <<
            QPair<QString, QColor>(tr("Ferdinand"), QColor("springgreen"));

    QTableWidget *table = new QTableWidget(3, 2);
    table->setHorizontalHeaderLabels(QStringList() << tr("Name")
                                                   << tr("Hair Color"));
    table->verticalHeader()->setVisible(false);
    table->resize(150, 50);

    for (int i = 0; i < 3; ++i) {
        QPair<QString, QColor> pair = list.at(i);

        QTableWidgetItem *nameItem = new QTableWidgetItem(pair.first);
        QTableWidgetItem *colorItem = new QTableWidgetItem;
        colorItem->setData(Qt::DisplayRole, pair.second);

        table->setItem(i, 0, nameItem);
        table->setItem(i, 1, colorItem);
    }
    table->resizeColumnToContents(0);
    table->horizontalHeader()->setStretchLastSection(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(table, 0, 0);

    setLayout(layout);

    setWindowTitle(tr("Color Editor Factory"));
}
