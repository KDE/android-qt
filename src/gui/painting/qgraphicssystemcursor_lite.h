/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
#ifndef QGRAPHICSSYSTEMCURSOR_H
#define QGRAPHICSSYSTEMCURSOR_H

#include <QList>
#include <QImage>
#include <QMouseEvent>
#include <QPointer>
#include <QObject>
#include "qgraphicssystem_p.h"

QT_BEGIN_NAMESPACE

// Cursor graphics management
class Q_GUI_EXPORT QGraphicsSystemCursorImage {
public:
    QGraphicsSystemCursorImage(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY)
    { set(data, mask, width, height, hotX, hotY); }
    QImage * image() { return &cursorImage; }
    QPoint hotspot() { return hot; }
    void set(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY);
    void set(const QImage * image, int hx, int hy);
    void set(Qt::CursorShape);
private:
    static void createSystemCursor(int id);
    QImage cursorImage;
    QPoint hot;
};

class Q_GUI_EXPORT QGraphicsSystemCursor : public QObject {
public:
    QGraphicsSystemCursor(QGraphicsSystemScreen *);

    // input methods
    virtual void pointerEvent(const QMouseEvent & event) { Q_UNUSED(event); }
    virtual void changeCursor(QCursor * widgetCursor, QWidget * widget) = 0;

    static QPointer<QGraphicsSystemCursor> getInstance() { return instance; }

protected:
    static QPointer<QGraphicsSystemCursor> instance;    // limit 1 cursor at a time

    QGraphicsSystemScreen * screen;  // Where to request an update
};

QT_END_NAMESPACE

#endif // QGRAPHICSSYSTEMCURSOR_H
