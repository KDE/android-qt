/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef GUIDE_H
#define GUIDE_H

#include "demoitem.h"

class Guide
{
public:
    Guide(Guide *follows = 0);
    virtual ~Guide();

    virtual void guide(DemoItem *item, float moveSpeed) = 0;
    void move(DemoItem *item, QPointF &dest, float moveSpeed);
    virtual QPointF startPos(){ return QPointF(0, 0); };
    virtual QPointF endPos(){ return QPointF(0, 0); };
    virtual float length(){ return 1; };
    float lengthAll();

    void setScale(float scaleX, float scaleY, bool all = true);
    void setFence(const QRectF &fence, bool all = true);

    int startLength;
    Guide *nextGuide;
    Guide *firstGuide;
    Guide *prevGuide;
    float scaleX;
    float scaleY;
    QRectF fence;
};

#endif // GUIDE_H

