/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY  BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "androidjnimain.h"
#include "qandroidwindowsurface.h"
#include "qandroidplatformintegration.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE


QAndroidWindowSurface::QAndroidWindowSurface(QWidget *window, int winId):QWindowSurface(window)
{
    m_winId=winId;
}

void QAndroidWindowSurface::resize(const QSize & size)
{
    QWindowSurface::resize(size);
    QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();
    if (mImage.size() != size)
        mImage = QImage(size, format);
}

QPaintDevice *QAndroidWindowSurface::paintDevice()
{
    return &mImage;
}

void QAndroidWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    //qDebug()<<offset<<region<<widget->winId();
//    Q_UNUSED(widget);
    Q_UNUSED(widget);

//    qDebug() << "QMinimalWindowSurface::flush()";
//    static int c = 0;
//    QString filename = QString("/sdcard/output%1.png").arg(c++, 4, 10, QLatin1Char('0'));
//    mImage.save(filename);
    QtAndroid::flushImage(m_winId, offset, mImage, region.boundingRect());
}


QT_END_NAMESPACE
