/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
    id: gaussianDropShadow

    property color shadowColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
    property variant source: 0
    property real radius: 8
    property real deviation: Math.sqrt(-((radius+1) * (radius+1)) / (2 * Math.log(1.0 / 255.0)))
    property bool live: true

    GaussianBlur {
        id: blur
        anchors.fill: parent
        radius: gaussianDropShadow.radius
        deviation: gaussianDropShadow.deviation
        source: gaussianDropShadow.source
        live: gaussianDropShadow.live
    }

    ShaderEffectItem {
        id: shadow
        property color shadowColor: gaussianDropShadow.shadowColor
        property variant source: ShaderEffectSource { sourceItem: blur; hideSource: true }
        anchors.fill: parent

        fragmentShader:
            "
            varying mediump vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform lowp vec4 shadowColor;

            void main() {
                lowp vec4 sourceColor = texture2D(source, qt_TexCoord0);
                gl_FragColor = mix(vec4(0), shadowColor, sourceColor.a);
            }
            "
    }
}

