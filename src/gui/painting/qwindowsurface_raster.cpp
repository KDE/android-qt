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

#include <qdebug.h>

#include <qglobal.h> // for Q_WS_WIN define (non-PCH)
#ifdef Q_WS_WIN
#include <qlibrary.h>
#include <qt_windows.h>
#endif

#include <QtGui/qpaintdevice.h>
#include <QtGui/qwidget.h>

#include "private/qwindowsurface_raster_p.h"
#include "private/qnativeimage_p.h"
#include "private/qwidget_p.h"

#ifdef Q_WS_X11
#include "private/qpixmap_x11_p.h"
#include "private/qt_x11_p.h"
#include "private/qwidget_p.h"
#include "qx11info_x11.h"
#endif
#include "private/qdrawhelper_p.h"

#ifdef Q_WS_MAC
#include <private/qt_cocoa_helpers_mac_p.h>
#include <QMainWindow>
#include <private/qmainwindowlayout_p.h>
#include <QToolBar>
#endif

QT_BEGIN_NAMESPACE

class QRasterWindowSurfacePrivate
{
public:
    QNativeImage *image;

#ifdef Q_WS_X11
    GC gc;
#ifndef QT_NO_XRENDER
    uint translucentBackground : 1;
#endif
#endif
    uint inSetGeometry : 1;
};

QRasterWindowSurface::QRasterWindowSurface(QWidget *window, bool setDefaultSurface)
    : QWindowSurface(window, setDefaultSurface), d_ptr(new QRasterWindowSurfacePrivate)
{
#ifdef Q_WS_X11
    d_ptr->gc = XCreateGC(X11->display, window->handle(), 0, 0);
#ifndef QT_NO_XRENDER
    d_ptr->translucentBackground = X11->use_xrender
        && window->x11Info().depth() == 32;
#endif
#endif
    d_ptr->image = 0;
    d_ptr->inSetGeometry = false;
    setStaticContentsSupport(true);
}


QRasterWindowSurface::~QRasterWindowSurface()
{
#ifdef Q_WS_X11
    XFreeGC(X11->display, d_ptr->gc);
#endif
    if (d_ptr->image)
        delete d_ptr->image;
}


QPaintDevice *QRasterWindowSurface::paintDevice()
{
    return &d_ptr->image->image;
}

void QRasterWindowSurface::beginPaint(const QRegion &rgn)
{
#if (defined(Q_WS_X11) && !defined(QT_NO_XRENDER)) || (defined(Q_WS_WIN) && !defined(Q_WS_WINCE))
    if (!qt_widget_private(window())->isOpaque && window()->testAttribute(Qt::WA_TranslucentBackground)) {
#if defined(Q_WS_WIN) && !defined(Q_WS_WINCE)
        if (d_ptr->image->image.format() != QImage::Format_ARGB32_Premultiplied)
            prepareBuffer(QImage::Format_ARGB32_Premultiplied, window());
#endif
        QPainter p(&d_ptr->image->image);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        const QVector<QRect> rects = rgn.rects();
        const QColor blank = Qt::transparent;
        for (QVector<QRect>::const_iterator it = rects.begin(); it != rects.end(); ++it) {
            p.fillRect(*it, blank);
        }
    }
#else
    Q_UNUSED(rgn);
#endif
}

void QRasterWindowSurface::flush(QWidget *widget, const QRegion &rgn, const QPoint &offset)
{
    Q_D(QRasterWindowSurface);

    // Not ready for painting yet, bail out. This can happen in
    // QWidget::create_sys()
    if (!d->image || rgn.rectCount() == 0)
        return;

#ifdef Q_WS_WIN
    QRect br = rgn.boundingRect();

#ifndef Q_WS_WINCE
    if (!qt_widget_private(window())->isOpaque
        && window()->testAttribute(Qt::WA_TranslucentBackground)
        && (qt_widget_private(window())->data.window_flags & Qt::FramelessWindowHint))
    {
        QRect r = window()->frameGeometry();
        QPoint frameOffset = qt_widget_private(window())->frameStrut().topLeft();
        QRect dirtyRect = br.translated(offset + frameOffset);

        SIZE size = {r.width(), r.height()};
        POINT ptDst = {r.x(), r.y()};
        POINT ptSrc = {0, 0};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, (int)(255.0 * window()->windowOpacity()), Q_AC_SRC_ALPHA};
        RECT dirty = {dirtyRect.x(), dirtyRect.y(),
            dirtyRect.x() + dirtyRect.width(), dirtyRect.y() + dirtyRect.height()};
        Q_UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &size, d->image->hdc, &ptSrc, 0, &blend, Q_ULW_ALPHA, &dirty};
        ptrUpdateLayeredWindowIndirect(window()->internalWinId(), &info);
    } else
