/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtGui/qapplication.h>
#include <QtGui/qmainwindow.h>
#include <QtGui/qtabwidget.h>
#include <QtDBus/qdbusconnection.h>
#include "qdbusviewer.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow mw;
#ifndef Q_WS_MAC
    app.setWindowIcon(QIcon(QLatin1String(":/trolltech/qdbusviewer/images/qdbusviewer.png")));
#else
    mw.setWindowTitle(qApp->translate("QtDBusViewer", "Qt D-Bus Viewer"));
#endif


    QTabWidget *mainWidget = new QTabWidget;
    mw.setCentralWidget(mainWidget);
    QDBusViewer *sessionBusViewer = new QDBusViewer(QDBusConnection::sessionBus());
    QDBusViewer *systemBusViewer = new QDBusViewer(QDBusConnection::systemBus());
    mainWidget->addTab(sessionBusViewer, QObject::tr("Session Bus"));
    mainWidget->addTab(systemBusViewer, QObject::tr("System Bus"));

    QMenu *fileMenu = mw.menuBar()->addMenu(QObject::tr("&File"));
    QAction *quitAction = fileMenu->addAction(QObject::tr("&Quit"), &mw, SLOT(close()));
    Q_UNUSED(quitAction);

    QMenu *helpMenu = mw.menuBar()->addMenu(QObject::tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(QObject::tr("&About"));
    aboutAction->setMenuRole(QAction::AboutRole);
    QObject::connect(aboutAction, SIGNAL(triggered()), sessionBusViewer, SLOT(about()));

    QAction *aboutQtAction = helpMenu->addAction(QObject::tr("About &Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    QObject::connect(aboutQtAction, SIGNAL(triggered()), &app, SLOT(aboutQt()));

    mw.show();

    return app.exec();
}

