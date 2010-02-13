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

#include <QTime>
#include <qwindowdefs.h>
#include <QEvent>
#include <QWidget>
#include <QWeakPointer>

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QWindowSystemInterface
{
public:
    static void handleMouseEvent(QWidget *w, const QPoint & local, const QPoint & global, Qt::MouseButtons b) {
        handleMouseEvent(w, eventTime.elapsed(), local, global, b);
    }

    static void handleMouseEvent(QWidget *w, ulong timestamp, const QPoint & local, const QPoint & global, Qt::MouseButtons b);

    static void handleKeyEvent(QWidget *w, QEvent::Type t, int k, Qt::KeyboardModifiers mods, const QString & text = QString(), bool autorep = false, ushort count = 1) {
        handleKeyEvent(w, eventTime.elapsed(), t, k, mods, text, autorep, count);
    }

    static void handleKeyEvent(QWidget *w, ulong timestamp, QEvent::Type t, int k, Qt::KeyboardModifiers mods, const QString & text = QString(), bool autorep = false, ushort count = 1);

    static void handleWheelEvent(QWidget *w, const QPoint & local, const QPoint & global, int d, Qt::Orientation o) {
        handleWheelEvent(w, eventTime.elapsed(), local, global, d, o);
    }

    static void handleWheelEvent(QWidget *w, ulong timestamp, const QPoint & local, const QPoint & global, int d, Qt::Orientation o);

    // delivered directly by the plugin via spontaneous events
    static void handleGeometryChange(QWidget *w, const QRect &newRect);
    static void handleCloseEvent(QWidget *w);
    static void handleEnterEvent(QWidget *w);
    static void handleLeaveEvent(QWidget *w);


    class UserEvent {
    public:
        UserEvent(QWidget * w, ulong time, QEvent::Type t)
            { widget = QWeakPointer<QWidget>::QWeakPointer(w); type = t; timestamp = time; }
        QWeakPointer<QWidget> widget;
        QEvent::Type type;
        unsigned long timestamp;
    };

    class MouseEvent : public UserEvent {
    public:
        MouseEvent(QWidget * w, ulong time, const QPoint & local, const QPoint & global, Qt::MouseButtons b)
            : UserEvent(w, time, QEvent::MouseMove){ localPos = local; globalPos = global; buttons = b; }
        QPoint localPos;
        QPoint globalPos;
        Qt::MouseButtons buttons;
    };

    class WheelEvent : public UserEvent {
    public:
        WheelEvent(QWidget *w, ulong time, const QPoint & local, const QPoint & global, int d, Qt::Orientation o)
            : UserEvent(w, time, QEvent::Wheel) { localPos = local; globalPos = global; delta = d; orient = o; }
        int delta;
        QPoint localPos;
        QPoint globalPos;
        Qt::Orientation orient;
    };

    class KeyEvent : public UserEvent {
    public:
        KeyEvent(QWidget *w, ulong time, QEvent::Type t, int k, Qt::KeyboardModifiers mods, const QString & text = QString(), bool autorep = false, ushort count = 1)
            :UserEvent(w, time, t){ key = k; unicode = text; repeat = autorep; repeatCount = count; modifiers = mods; }
        int key;
        QString unicode;
        bool repeat;
        ushort repeatCount;
        Qt::KeyboardModifiers modifiers;
    };

private:
    static QTime eventTime;

};

class QWindowSystemInterfacePrivate {
public:
    static QList<QWindowSystemInterface::UserEvent *> userEventQueue;

    static int userEventsQueued() { return userEventQueue.count(); }
    static QWindowSystemInterface::UserEvent * getUserEvent() { return userEventQueue.takeFirst(); }
    static void queueUserEvent(QWindowSystemInterface::UserEvent *ev) { userEventQueue.append(ev); }
};
QT_END_NAMESPACE
#endif // QWINDOWSYSTEMINTERFACE_H