#endif
    {
        QPoint wOffset = qt_qwidget_data(widget)->wrect.topLeft();

        HDC widget_dc = widget->getDC();

        QRect wbr = br.translated(-wOffset);
        BitBlt(widget_dc, wbr.x(), wbr.y(), wbr.width(), wbr.height(),
               d->image->hdc, br.x() + offset.x(), br.y() + offset.y(), SRCCOPY);
        widget->releaseDC(widget_dc);
    }

#ifndef QT_NO_DEBUG
    static bool flush = !qgetenv("QT_FLUSH_WINDOWSURFACE").isEmpty();
    if (flush) {
        SelectObject(qt_win_display_dc(), GetStockObject(BLACK_BRUSH));
        Rectangle(qt_win_display_dc(), 0, 0, d->image->width() + 2, d->image->height() + 2);
        BitBlt(qt_win_display_dc(), 1, 1, d->image->width(), d->image->height(),
               d->image->hdc, 0, 0, SRCCOPY);
    }
#endif

#endif

#ifdef Q_WS_X11
    extern void *qt_getClipRects(const QRegion &r, int &num); // in qpaintengine_x11.cpp
    extern QWidgetData* qt_widget_data(QWidget *);
    QPoint wOffset = qt_qwidget_data(widget)->wrect.topLeft();

    if (widget->window() != window()) {
        XFreeGC(X11->display, d_ptr->gc);
        d_ptr->gc = XCreateGC(X11->display, widget->handle(), 0, 0);
    }

    QRegion wrgn(rgn);
    if (!wOffset.isNull())
        wrgn.translate(-wOffset);
    QRect wbr = wrgn.boundingRect();

    if (wrgn.rectCount() != 1) {
        int num;
        XRectangle *rects = (XRectangle *)qt_getClipRects(wrgn, num);
        XSetClipRectangles(X11->display, d_ptr->gc, 0, 0, rects, num, YXBanded);
    }

    QRect br = rgn.boundingRect().translated(offset);
#ifndef QT_NO_MITSHM
    if (d_ptr->image->xshmpm) {
        XCopyArea(X11->display, d_ptr->image->xshmpm, widget->handle(), d_ptr->gc,
                  br.x(), br.y(), br.width(), br.height(), wbr.x(), wbr.y());
        XSync(X11->display, False);
    } else if (d_ptr->image->xshmimg) {
        const QImage &src = d->image->image;
        br = br.intersected(src.rect());
        XShmPutImage(X11->display, widget->handle(), d_ptr->gc, d_ptr->image->xshmimg,
                     br.x(), br.y(), wbr.x(), wbr.y(), br.width(), br.height(), False);
        XSync(X11->display, False);
    } else
