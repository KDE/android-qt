#include "qpixmap_blitter_p.h"

#include <qpainter.h>

#include <private/qapplication_p.h>
#include <private/qgraphicssystem_p.h>

QBlittablePixmapData::QBlittablePixmapData(QPixmapData::PixelType type)
    : QPixmapData(type,BlitterClass), m_engine(0), m_blittable(0)
{
}

QBlittablePixmapData::~QBlittablePixmapData()
{
    delete m_blittable;
    delete m_engine;
}

QBlittable *QBlittablePixmapData::blittable() const
{
    if (!m_blittable) {
        QBlittablePixmapData *that = const_cast<QBlittablePixmapData *>(this);
        that->m_blittable = QApplicationPrivate::graphicsSystem()->createBlittable(QRect(0,0,w,h));
        that->setSerialNumber(m_blittable->lock()->serialNumber());
    }

    return m_blittable;
}

void QBlittablePixmapData::setBlittable(QBlittable *blittable)
{
    resize(blittable->rect().width(),blittable->rect().height());
    m_blittable = blittable;
}

void QBlittablePixmapData::resize(int width, int height)
{

    delete m_blittable;
    m_blittable = 0;
    delete m_engine;
    m_engine = 0;
    d = QApplicationPrivate::graphicsSystem()->screens().at(0)->depth();
    w = width;
    h = height;
    is_null = (w <= 0 || h <= 0);
}
extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

int QBlittablePixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    switch (metric) {
    case QPaintDevice::PdmWidth:
        return w;
    case QPaintDevice::PdmHeight:
        return h;
    case QPaintDevice::PdmWidthMM:
        return qRound(w * 25.4 / qt_defaultDpiX());
    case QPaintDevice::PdmHeightMM:
        return qRound(h * 25.4 / qt_defaultDpiY());
    case QPaintDevice::PdmDepth:
        return 32;
    case QPaintDevice::PdmDpiX: // fall-through
    case QPaintDevice::PdmPhysicalDpiX:
        return qt_defaultDpiX();
    case QPaintDevice::PdmDpiY: // fall-through
    case QPaintDevice::PdmPhysicalDpiY:
        return qt_defaultDpiY();
    default:
        qWarning("QRasterPixmapData::metric(): Unhandled metric type %d", metric);
        break;
    }

    return 0;
}

void QBlittablePixmapData::fill(const QColor &color)
{
    if (color.alpha() == 255 && blittable()->capabilities() & QBlittable::SolidRectCapability) {
        blittable()->unlock();
        blittable()->fillRect(QRectF(0,0,w,h),color);
    }else
        blittable()->lock()->fill(color.rgba());
}

QImage *QBlittablePixmapData::buffer()
{
    return blittable()->lock();
}

QImage QBlittablePixmapData::toImage() const
{
    return blittable()->lock()->copy();
}

bool QBlittablePixmapData::hasAlphaChannel() const
{
    return blittable()->lock()->hasAlphaChannel();
}

void QBlittablePixmapData::fromImage(const QImage &image,
                                     Qt::ImageConversionFlags flags)
{
    resize(image.width(),image.height());
    QImage *thisImg = buffer();

    QImage correctFormatPic = image;
    if (correctFormatPic.format() != thisImg->format())
        correctFormatPic = correctFormatPic.convertToFormat(thisImg->format(), flags);

    //jl: This does not ALWAYS work as expected :(
//    QPainter p(thisImg);
//    p.setCompositionMode(QPainter::CompositionMode_Source);
//    p.drawImage(0,0,image,flags);

    //So just copy strides by hand
    uchar *mem = thisImg->bits();
    const uchar *bits = correctFormatPic.bits();
    int bytesCopied = 0;
    while (bytesCopied < correctFormatPic.byteCount()) {
        memcpy(mem,bits,correctFormatPic.bytesPerLine());
        mem += thisImg->bytesPerLine();
        bits += correctFormatPic.bytesPerLine();
        bytesCopied+=correctFormatPic.bytesPerLine();
    }
}

QPaintEngine *QBlittablePixmapData::paintEngine() const
{
    if (!m_engine) {
        QBlittablePixmapData *that = const_cast<QBlittablePixmapData *>(this);
        that->m_engine = new QBlitterPaintEngine(that);
    }
    return m_engine;
}
