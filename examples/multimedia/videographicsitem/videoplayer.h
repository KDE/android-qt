/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QtGui/QMovie>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractVideoSurface;
class QSlider;
QT_END_NAMESPACE


class VideoItem;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~VideoPlayer();

    QSize sizeHint() const { return QSize(800, 600); }

public slots:
    void openFile();
    void play();

private slots:
    void movieStateChanged(QMovie::MovieState state);
    void frameChanged(int frame);
    void setPosition(int frame);
    void rotateVideo(int angle);

private:
    bool presentImage(const QImage &image);

    QMovie movie;
    VideoItem *videoItem;
    QAbstractButton *playButton;
    QSlider *positionSlider;
};

#endif

