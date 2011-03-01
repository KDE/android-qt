/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "customproxy.h"
#include "embeddeddialog.h"

#include <QtGui>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(embeddeddialogs);
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setStickyFocus(true);
#ifndef Q_OS_WINCE
    const int gridSize = 10;
#else
    const int gridSize = 5;
#endif

    for (int y = 0; y < gridSize; ++y) {
        for (int x = 0; x < gridSize; ++x) {
            CustomProxy *proxy = new CustomProxy(0, Qt::Window);
            proxy->setWidget(new EmbeddedDialog);

            QRectF rect = proxy->boundingRect();

            proxy->setPos(x * rect.width() * 1.05, y * rect.height() * 1.05);
            proxy->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

            scene.addItem(proxy);
        }
    }
    scene.setSceneRect(scene.itemsBoundingRect());

    QGraphicsView view(&scene);
    view.scale(0.5, 0.5);
    view.setRenderHints(view.renderHints() | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view.setBackgroundBrush(QPixmap(":/No-Ones-Laughing-3.jpg"));
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.show();
    view.setWindowTitle("Embedded Dialogs Demo");
    return app.exec();
}
