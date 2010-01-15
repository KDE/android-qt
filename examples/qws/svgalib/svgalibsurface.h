/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef SVGALIBSURFACE_H
#define SVGALIBSURFACE_H

#include "svgalibpaintengine.h"
#include "svgalibpaintdevice.h"
#include <private/qwindowsurface_qws_p.h>

class SvgalibPaintDevice;

//! [0]
class SvgalibSurface : public QWSWindowSurface
{
public:
    SvgalibSurface();
    SvgalibSurface(QWidget *w);
    ~SvgalibSurface();

    void setGeometry(const QRect &rect);
    bool isValid() const { return true; }
    bool scroll(const QRegion &region, int dx, int dy);
    QString key() const { return QLatin1String("svgalib"); }

    bool attach(const QByteArray &) { return true; }
    void detach() {}

    QImage image() const { return QImage(); }
    QPaintDevice *paintDevice() { return pdevice; }
    QPoint painterOffset() const;

private:
    SvgalibPaintDevice *pdevice;
};
//! [0]

#endif // SVGALIBSURFACE_H
