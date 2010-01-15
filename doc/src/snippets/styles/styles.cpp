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

#include <QStyleOption>
#include <QStylePainter>
#include <QWidget>

class MyWidget : public QWidget
{
protected:
    void paintEvent(QPaintEvent *event);
    void paintEvent2(QPaintEvent *event);

};

//! [0] //! [1]
void MyWidget::paintEvent(QPaintEvent * /* event */)
//! [0]
{
//! [2]
    QPainter painter(this);
//! [2]

    QStyleOptionFocusRect option;
    option.initFrom(this);
    option.backgroundColor = palette().color(QPalette::Background);

//! [3]
    style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
//! [3]
}
//! [1]

void MyWidget::paintEvent2(QPaintEvent * /* event */)
//! [4]
{
//! [4] //! [5] //! [6]
    QStylePainter painter(this);
//! [5]

    QStyleOptionFocusRect option;
    option.initFrom(this);
    option.backgroundColor = palette().color(QPalette::Background);

//! [7]
    painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
//! [7]
}
//! [6]

int main()
{
    return 0;
}
