/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONTENT_ITEM_H
#define CONTENT_ITEM_H

#include <QtGui>
#include <QtXml>
#include "demoitem.h"

class HeadingItem;
class DemoTextItem;
class ImageItem;

class ExampleContent : public DemoItem
{

public:
    ExampleContent(const QString &name, QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0){};
    void animationStopped(int id);
    void prepare();

private:
    QString name;
    HeadingItem *heading;
    DemoTextItem *description;
    ImageItem *screenshot;

    QString loadDescription();
    QString extractTextFromParagraph(const QDomNode &parentNode);
    bool isSummary(const QString &text);
    void createContent();
};

#endif // CONTENT_ITEM_H

