/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qeventdispatcher_glib_lite_p.h"

#include "qapplication.h"

#include "qplatformdefs.h"
#include "qapplication.h"

#include <glib.h>
#include "qapplication_p.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

struct GUserEventSource
{
    GSource source;
    QLiteEventDispatcherGlib *q;
};

static gboolean userEventSourcePrepare(GSource *s, gint *timeout)
{
    Q_UNUSED(s)
    Q_UNUSED(timeout)

    return QApplicationPrivate::userEventsQueued() > 0;
}

static gboolean userEventSourceCheck(GSource *source)
{
    return userEventSourcePrepare(source, 0);
}

static gboolean userEventSourceDispatch(GSource *s, GSourceFunc, gpointer)
{
    GUserEventSource * source = reinterpret_cast<GUserEventSource *>(s);

    QApplicationPrivate::UserEvent * event;
    while (QApplicationPrivate::userEventsQueued()) {
        event = QApplicationPrivate::getUserEvent();

        // send through event filter
        if (source->q->filterEvent(event)) {
            delete event;
            continue;
        }
        QApplicationPrivate::processUserEvent(event);
        delete event;
    }

    return true;
}


static GSourceFuncs userEventSourceFuncs = {
    userEventSourcePrepare,
    userEventSourceCheck,
    userEventSourceDispatch,
    NULL,
    NULL,
    NULL
};

QLiteEventDispatcherGlibPrivate::QLiteEventDispatcherGlibPrivate(GMainContext *context)
    : QEventDispatcherGlibPrivate(context)
{
    userEventSource = reinterpret_cast<GUserEventSource *>(g_source_new(&userEventSourceFuncs,
                                                                       sizeof(GUserEventSource)));
    userEventSource->q = 0;
    g_source_set_can_recurse(&userEventSource->source, true);
    g_source_attach(&userEventSource->source, mainContext);
}


QLiteEventDispatcherGlib::QLiteEventDispatcherGlib(QObject *parent)
    : QEventDispatcherGlib(*new QLiteEventDispatcherGlibPrivate, parent)
{
    Q_D(QLiteEventDispatcherGlib);
    d->userEventSource->q = this;
}

QLiteEventDispatcherGlib::~QLiteEventDispatcherGlib()
{
    Q_D(QLiteEventDispatcherGlib);

    g_source_destroy(&d->userEventSource->source);
    g_source_unref(&d->userEventSource->source);
    d->userEventSource = 0;
}

bool QLiteEventDispatcherGlib::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    return QEventDispatcherGlib::processEvents(flags);
}

QT_END_NAMESPACE
