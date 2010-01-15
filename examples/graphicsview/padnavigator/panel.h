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

#include <QtGui/qgraphicsview.h>

QT_BEGIN_NAMESPACE
class QTimeLine;
class Ui_BackSide;
QT_END_NAMESPACE

class RoundRectItem;

class Panel : public QGraphicsView
{
    Q_OBJECT
public:
    Panel(int width, int height);
    ~Panel();

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void updateSelectionStep(qreal val);
    void updateFlipStep(qreal val);
    void flip();

private:
    QPointF posForLocation(int x, int y) const;

    QGraphicsScene *scene;
    RoundRectItem *selectionItem;
    RoundRectItem *baseItem;
    RoundRectItem *backItem;
    QGraphicsWidget *splash;
    QTimeLine *selectionTimeLine;
    QTimeLine *flipTimeLine;
    int selectedX, selectedY;

    QGraphicsItem ***grid;
    
    QPointF startPos;
    QPointF endPos;
    qreal xrot, yrot;
    qreal xrot2, yrot2;

    int width;
    int height;
    bool flipped;
    bool flipLeft;

    Ui_BackSide *ui;
};