#endif
    {
        const QImage &src = d->image->image;
        br = br.intersected(src.rect());
        if (src.format() != QImage::Format_RGB32 || widget->x11Info().depth() < 24) {
            Q_ASSERT(src.depth() >= 16);
            const QImage sub_src(src.scanLine(br.y()) + br.x() * (uint(src.depth()) / 8),
                                 br.width(), br.height(), src.bytesPerLine(), src.format());
            QX11PixmapData *data = new QX11PixmapData(QPixmapData::PixmapType);
            data->xinfo = widget->x11Info();
            data->fromImage(sub_src, Qt::NoOpaqueDetection);
            QPixmap pm = QPixmap(data);
            XCopyArea(X11->display, pm.handle(), widget->handle(), d_ptr->gc, 0 , 0 , br.width(), br.height(), wbr.x(), wbr.y());
        } else {
            // qpaintengine_x11.cpp
            extern void qt_x11_drawImage(const QRect &rect, const QPoint &pos, const QImage &image, Drawable hd, GC gc, Display *dpy, Visual *visual, int depth);
            qt_x11_drawImage(br, wbr.topLeft(), src, widget->handle(), d_ptr->gc, X11->display, (Visual *)widget->x11Info().visual(), widget->x11Info().depth());
        }
    }

    if (wrgn.rectCount() != 1)
        XSetClipMask(X11->display, d_ptr->gc, XNone);
#endif // FALCON

#ifdef Q_WS_MAC

#ifdef QT_MAC_USE_COCOA
    // Unified toolbar hack.
    QMainWindow* mWindow = qobject_cast<QMainWindow*>(widget->window());
    if (mWindow) {
        QMainWindowLayout *mLayout = qobject_cast<QMainWindowLayout*>(mWindow->layout());
        QList<QToolBar *> toolbarList = mLayout->qtoolbarsInUnifiedToolbarList;

        for (int i = 0; i < toolbarList.size(); ++i) {
            QToolBar* toolbar = toolbarList.at(i);
            if (mLayout->toolBarArea(toolbar) == Qt::TopToolBarArea) {
                QWidget* tbWidget = (QWidget*) toolbar;
                if (tbWidget->d_func()->unifiedSurface) {
                    tbWidget->d_func()->unifiedSurface->flush(tbWidget, rgn, offset);
                }
            }
        }
    }
#endif // QT_MAC_USE_COCOA

    Q_UNUSED(offset);
    // Get a context for the widget.
#ifndef QT_MAC_USE_COCOA
    CGContextRef context;
    CGrafPtr port = GetWindowPort(qt_mac_window_for(widget));
    QDBeginCGContext(port, &context);
#else
    extern CGContextRef qt_mac_graphicsContextFor(QWidget *);
    CGContextRef context = qt_mac_graphicsContextFor(widget);
#endif
    CGContextSaveGState(context);

    // Flip context.
    CGContextTranslateCTM(context, 0, widget->height());
    CGContextScaleCTM(context, 1, -1);

    // Clip to region.
    const QVector<QRect> &rects = rgn.rects();
    for (int i = 0; i < rects.size(); ++i) {
        const QRect &rect = rects.at(i);
        CGContextAddRect(context, CGRectMake(rect.x(), rect.y(), rect.width(), rect.height()));
    }
    CGContextClip(context);

    QRect r = rgn.boundingRect();
    const CGRect area = CGRectMake(r.x(), r.y(), r.width(), r.height());
    CGImageRef image = CGBitmapContextCreateImage(d->image->cg);
    CGImageRef subImage = CGImageCreateWithImageInRect(image, area);

    qt_mac_drawCGImage(context, &area, subImage);

    CGImageRelease(subImage);
    CGImageRelease(image);

#ifndef QT_MAC_USE_COCOA
    QDEndCGContext(port, &context);
#else
    CGContextFlush(context);
#endif

    // Restore context.
    CGContextRestoreGState(context);

#endif // Q_WS_MAC

#ifdef Q_OS_SYMBIAN
    Q_UNUSED(widget);
    Q_UNUSED(rgn);
    Q_UNUSED(offset);
#endif
}

