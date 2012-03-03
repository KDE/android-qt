/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qdirectfbscreen.h"
#include "qdirectfbcursor.h"

QT_BEGIN_NAMESPACE

QDirectFbScreen::QDirectFbScreen(int display)
    : QPlatformScreen()
    , m_layer(QDirectFbConvenience::dfbDisplayLayer(display))
{
    m_layer->SetCooperativeLevel(m_layer.data(), DLSCL_SHARED);

    DFBDisplayLayerConfig config;
    m_layer->GetConfiguration(m_layer.data(), &config);

    m_format = QDirectFbConvenience::imageFormatFromSurfaceFormat(config.pixelformat, config.surface_caps);
    m_geometry = QRect(0, 0, config.width, config.height);
    const int dpi = 72;
    const qreal inch = 25.4;
    m_depth = QDirectFbConvenience::colorDepthForSurface(config.pixelformat);
    m_physicalSize = QSize(config.width, config.height) * inch / dpi;

    m_cursor.reset(new QDirectFBCursor(this));
}

IDirectFBDisplayLayer *QDirectFbScreen::dfbLayer() const
{
    return m_layer.data();
}


QT_END_NAMESPACE
