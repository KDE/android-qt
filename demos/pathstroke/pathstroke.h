/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef PATHSTROKE_H
#define PATHSTROKE_H

#include "arthurwidgets.h"
#include <QtGui>

class PathStrokeRenderer : public ArthurFrame
{
    Q_OBJECT
    Q_PROPERTY(bool animation READ animation WRITE setAnimation)
    Q_PROPERTY(qreal penWidth READ realPenWidth WRITE setRealPenWidth)
public:
    enum PathMode { CurveMode, LineMode };

    PathStrokeRenderer(QWidget *parent, bool smallScreen = false);

    void paint(QPainter *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);
    bool event(QEvent *e);

    QSize sizeHint() const { return QSize(500, 500); }

    bool animation() const { return m_timer.isActive(); }

    qreal realPenWidth() const { return m_penWidth; }
    void setRealPenWidth(qreal penWidth) { m_penWidth = penWidth; update(); }

signals:
    void clicked();

public slots:
    void setPenWidth(int penWidth) { m_penWidth = penWidth / 10.0; update(); }
    void setAnimation(bool animation);

    void setFlatCap() { m_capStyle = Qt::FlatCap; update(); }
    void setSquareCap() { m_capStyle = Qt::SquareCap; update(); }
    void setRoundCap() { m_capStyle = Qt::RoundCap; update(); }

    void setBevelJoin() { m_joinStyle = Qt::BevelJoin; update(); }
    void setMiterJoin() { m_joinStyle = Qt::MiterJoin; update(); }
    void setRoundJoin() { m_joinStyle = Qt::RoundJoin; update(); }

    void setCurveMode() { m_pathMode = CurveMode; update(); }
    void setLineMode() { m_pathMode = LineMode; update(); }

    void setSolidLine() { m_penStyle = Qt::SolidLine; update(); }
    void setDashLine() { m_penStyle = Qt::DashLine; update(); }
    void setDotLine() { m_penStyle = Qt::DotLine; update(); }
    void setDashDotLine() { m_penStyle = Qt::DashDotLine; update(); }
    void setDashDotDotLine() { m_penStyle = Qt::DashDotDotLine; update(); }
    void setCustomDashLine() { m_penStyle = Qt::NoPen; update(); }

private:
    void initializePoints();
    void updatePoints();

    QBasicTimer m_timer;

    PathMode m_pathMode;

    bool m_wasAnimated;

    qreal m_penWidth;
    int m_pointCount;
    int m_pointSize;
    int m_activePoint;
    QVector<QPointF> m_points;
    QVector<QPointF> m_vectors;

    Qt::PenJoinStyle m_joinStyle;
    Qt::PenCapStyle m_capStyle;

    Qt::PenStyle m_penStyle;

    bool m_smallScreen;
    QPoint m_mousePress;
    bool m_mouseDrag;

    QHash<int, int> m_fingerPointMapping;
};

class PathStrokeControls : public QWidget
{
    Q_OBJECT
public:
    PathStrokeControls(QWidget* parent, PathStrokeRenderer* renderer, bool smallScreen);

signals:
    void okPressed();
    void quitPressed();

private:
    PathStrokeRenderer* m_renderer;

    QGroupBox *m_capGroup;
    QGroupBox *m_joinGroup;
    QGroupBox *m_styleGroup;
    QGroupBox *m_pathModeGroup;

    void createCommonControls(QWidget* parent);
    void layoutForDesktop();
    void layoutForSmallScreens();

private slots:
    void emitQuitSignal();
    void emitOkSignal();

};

class PathStrokeWidget : public QWidget
{
    Q_OBJECT
public:
    PathStrokeWidget(bool smallScreen);
    void setStyle ( QStyle * style );

private:
    PathStrokeRenderer *m_renderer;
    PathStrokeControls *m_controls;

private slots:
    void showControls();
    void hideControls();

};

#endif // PATHSTROKE_H
