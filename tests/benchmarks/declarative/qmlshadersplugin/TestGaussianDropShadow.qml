/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
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

import QtQuick 1.0
import Qt.labs.shaders 1.0

Item {
    id: main
    width: 360
    height: 640

    property bool liveShadows: true
    property real r: 0

    NumberAnimation on r {
        loops: Animation.Infinite
        from: 0
        to: 360
        duration: 3000
    }

    Image {
        id: background
        width: main.width
        height: main.height
        fillMode: Image.Tile
        source: "bg.jpg"
    }

    GaussianDropShadow {
        x: image1.x + 50
        y: image1.y + 50
        width: image1.width
        height: image1.height
        shadowColor: "#88000000"
        source: ShaderEffectSource { sourceItem: image1; hideSource: false; sourceRect: Qt.rect(-10, -10, image1.width + 20, image1.height + 20) }
        radius: 12.0
        deviation: 12
        rotation: r
    }

    Image {
        id: image1
        anchors.fill: parent
        source: "drop_shadow_small.png"
        smooth: true
        rotation: r
    }

    GaussianDropShadow {
        x: image2.x + 50
        y: image2.y + 50
        width: image2.width
        height: image2.height
        shadowColor: "#88000000"
        source: ShaderEffectSource { sourceItem: image2; hideSource: false; sourceRect: Qt.rect(-10, -10, image2.width + 20, image2.height + 20) }
        radius: 12.0
        deviation: 12
        rotation: -r
    }

    Image {
        id: image2
        anchors.fill: parent
        source: "drop_shadow_small.png"
        smooth: true
        rotation: -r
    }
}
