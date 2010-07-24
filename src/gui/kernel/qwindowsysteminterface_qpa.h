/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#ifndef QWINDOWSYSTEMINTERFACE_H
#define QWINDOWSYSTEMINTERFACE_H

#include <QtCore/QTime>
#include <QtGui/qwindowdefs.h>
#include <QtCore/QEvent>
#include <QtGui/QWidget>
#include <QtCore/QWeakPointer>
#include <QtCore/QMutex>
#include <QtGui/QTouchEvent>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QWindowSystemInterface
{
public:
    static void handleMouseEvent(QWidget *w, const QPoint & local, const QPoint & global, Qt::MouseButtons b);
    static void handleMouseEvent(QWidget *w, ulong timestamp, const QPoint & local, const QPoint & global, Qt::MouseButtons b);

    static void handleKeyEvent(QWidget *w, QEvent::Type t, int k, Qt::KeyboardModifiers mods, const QString & text = QString(), bool autorep = false, ushort count = 1);
    static void handleKeyEvent(QWidget *w, ulong timestamp, QEvent::Type t, int k, Qt::KeyboardModifiers mods, const QString & text = QString(), bool autorep = false, ushort count = 1);

    static void handleWheelEvent(QWidget *w, const QPoint & local, const QPoint & global, int d, Qt::Orientation o);
    static void handleWheelEvent(QWidget *w, ulong timestamp, const QPoint & local, const QPoint & global, int d, Qt::Orientation o);

    struct TouchPoint {
        int id;                 // for application use
        bool isPrimary;         // for application use
        QPointF normalPosition; // touch device coordinates, (0 to 1, 0 to 1)
        QRectF area;            // the touched area, centered at position in screen coordinates
        qreal pressure;         // 0 to 1
        Qt::TouchPointStates state; //Qt::TouchPoint{Pressed|Moved|Stationary|Released}
    };

    static void handleTouchEvent(QWidget *w, QEvent::Type type, QTouchEvent::DeviceType devType, const QList<struct TouchPoint> &points);
    static void handleTouchEvent(QWidget *w, ulong timestamp, QEvent::Type type, QTouchEvent::DeviceType devType, const QList<struct TouchPoint> &points);

    // delivered directly by the plugin via spontaneous events
    static void handleGeometryChange(QWidget *w, const QRect &newRect);
    static void handleCloseEvent(QWidget *w);
    static void handleEnterEvent(QWidget *w);
    static void handleLeaveEvent(QWidget *w);

    // Changes to the screen
    static void handleScreenGeometryChange(int screenIndex);
    static void handleScreenAvailableGeometryChange(int screenIndex);
    static void handleScreenCountChange(int count);
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // QWINDOWSYSTEMINTERFACE_H
