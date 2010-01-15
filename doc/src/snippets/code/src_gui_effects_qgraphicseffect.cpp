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
MyGraphicsOpacityEffect::draw(QPainter *painter)
{
    // Fully opaque; draw directly without going through a pixmap.
    if (qFuzzyCompare(m_opacity, 1)) {
        drawSource(painter);
        return;
    }
    ...
}
//! [0]

//! [1]
MyGraphicsEffect::draw(QPainter *painter)
{
    ...
    QPoint offset;
    if (sourceIsPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset);
        ...
        painter->drawPixmap(offset, pixmap);
    } else {
        // Draw pixmap in device coordinates to avoid pixmap scaling;
        const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);
        painter->setWorldTransform(QTransform());
        ...
        painter->drawPixmap(offset, pixmap);
    }
    ...
}
//! [1]

//! [2]
...
QLinearGradient alphaGradient(rect.topLeft(), rect.bottomLeft());
alphaGradient.setColorAt(0.0, Qt::transparent);
alphaGradient.setColorAt(0.5, Qt::black);
alphaGradient.setColorAt(1.0, Qt::transparent);
QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
effect->setOpacityMask(alphaGradient);
...
//! [2]
