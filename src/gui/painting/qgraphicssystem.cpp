/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qgraphicssystem_p.h"

#ifdef Q_WS_X11
# include <private/qpixmap_x11_p.h>
#endif
#if defined(Q_WS_WIN)
# include <private/qpixmap_raster_p.h>
#endif
#ifdef Q_WS_MAC
# include <private/qpixmap_mac_p.h>
#endif
#ifdef Q_WS_LITE
# include <private/qpixmap_raster_p.h>
# include <qapplication.h>
# include <qdesktopwidget.h>
#endif
#ifdef Q_WS_S60
# include <private/qpixmap_s60_p.h>
#endif

QT_BEGIN_NAMESPACE

QGraphicsSystem::~QGraphicsSystem()
{
}

QPixmapData *QGraphicsSystem::createDefaultPixmapData(QPixmapData::PixelType type)
{
#ifdef Q_WS_QWS
    Q_UNUSED(type);
#endif
#if defined(Q_WS_X11)
    return new QX11PixmapData(type);
#elif defined(Q_WS_WIN)
    return new QRasterPixmapData(type);
#elif defined(Q_WS_MAC)
    return new QMacPixmapData(type);
#elif defined(Q_WS_LITE)
    return new QRasterPixmapData(type);
#elif defined(Q_WS_S60)
    return new QS60PixmapData(type);
#elif !defined(Q_WS_QWS)
#error QGraphicsSystem::createDefaultPixmapData() not implemented
#endif
    return 0;
}

#ifdef Q_WS_LITE
QWidget *QGraphicsSystemScreen::topLevelAt(const QPoint & pos) const
{
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i = list.size()-1; i >= 0; --i) {
        QWidget *w = list[i];
        //### mask is ignored
        if (w != QApplication::desktop() && w->isVisible() && w->geometry().contains(pos))
            return w;
    }

    return 0;
}

QList<QGraphicsSystemScreen *> QGraphicsSystem::screens() const
{
    return QList<QGraphicsSystemScreen *>();
}

QPixmap QGraphicsSystem::grabWindow(WId window, int x, int y, int width, int height) const
{
    Q_UNUSED(window);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(width);
    Q_UNUSED(height);
    return QPixmap();
}


QGraphicsSystemScreen::QGraphicsSystemScreen(QObject *parent)
  : QObject(parent)
{}

QGraphicsSystemScreen::~QGraphicsSystemScreen()
{
}

QRect QGraphicsSystemScreen::availableGeometry() const
{
    return geometry();
}

#endif //Q_WS_LITE

QT_END_NAMESPACE
