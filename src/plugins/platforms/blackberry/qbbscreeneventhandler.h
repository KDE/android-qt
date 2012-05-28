/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
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
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBSCREENEVENTHANDLER_H
#define QBBSCREENEVENTHANDLER_H

#include <QWindowSystemInterface>

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QBBScreenEventHandler : public QObject
{
    Q_OBJECT
public:
    QBBScreenEventHandler();

    static void injectKeyboardEvent(int flags, int sym, int mod, int scan, int cap);
    void injectPointerMoveEvent(int x, int y);

    bool handleEvent(screen_event_t event);
    bool handleEvent(screen_event_t event, int qnxType);

Q_SIGNALS:
    void newWindowCreated(screen_window_t window);
    void windowClosed(screen_window_t window);

private:
    void handleKeyboardEvent(screen_event_t event);
    void handlePointerEvent(screen_event_t event);
    void handleTouchEvent(screen_event_t event, int type);
    void handleCloseEvent(screen_event_t event);
    void handleCreateEvent(screen_event_t event);

private:
    enum {
        MAX_TOUCH_POINTS = 10
    };

    QPoint mLastGlobalMousePoint;
    QPoint mLastLocalMousePoint;
    Qt::MouseButtons mLastButtonState;
    void* mLastMouseWindow;

    QWindowSystemInterface::TouchPoint mTouchPoints[MAX_TOUCH_POINTS];
};

QT_END_NAMESPACE

#endif // QBBSCREENEVENTHANDLER_H
