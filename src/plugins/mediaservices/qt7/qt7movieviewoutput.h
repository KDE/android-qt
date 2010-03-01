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

#ifndef QT7MOVIEVIEWOUTPUT_H
#define QT7MOVIEVIEWOUTPUT_H

#include <QtCore/qobject.h>

#include <QtMultimedia/qvideowindowcontrol.h>
#include <QtMultimedia/qmediaplayer.h>

#include <QtGui/qmacdefines_mac.h>
#include "qt7videooutputcontrol.h"


QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QT7PlayerSession;
class QT7PlayerService;

class QT7MovieViewOutput : public QT7VideoWindowControl
{
public:
    QT7MovieViewOutput(QObject *parent = 0);
    ~QT7MovieViewOutput();

    void setEnabled(bool);
    void setMovie(void *movie);
    void updateNaturalSize(const QSize &newSize);

    WId winId() const;
    void setWinId(WId id);

    QRect displayRect() const;
    void setDisplayRect(const QRect &rect);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    void repaint();

    QSize nativeSize() const;

    QVideoWidget::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);
    
private:
    void setupVideoOutput();

    void *m_movie;
    void *m_movieView;

    WId m_winId;
    QRect m_displayRect;
    bool m_fullscreen;
    QSize m_nativeSize;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
