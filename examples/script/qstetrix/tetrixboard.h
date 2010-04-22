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

#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include <QTimer>
#include <QFrame>
#include <QPointer>
#include <QLabel>

class TetrixBoard : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QObject* timer READ getTimer)
    Q_PROPERTY(QWidget* nextPieceLabel READ nextPieceLabel WRITE setNextPieceLabel)

public:
    TetrixBoard(QWidget *parent = 0);

    void setNextPieceLabel(QWidget *label);
    QLabel *nextPieceLabel() const;
    void setBoardWidth(int width);
    void setBoardHeight(int height);
    QSize minimumSizeHint() const;

    QObject *getTimer();

signals:
    void scoreChanged(int score);
    void levelChanged(int level);
    void linesRemovedChanged(int numLines);

#if !defined(Q_MOC_RUN)
private: // can only be emitted by TetrixBoard
#endif
    void keyPressed(int key);
    void paintRequested(QPainter *painter);
    void paintNextPieceRequested(QPainter *painter);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected slots:
    void showNextPiece(int width, int height);
    void drawPauseScreen(QPainter *painter);
    void drawSquare(QPainter *painter, int x, int y, int shape);
    void update();

private:
    enum { BoardWidth = 10, BoardHeight = 22 };

    int squareWidth() { return contentsRect().width() / BoardWidth; }
    int squareHeight() { return contentsRect().height() / BoardHeight; }

    QTimer *timer;
    QPointer<QLabel> nextPieceLbl;
    QImage image;
};

#endif
