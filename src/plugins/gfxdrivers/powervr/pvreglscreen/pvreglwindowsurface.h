/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef PVREGLWINDOWSURFACE_H
#define PVREGLWINDOWSURFACE_H

#include <private/qglwindowsurface_qws_p.h>
#include "pvrqwsdrawable.h"

class PvrEglScreen;

class PvrEglWindowSurface : public QWSGLWindowSurface
{
public:
    PvrEglWindowSurface(QWidget *widget, PvrEglScreen *screen, int screenNum);
    PvrEglWindowSurface();
    ~PvrEglWindowSurface();

    QString key() const { return QLatin1String("PvrEgl"); }

    bool isValid() const;

    void setGeometry(const QRect &rect);
    bool move(const QPoint &offset);

    QByteArray permanentState() const;
    void setPermanentState(const QByteArray &state);

    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);

    QImage image() const;
    QPaintDevice *paintDevice();

    void setDirectRegion(const QRegion &region, int id);

    long nativeDrawable() const { return (long)widget; }

private:
    QWidget *widget;
    PvrQwsDrawable *drawable;
    PvrEglScreen *screen;
    QPaintDevice *pdevice;

    void transformRects(PvrQwsRect *rects, int count) const;
};

#endif
