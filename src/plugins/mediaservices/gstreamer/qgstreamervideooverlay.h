/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QGSTREAMERVIDEOOVERLAY_H
#define QGSTREAMERVIDEOOVERLAY_H

#include <QtMultimedia/qvideowindowcontrol.h>

#include "qgstreamervideorendererinterface.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QAbstractVideoSurface;
class QX11VideoSurface;

class QGstreamerVideoOverlay : public QVideoWindowControl, public QGstreamerVideoRendererInterface
{
    Q_OBJECT
    Q_INTERFACES(QGstreamerVideoRendererInterface)
public:
    QGstreamerVideoOverlay(QObject *parent = 0);
    ~QGstreamerVideoOverlay();

    WId winId() const;
    void setWinId(WId id);

    QRect displayRect() const;
    void setDisplayRect(const QRect &rect);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    QSize nativeSize() const;

    QVideoWidget::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    void repaint();

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

    QAbstractVideoSurface *surface() const;

    GstElement *videoSink();

private slots:
    void surfaceFormatChanged();

private:
    void setScaledDisplayRect();

    QX11VideoSurface *m_surface;
    GstElement *m_videoSink;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    QRect m_displayRect;
    bool m_fullScreen;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
