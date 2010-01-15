/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
Q3ScrollView* sv = new Q3ScrollView(...);
QWidget *widget = new QWidget(sv->viewport());
QVBoxLayout *layout = new QVBoxLayout(widget);
addChild(widget);
//! [0]


//! [1]
QLabel* child1 = new QLabel("CHILD", widget);
QLabel* child2 = new QLabel("CHILD", widget);
QLabel* child3 = new QLabel("CHILD", widget);
layout->addWidget(child1);
layout->addWidget(child2);
layout->addWidget(child3);
...
//! [1]


//! [2]
Q3ScrollView* sv = new Q3ScrollView(...);
QLabel* child1 = new QLabel("CHILD", sv->viewport());
sv->addChild(child1);
QLabel* child2 = new QLabel("CHILD", sv->viewport());
sv->addChild(child2);
QLabel* child3 = new QLabel("CHILD", sv->viewport());
sv->addChild(child3);
//! [2]


//! [3]
Q3ScrollView* sv = new Q3ScrollView(...);
sv->enableClipper(true);
QLabel* child1 = new QLabel("CHILD", sv->viewport());
sv->addChild(child1);
QLabel* child2 = new QLabel("CHILD", sv->viewport());
sv->addChild(child2);
QLabel* child3 = new QLabel("CHILD", sv->viewport());
sv->addChild(child3);
//! [3]


//! [4]
{
    // Fill a 40000 by 50000 rectangle at (100000,150000)

    // Calculate the coordinates...
    int x1 = 100000, y1 = 150000;
    int x2 = x1+40000-1, y2 = y1+50000-1;

    // Clip the coordinates so X/Windows will not have problems...
    if (x1 < clipx) x1=clipx;
    if (y1 < clipy) y1=clipy;
    if (x2 > clipx+clipw-1) x2=clipx+clipw-1;
    if (y2 > clipy+cliph-1) y2=clipy+cliph-1;

    // Paint using the small coordinates...
    if (x2 >= x1 && y2 >= y1)
        p->fillRect(x1, y1, x2-x1+1, y2-y1+1, red);
}
//! [4]
