/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtGui>
#include <windows.h>

int main(int argc, char * argv[])
{
    QList<QWidget*> widgets;
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Test");
    QMenu *fileMenu = mainWindow.menuBar()->addMenu("File");
    QMenu *editMenu = mainWindow.menuBar()->addMenu("Edit");
    QMenu *viewMenu = mainWindow.menuBar()->addMenu("View");
    QMenu *toolsMenu = mainWindow.menuBar()->addMenu("Tools");
    QMenu *optionsMenu = mainWindow.menuBar()->addMenu("Options");
    QMenu *helpMenu = mainWindow.menuBar()->addMenu("Help");

    qApp->processEvents();

    fileMenu->addAction("Open");
    QAction *close = fileMenu->addAction("Close");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit");

    close->setEnabled(false);

    editMenu->addAction("Cut");
    editMenu->addAction("Pase");
    editMenu->addAction("Copy");
    editMenu->addSeparator();
    editMenu->addAction("Find");

    viewMenu->addAction("Hide");
    viewMenu->addAction("Show");
    viewMenu->addAction("Explore");
    QAction *visible = viewMenu->addAction("Visible");
    visible->setCheckable(true);
    visible->setChecked(true);

    toolsMenu->addMenu("Hammer");
    toolsMenu->addMenu("Caliper");
    toolsMenu->addMenu("Helm");

    optionsMenu->addMenu("Settings");
    optionsMenu->addMenu("Standard");
    optionsMenu->addMenu("Extended");
    
    QMenu *subMenu = helpMenu->addMenu("Help");
    subMenu->addAction("Index");
    subMenu->addSeparator();
    subMenu->addAction("Vodoo Help");
    helpMenu->addAction("Contens");
    helpMenu->addSeparator();
    helpMenu->addAction("About");

    QToolBar toolbar;
    mainWindow.addToolBar(&toolbar);
    toolbar.addAction(QIcon(qApp->style()->standardPixmap(QStyle::SP_FileIcon)), QString("textAction"));

    QTextEdit textEdit;
    mainWindow.setCentralWidget(&textEdit);

    mainWindow.showMaximized();

    app.exec();
}
