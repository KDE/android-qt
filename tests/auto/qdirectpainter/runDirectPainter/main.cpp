/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QApplication>
#include <QCopChannel>
#include <QDirectPainter>
#include <QImage>
#include <QScreen>
#include <QPainter>

int main(int argc, char** argv)
{
#ifdef Q_WS_QWS
    QApplication app(argc, argv);
    QDirectPainter dp;
    QImage screenImage(dp.frameBuffer(), dp.screenWidth(), dp.screenHeight(),
                       dp.linestep(), QScreen::instance()->pixelFormat());
    QPainter p;

    QStringList args = app.arguments();

    dp.setRegion(QRect(args.at(1).toInt(),
                       args.at(2).toInt(),
                       args.at(3).toInt(),
                       args.at(4).toInt()));

    for (;;) {
        dp.startPainting();
        p.begin(&screenImage);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::blue);
        p.drawRects(dp.allocatedRegion().rects());
        p.end();
        dp.endPainting(dp.allocatedRegion());
        app.processEvents();
    }
#else
    Q_UNUSED(argc);
    Q_UNUSED(argv);
#endif

    return 0;
}

