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

#include "videoplayer.h"
#include "videoitem.h"

#include <QtMultimedia>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

VideoPlayer::VideoPlayer(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , videoItem(0)
    , playButton(0)
    , positionSlider(0)
{
    connect(&movie, SIGNAL(stateChanged(QMovie::MovieState)),
            this, SLOT(movieStateChanged(QMovie::MovieState)));
    connect(&movie, SIGNAL(frameChanged(int)),
            this, SLOT(frameChanged(int)));

    videoItem = new VideoItem;

    QGraphicsScene *scene = new QGraphicsScene(this);
    QGraphicsView *graphicsView = new QGraphicsView(scene);

#ifndef QT_NO_OPENGL
    graphicsView->setViewport(new QGLWidget);
#endif

    scene->addItem(videoItem);

    QSlider *rotateSlider = new QSlider(Qt::Horizontal);
    rotateSlider->setRange(-180,  180);
    rotateSlider->setValue(0);

    connect(rotateSlider, SIGNAL(valueChanged(int)),
            this, SLOT(rotateVideo(int)));

    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    playButton = new QPushButton;
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(playButton, SIGNAL(clicked()),
            this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 0);

    connect(positionSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));

    connect(&movie, SIGNAL(frameChanged(int)),
            positionSlider, SLOT(setValue(int)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(graphicsView);
    layout->addWidget(rotateSlider);
    layout->addLayout(controlLayout);

    setLayout(layout);
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"));

    if (!fileName.isEmpty()) {
        videoItem->stop();

        movie.setFileName(fileName);

        playButton->setEnabled(true);
        positionSlider->setMaximum(movie.frameCount());

        movie.jumpToFrame(0);
    }
}

void VideoPlayer::play()
{
    switch(movie.state()) {
    case QMovie::NotRunning:
        movie.start();
        break;
    case QMovie::Paused:
        movie.setPaused(false);
        break;
    case QMovie::Running:
        movie.setPaused(true);
        break;
    }
}

void VideoPlayer::movieStateChanged(QMovie::MovieState state)
{
    switch(state) {
    case QMovie::NotRunning:
    case QMovie::Paused:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMovie::Running:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    }
}

void VideoPlayer::frameChanged(int frame)
{
    if (!presentImage(movie.currentImage())) {
        movie.stop();
        playButton->setEnabled(false);
        positionSlider->setMaximum(0);
    } else {
        positionSlider->setValue(frame);
    }
}

void VideoPlayer::setPosition(int frame)
{
    movie.jumpToFrame(frame);
}

void VideoPlayer::rotateVideo(int angle)
{
    //rotate around the center of video element
    qreal x = videoItem->boundingRect().width() / 2.0;
    qreal y = videoItem->boundingRect().height() / 2.0;
    videoItem->setTransform(QTransform().translate(x, y).rotate(angle).translate(-x, -y));
}

bool VideoPlayer::presentImage(const QImage &image)
{
    QVideoFrame frame(image);

    if (!frame.isValid())
        return false;

    QVideoSurfaceFormat currentFormat = videoItem->surfaceFormat();

    if (frame.pixelFormat() != currentFormat.pixelFormat()
            || frame.size() != currentFormat.frameSize()) {
        QVideoSurfaceFormat format(frame.size(), frame.pixelFormat());

        if (!videoItem->start(format))
            return false;
    }

    if (!videoItem->present(frame)) {
        videoItem->stop();

        return false;
    } else {
        return true;
    }
}
