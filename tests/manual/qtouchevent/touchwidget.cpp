/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
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

#include "touchwidget.h"

#include <QApplication>
#include <QtEvents>
#include <QTimer>
#include <QTouchEvent>

void TouchWidget::reset()
{
    acceptTouchBegin
        = acceptTouchUpdate
        = acceptTouchEnd
        = seenTouchBegin
        = seenTouchUpdate
        = seenTouchEnd
        = closeWindowOnTouchEnd

        = acceptMousePress
        = acceptMouseMove
        = acceptMouseRelease
        = seenMousePress
        = seenMouseMove
        = seenMouseRelease
        = closeWindowOnMouseRelease

        = false;
    touchPointCount = 0;
}

bool TouchWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
        if (seenTouchBegin) qWarning("TouchBegin: already seen a TouchBegin");
        if (seenTouchUpdate) qWarning("TouchBegin: TouchUpdate cannot happen before TouchBegin");
        if (seenTouchEnd) qWarning("TouchBegin: TouchEnd cannot happen before TouchBegin");
        seenTouchBegin = !seenTouchBegin && !seenTouchUpdate && !seenTouchEnd;
        touchPointCount = qMax(touchPointCount, static_cast<QTouchEvent *>(event)->touchPoints().count());
        if (acceptTouchBegin) {
            event->accept();
            return true;
        }
        break;
    case QEvent::TouchUpdate:
        if (!seenTouchBegin) qWarning("TouchUpdate: have not seen TouchBegin");
        if (seenTouchEnd) qWarning("TouchUpdate: TouchEnd cannot happen before TouchUpdate");
        seenTouchUpdate = seenTouchBegin && !seenTouchEnd;
        touchPointCount = qMax(touchPointCount, static_cast<QTouchEvent *>(event)->touchPoints().count());
        if (acceptTouchUpdate) {
            event->accept();
            return true;
        }
        break;
    case QEvent::TouchEnd:
        if (!seenTouchBegin) qWarning("TouchEnd: have not seen TouchBegin");
        if (seenTouchEnd) qWarning("TouchEnd: already seen a TouchEnd");
        seenTouchEnd = seenTouchBegin && !seenTouchEnd;
        touchPointCount = qMax(touchPointCount, static_cast<QTouchEvent *>(event)->touchPoints().count());
        if (closeWindowOnTouchEnd)
            window()->close();
        if (acceptTouchEnd) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
        seenMousePress = true;
        if (acceptMousePress) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseMove:
        seenMouseMove = true;
        if (acceptMouseMove) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        seenMouseRelease = true;
        if (closeWindowOnMouseRelease)
            window()->close();        
        if (acceptMouseRelease) {
            event->accept();
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::event(event);
}