void QRasterWindowSurface::setGeometry(const QRect &rect)
{
    QWindowSurface::setGeometry(rect);
    Q_D(QRasterWindowSurface);
    d->inSetGeometry = true;
    if (d->image == 0 || d->image->width() < rect.width() || d->image->height() < rect.height()) {
#if (defined(Q_WS_X11) && !defined(QT_NO_XRENDER)) || (defined(Q_WS_WIN) && !defined(Q_WS_WINCE))
#ifndef Q_WS_WIN
        if (d_ptr->translucentBackground)
#else
        if (!qt_widget_private(window())->isOpaque)
#endif
            prepareBuffer(QImage::Format_ARGB32_Premultiplied, window());
        else
#endif
            prepareBuffer(QNativeImage::systemFormat(), window());
    }
    d->inSetGeometry = false;

#if defined(Q_WS_MAC) && defined(QT_MAC_USE_COCOA)
    QMainWindow* mWindow = qobject_cast<QMainWindow*>(window());
    if (mWindow) {
        QMainWindowLayout *mLayout = qobject_cast<QMainWindowLayout*>(mWindow->layout());
        QList<QToolBar *> toolbarList = mLayout->qtoolbarsInUnifiedToolbarList;

        for (int i = 0; i < toolbarList.size(); ++i) {
            QToolBar* toolbar = toolbarList.at(i);
            if (mLayout->toolBarArea(toolbar) == Qt::TopToolBarArea) {
                QWidget* tbWidget = (QWidget*) toolbar;
                if (tbWidget->d_func()->unifiedSurface) {
                    tbWidget->d_func()->unifiedSurface->setGeometry(rect);
                }
            }
        }
    }
#endif // Q_WS_MAC && QT_MAC_USE_COCOA

}

// from qwindowsurface.cpp
extern void qt_scrollRectInImage(QImage &img, const QRect &rect, const QPoint &offset);

bool QRasterWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
#ifdef Q_WS_WIN
    Q_D(QRasterWindowSurface);

    if (!d->image || !d->image->hdc)
        return false;

    QRect rect = area.boundingRect();
    BitBlt(d->image->hdc, rect.x()+dx, rect.y()+dy, rect.width(), rect.height(),
           d->image->hdc, rect.x(), rect.y(), SRCCOPY);

    return true;
#else
    Q_D(QRasterWindowSurface);

    if (!d->image || d->image->image.isNull())
        return false;

    const QVector<QRect> rects = area.rects();
    for (int i = 0; i < rects.size(); ++i)
        qt_scrollRectInImage(d->image->image, rects.at(i), QPoint(dx, dy));

    return true;
#endif
}


void QRasterWindowSurface::prepareBuffer(QImage::Format format, QWidget *widget)
{
    Q_D(QRasterWindowSurface);

    int width = window()->width();
    int height = window()->height();
    if (d->image) {
        width = qMax(d->image->width(), width);
        height = qMax(d->image->height(), height);
    }

    if (width == 0 || height == 0) {
        delete d->image;
        d->image = 0;
        return;
    }

    QNativeImage *oldImage = d->image;

    d->image = new QNativeImage(width, height, format, false, widget);

    if (oldImage && d->inSetGeometry && hasStaticContents()) {
        // Make sure we use the const version of bits() (no detach).
        const uchar *src = const_cast<const QImage &>(oldImage->image).bits();
        uchar *dst = d->image->image.bits();

        const int srcBytesPerLine = oldImage->image.bytesPerLine();
        const int dstBytesPerLine = d->image->image.bytesPerLine();
        const int bytesPerPixel = oldImage->image.depth() >> 3;

        QRegion staticRegion(staticContents());
        // Make sure we're inside the boundaries of the old image.
        staticRegion &= QRect(0, 0, oldImage->image.width(), oldImage->image.height());
        const QVector<QRect> &rects = staticRegion.rects();
        const QRect *srcRect = rects.constData();

        // Copy the static content of the old image into the new one.
        int numRectsLeft = rects.size();
        do {
            const int bytesOffset = srcRect->x() * bytesPerPixel;
            const int dy = srcRect->y();

            // Adjust src and dst to point to the right offset.
            const uchar *s = src + dy * srcBytesPerLine + bytesOffset;
            uchar *d = dst + dy * dstBytesPerLine + bytesOffset;
            const int numBytes = srcRect->width() * bytesPerPixel;

            int numScanLinesLeft = srcRect->height();
            do {
                ::memcpy(d, s, numBytes);
                d += dstBytesPerLine;
                s += srcBytesPerLine;
            } while (--numScanLinesLeft);

            ++srcRect;
        } while (--numRectsLeft);
    }

    delete oldImage;
}

QT_END_NAMESPACE
