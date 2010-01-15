/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include "valgrind/callgrind.h"

#ifdef Q_WS_X11
extern void qt_x11_wait_for_window_manager(QWidget *);
#endif

class View : public QGraphicsView
{
    Q_OBJECT
public:
    View(QGraphicsScene *scene, QGraphicsItem *item)
        : QGraphicsView(scene), _item(item)
    {
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        static int n = 0;
        if (n)
            CALLGRIND_START_INSTRUMENTATION
        QGraphicsView::paintEvent(event);
        _item->moveBy(1, 1);
        if (n)
            CALLGRIND_STOP_INSTRUMENTATION
        if (++n == 200)
            qApp->quit();
    }

private:
    QGraphicsItem *_item;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (argc < 2) {
        qDebug("usage: ./%s <numItems>", argv[0]);
        return 1;
    }

    QGraphicsScene scene(-150, -150, 300, 300);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    QGraphicsRectItem *item = scene.addRect(-50, -50, 100, 100, QPen(Qt::NoPen), QBrush(Qt::blue));
    item->setFlag(QGraphicsItem::ItemIsMovable);

    for (int i = 0; i < atoi(argv[1]); ++i) {
        QGraphicsRectItem *child = scene.addRect(-5, -5, 10, 10, QPen(Qt::NoPen), QBrush(Qt::blue));
        child->setPos(-50 + qrand() % 100, -50 + qrand() % 100);
        child->setParentItem(item);
    }

    View view(&scene, item);
    view.resize(300, 300);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    return app.exec();
}

#include "main.moc"
