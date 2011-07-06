/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFLICKGESTURE_P_H
#define QFLICKGESTURE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qevent.h"
#include "qgesturerecognizer.h"
#include "private/qgesture_p.h"
#include "qscroller.h"
#include "qscopedpointer.h"

#ifndef QT_NO_GESTURES

QT_BEGIN_NAMESPACE

class QFlickGesturePrivate;
class QGraphicsItem;

class Q_GUI_EXPORT QFlickGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFlickGesture)

public:
    QFlickGesture(QObject *receiver, Qt::MouseButton button, QObject *parent = 0);
    ~QFlickGesture();

    friend class QFlickGestureRecognizer;
};

class PressDelayHandler;

class QFlickGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QFlickGesture)
public:
    QFlickGesturePrivate();

    QPointer<QObject> receiver;
    QScroller *receiverScroller;
    Qt::MouseButton button; // NoButton == Touch
    bool macIgnoreWheel;
    static PressDelayHandler *pressDelayHandler;
};

class QFlickGestureRecognizer : public QGestureRecognizer
{
public:
    QFlickGestureRecognizer(Qt::MouseButton button);

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);

private:
    Qt::MouseButton button; // NoButton == Touch
};

QT_END_NAMESPACE

#endif // QT_NO_GESTURES

#endif // QFLICKGESTURE_P_H
