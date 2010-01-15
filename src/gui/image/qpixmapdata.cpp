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

#include "qpixmapdata_p.h"
#include <QtCore/qbuffer.h>
#include <QtGui/qbitmap.h>
#include <QtGui/qimagereader.h>
#include <private/qgraphicssystem_p.h>
#include <private/qapplication_p.h>

QT_BEGIN_NAMESPACE

const uchar qt_pixmap_bit_mask[] = { 0x01, 0x02, 0x04, 0x08,
                                     0x10, 0x20, 0x40, 0x80 };

QPixmapData *QPixmapData::create(int w, int h, PixelType type)
{
    QPixmapData *data;
    QGraphicsSystem* gs = QApplicationPrivate::graphicsSystem();
    if (gs)
        data = gs->createPixmapData(static_cast<QPixmapData::PixelType>(type));
    else
        data = QGraphicsSystem::createDefaultPixmapData(static_cast<QPixmapData::PixelType>(type));
    data->resize(w, h);
    return data;
}


QPixmapData::QPixmapData(PixelType pixelType, int objectId)
    : w(0),
      h(0),
      d(0),
      is_null(true),
      ref(0),
      detach_no(0),
      type(pixelType),
      id(objectId),
      ser_no(0),
      is_cached(false)
{
}

QPixmapData::~QPixmapData()
{
}

QPixmapData *QPixmapData::createCompatiblePixmapData() const
{
    QPixmapData *d;
    QGraphicsSystem *gs = QApplicationPrivate::graphicsSystem();
    if (gs)
        d = gs->createPixmapData(pixelType());
    else
        d = QGraphicsSystem::createDefaultPixmapData(pixelType());
    return d;
}

static QImage makeBitmapCompliantIfNeeded(QPixmapData *d, const QImage &image, Qt::ImageConversionFlags flags)
{
    if (d->pixelType() == QPixmapData::BitmapType) {
        QImage img = image.convertToFormat(QImage::Format_MonoLSB, flags);

        // make sure image.color(0) == Qt::color0 (white)
        // and image.color(1) == Qt::color1 (black)
        const QRgb c0 = QColor(Qt::black).rgb();
        const QRgb c1 = QColor(Qt::white).rgb();
        if (img.color(0) == c0 && img.color(1) == c1) {
            img.invertPixels();
            img.setColor(0, c1);
            img.setColor(1, c0);
        }
        return img;
    }

    return image;
}

bool QPixmapData::fromFile(const QString &fileName, const char *format,
                           Qt::ImageConversionFlags flags)
{
    QImage image = QImageReader(fileName, format).read();
    if (image.isNull())
        return false;
    fromImage(makeBitmapCompliantIfNeeded(this, image, flags), flags);
    return !isNull();
}

bool QPixmapData::fromData(const uchar *buf, uint len, const char *format, Qt::ImageConversionFlags flags)
{
    QByteArray a = QByteArray::fromRawData(reinterpret_cast<const char *>(buf), len);
    QBuffer b(&a);
    b.open(QIODevice::ReadOnly);
    QImage image = QImageReader(&b, format).read();
    fromImage(makeBitmapCompliantIfNeeded(this, image, flags), flags);
    return !isNull();
}

void QPixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    fromImage(data->toImage().copy(rect), Qt::AutoColor);
}

bool QPixmapData::scroll(int dx, int dy, const QRect &rect)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    Q_UNUSED(rect);
    return false;
}

void QPixmapData::setMask(const QBitmap &mask)
{
    if (mask.size().isEmpty()) {
        if (depth() != 1)
            fromImage(toImage().convertToFormat(QImage::Format_RGB32),
                      Qt::AutoColor);
    } else {
        QImage image = toImage();
        const int w = image.width();
        const int h = image.height();

        switch (image.depth()) {
        case 1: {
            const QImage imageMask = mask.toImage().convertToFormat(image.format());
            for (int y = 0; y < h; ++y) {
                const uchar *mscan = imageMask.scanLine(y);
                uchar *tscan = image.scanLine(y);
                int bytesPerLine = image.bytesPerLine();
                for (int i = 0; i < bytesPerLine; ++i)
                    tscan[i] &= mscan[i];
            }
            break;
        }
        default: {
            const QImage imageMask = mask.toImage().convertToFormat(QImage::Format_MonoLSB);
            image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            for (int y = 0; y < h; ++y) {
                const uchar *mscan = imageMask.scanLine(y);
                QRgb *tscan = (QRgb *)image.scanLine(y);
                for (int x = 0; x < w; ++x) {
                    if (!(mscan[x>>3] & qt_pixmap_bit_mask[x&7]))
                        tscan[x] = 0;
                }
            }
            break;
        }
        }
        fromImage(image, Qt::AutoColor);
    }
}

QBitmap QPixmapData::mask() const
{
    if (!hasAlphaChannel())
        return QBitmap();

    const QImage img = toImage();
    const QImage image = (img.depth() < 32 ? img.convertToFormat(QImage::Format_ARGB32_Premultiplied) : img);
    const int w = image.width();
    const int h = image.height();

    QImage mask(w, h, QImage::Format_MonoLSB);
    if (mask.isNull()) // allocation failed
        return QBitmap();

    mask.setColorCount(2);
    mask.setColor(0, QColor(Qt::color0).rgba());
    mask.setColor(1, QColor(Qt::color1).rgba());

    const int bpl = mask.bytesPerLine();

    for (int y = 0; y < h; ++y) {
        const QRgb *src = reinterpret_cast<const QRgb*>(image.scanLine(y));
        uchar *dest = mask.scanLine(y);
        memset(dest, 0, bpl);
        for (int x = 0; x < w; ++x) {
            if (qAlpha(*src) > 0)
                dest[x >> 3] |= qt_pixmap_bit_mask[x & 7];
            ++src;
        }
    }

    return QBitmap::fromImage(mask);
}

QPixmap QPixmapData::transformed(const QTransform &matrix,
                                 Qt::TransformationMode mode) const
{
    return QPixmap::fromImage(toImage().transformed(matrix, mode));
}

void QPixmapData::setAlphaChannel(const QPixmap &alphaChannel)
{
    QImage image = toImage();
    image.setAlphaChannel(alphaChannel.toImage());
    fromImage(image, Qt::AutoColor);
}

QPixmap QPixmapData::alphaChannel() const
{
    return QPixmap::fromImage(toImage().alphaChannel());
}

void QPixmapData::setSerialNumber(int serNo)
{
    ser_no = serNo;
}

QImage* QPixmapData::buffer()
{
    return 0;
}

#if defined(Q_OS_SYMBIAN)
void* QPixmapData::toNativeType(NativeType /* type */)
{
    return 0;
}

void QPixmapData::fromNativeType(void* /* pixmap */, NativeType /* typre */)
{
    return;
}
#endif

QT_END_NAMESPACE
