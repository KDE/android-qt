/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QVFBHDR_H
#define QVFBHDR_H

#include <QtGui/qcolor.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_QWS_TEMP_DIR
#define QT_QWS_TEMP_DIR "/tmp"
#endif

#ifdef QT_PRIVATE_QWS
#define QT_VFB_DATADIR(DISPLAY) QString("%1/qtembedded-%2-%3") \
                                .arg(QT_QWS_TEMP_DIR).arg(getuid()).arg(DISPLAY)

#define QT_VFB_MOUSE_PIPE(DISPLAY)    QT_VFB_DATADIR(DISPLAY) \
                                      .append("/qtvfb_mouse")
#define QT_VFB_KEYBOARD_PIPE(DISPLAY) QT_VFB_DATADIR(DISPLAY) \
                                      .append("/qtvfb_keyboard")
#define QT_VFB_MAP(DISPLAY)           QT_VFB_DATADIR(DISPLAY) \
                                      .append("/qtvfb_map")
#define QT_VFB_SOUND_PIPE(DISPLAY)    QT_VFB_DATADIR(DISPLAY) \
                                      .append("/qt_soundserver")
#define QTE_PIPE(DISPLAY)             QT_VFB_DATADIR(DISPLAY) \
                                      .append("/QtEmbedded")
#define QTE_PIPE_QVFB(DISPLAY)        QTE_PIPE(DISPLAY)
#else
#define QT_VFB_DATADIR(DISPLAY) QString("%1/qtembedded-%2") \
                                .arg(QT_QWS_TEMP_DIR).arg(DISPLAY)
#define QT_VFB_MOUSE_PIPE(DISPLAY)    QString("/tmp/.qtvfb_mouse-%1").arg(DISPLAY)
#define QT_VFB_KEYBOARD_PIPE(DISPLAY) QString("/tmp/.qtvfb_keyboard-%1").arg(DISPLAY)
#define QT_VFB_MAP(DISPLAY)           QString("/tmp/.qtvfb_map-%1").arg(DISPLAY)
#define QT_VFB_SOUND_PIPE(DISPLAY)    QString("/tmp/.qt_soundserver-%1").arg(DISPLAY)
#define QTE_PIPE(DISPLAY)             QT_VFB_DATADIR(DISPLAY)   \
                                      .append("/QtEmbedded-%1") \
                                      .arg(DISPLAY)
#define QTE_PIPE_QVFB(DISPLAY)        QString("/tmp/qtembedded-%1/QtEmbedded-%1") \
                                      .arg(DISPLAY)
#endif

struct QVFbHeader
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    QRect update;
    bool dirty;
    int  numcols;
    QRgb clut[256];
    int viewerVersion;
    int serverVersion;
    int brightness; // since 4.4.0
    WId windowId; // since 4.5.0
};

struct QVFbKeyData
{
    unsigned int keycode;
    Qt::KeyboardModifiers modifiers;
    unsigned short int unicode;
    bool press;
    bool repeat;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QVFBHDR_H
