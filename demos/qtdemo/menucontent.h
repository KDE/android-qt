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

#ifndef MENU_CONTENT_ITEM_H
#define MENU_CONTENT_ITEM_H

#include <QtGui>
#include <QtXml>
#include "demoitem.h"

class HeadingItem;
class DemoTextItem;

class MenuContentItem : public DemoItem
{

public:
    MenuContentItem(const QDomElement &el, QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const; // overridden
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0){}; // overridden
    void animationStopped(int id);
    void prepare();

private:
    QString name;
    QString readmePath;
    HeadingItem *heading;
    DemoTextItem *description1;
    DemoTextItem *description2;

    QString loadDescription(int startPara, int nrPara);
    QString extractTextFromParagraph(const QDomNode &parentNode);

    void createContent();
};

#endif // MENU_CONTENT_ITEM_H

