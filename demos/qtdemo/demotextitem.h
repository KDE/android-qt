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

#ifndef DEMO_TEXT_ITEM_H
#define DEMO_TEXT_ITEM_H

#include <QtGui>
#include "demoitem.h"

class DemoTextItem : public DemoItem
{
public:
    enum TYPE {STATIC_TEXT, DYNAMIC_TEXT};

    DemoTextItem(const QString &text, const QFont &font, const QColor &textColor,
        float textWidth, QGraphicsScene *scene = 0, QGraphicsItem *parent = 0, TYPE type = STATIC_TEXT, const QColor &bgColor = QColor());
    void setText(const QString &text);
    QRectF boundingRect() const; // overridden
    void animationStarted(int id = 0);
    void animationStopped(int id = 0);

protected:
    virtual QImage *createImage(const QMatrix &matrix) const; // overridden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0); // overridden

private:
    float textWidth;
    QString text;
    QFont font;
    QColor textColor;
    QColor bgColor;
    TYPE type;
};

#endif // DEMO_TEXT_ITEM_H

