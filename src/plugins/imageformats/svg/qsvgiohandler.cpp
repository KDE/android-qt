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

#include "qsvgiohandler.h"

#ifndef QT_NO_SVGRENDERER

#include "qsvgrenderer.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qpainter.h"
#include "qvariant.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

class QSvgIOHandlerPrivate
{
public:
    QSvgIOHandlerPrivate()
        : r(new QSvgRenderer()), loaded(false)
    {}
    ~QSvgIOHandlerPrivate()
    {
        delete r;
    }

    bool load(QIODevice *device);
    static bool findSvgTag(QIODevice *device);

    QSvgRenderer *r;
    QSize         defaultSize;
    QSize         currentSize;
    bool          loaded;
};

bool QSvgIOHandlerPrivate::load(QIODevice *device)
{
    if (loaded)
        return true;

    if (r->load(device->readAll())) {
        defaultSize = QSize(r->viewBox().width(), r->viewBox().height());
        if (currentSize.isEmpty())
            currentSize = defaultSize;
    }
    loaded = r->isValid();

    return loaded;
}

bool QSvgIOHandlerPrivate::findSvgTag(QIODevice *device)
{
    qint64 pos = device->pos();
    device->seek(0);
    char buffer[256];
    const char svg_tag[] = "<svg";

    while (1) {
        int size = device->read(buffer, 256);
        for (int i=0; i<size - 5; ++i) {
            if (!memcmp(buffer + i, svg_tag, 4)) {
                if (buffer[i+4] == ' ' || buffer[i+4] == '\t'
                    || buffer[i+4] == '\n' || buffer[i+4] == '\r')
                {
                    device->seek(pos);
                    return true;
                }
            }
        }
        if (device->atEnd())
            break;
        device->seek(device->pos()-4);
    }
    device->seek(pos);
    return false;
}

QSvgIOHandler::QSvgIOHandler()
    : d(new QSvgIOHandlerPrivate())
{

}


QSvgIOHandler::~QSvgIOHandler()
{
    delete d;
}


bool QSvgIOHandler::canRead() const
{
    return QSvgIOHandlerPrivate::findSvgTag(device());
}


QByteArray QSvgIOHandler::name() const
{
    return "svg";
}


bool QSvgIOHandler::read(QImage *image)
{
    if (d->load(device())) {
        *image = QImage(d->currentSize, QImage::Format_ARGB32_Premultiplied);
        if (!d->currentSize.isEmpty()) {
            image->fill(0x00000000);
            QPainter p(image);
            d->r->render(&p);
            p.end();
        }
        return true;
    }

    return false;
}


QVariant QSvgIOHandler::option(ImageOption option) const
{
    switch(option) {
    case ImageFormat:
        return QImage::Format_ARGB32_Premultiplied;
        break;
    case Size:
        d->load(device());
        return d->defaultSize;
        break;
    case ScaledSize:
        return d->currentSize;
        break;
    default:
        break;
    }
    return QVariant();
}


void QSvgIOHandler::setOption(ImageOption option, const QVariant & value)
{
    switch(option) {
    case Size:
        d->defaultSize = value.toSize();
        d->currentSize = value.toSize();
        break;
    case ScaledSize:
        d->currentSize = value.toSize();
        break;
    default:
        break;
    }
}


bool QSvgIOHandler::supportsOption(ImageOption option) const
{
    switch(option)
    {
    case ImageFormat:
    case Size:
    case ScaledSize:
        return true;
    default:
        break;
    }
    return false;
}

bool QSvgIOHandler::canRead(QIODevice *device)
{
    return QSvgIOHandlerPrivate::findSvgTag(device);
}

QT_END_NAMESPACE

#endif // QT_NO_SVGRENDERER
