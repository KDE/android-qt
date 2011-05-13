/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qwaylanddisplay.h"

#include "qwaylandwindow.h"
#include "qwaylandscreen.h"
#include "qwaylandcursor.h"
#include "qwaylandinputdevice.h"
#include "qwaylandclipboard.h"

#ifdef QT_WAYLAND_GL_SUPPORT
#include "gl_integration/qwaylandglintegration.h"
#endif

#include <QtCore/QAbstractEventDispatcher>
#include <QtGui/QApplication>
#include <QtGui/private/qapplication_p.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

struct wl_surface *QWaylandDisplay::createSurface(void *handle)
{
    struct wl_surface * surface = wl_compositor_create_surface(mCompositor);
    wl_surface_set_user_data(surface, handle);
    return surface;
}

struct wl_buffer *QWaylandDisplay::createShmBuffer(int fd,
                                                   int width, int height,
                                                   uint32_t stride,
                                                   struct wl_visual *visual)
{
    return wl_shm_create_buffer(mShm, fd, width, height, stride, visual);
}

struct wl_visual *QWaylandDisplay::rgbVisual()
{
    return wl_display_get_rgb_visual(mDisplay);
}

struct wl_visual *QWaylandDisplay::argbVisual()
{
    return wl_display_get_argb_visual(mDisplay);
}

struct wl_visual *QWaylandDisplay::argbPremultipliedVisual()
{
    return wl_display_get_premultiplied_argb_visual(mDisplay);
}

#ifdef QT_WAYLAND_GL_SUPPORT
QWaylandGLIntegration * QWaylandDisplay::eglIntegration()
{
    return mEglIntegration;
}
#endif

void QWaylandDisplay::shellHandleConfigure(void *data, struct wl_shell *shell,
                                           uint32_t time, uint32_t edges,
                                           struct wl_surface *surface,
                                           int32_t width, int32_t height)
{
    Q_UNUSED(data);
    Q_UNUSED(shell);
    Q_UNUSED(time);
    Q_UNUSED(edges);
    QWaylandWindow *ww = (QWaylandWindow *) wl_surface_get_user_data(surface);

    ww->configure(time, edges, 0, 0, width, height);
}

const struct wl_shell_listener QWaylandDisplay::shellListener = {
    QWaylandDisplay::shellHandleConfigure,
};

QWaylandDisplay::QWaylandDisplay(void)
{
    mDisplay = wl_display_connect(NULL);
    if (mDisplay == NULL) {
        qErrnoWarning(errno, "Failed to create display");
        qFatal("No wayland connection available.");
    }

    wl_display_add_global_listener(mDisplay, QWaylandDisplay::displayHandleGlobal, this);

#ifdef QT_WAYLAND_GL_SUPPORT
    mEglIntegration = QWaylandGLIntegration::createGLIntegration(this);
#endif
    blockingReadEvents();

    qRegisterMetaType<uint32_t>("uint32_t");

#ifdef QT_WAYLAND_GL_SUPPORT
    mEglIntegration->initialize();
#endif

    connect(QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(flushRequests()));

    mFd = wl_display_get_fd(mDisplay, sourceUpdate, this);

    mReadNotifier = new QSocketNotifier(mFd, QSocketNotifier::Read, this);
    connect(mReadNotifier, SIGNAL(activated(int)), this, SLOT(readEvents()));

    waitForScreens();
}

QWaylandDisplay::~QWaylandDisplay(void)
{
    close(mFd);
#ifdef QT_WAYLAND_GL_SUPPORT
    delete mEglIntegration;
#endif
    wl_display_destroy(mDisplay);
}

void QWaylandDisplay::createNewScreen(struct wl_output *output, QRect geometry)
{
    QWaylandScreen *waylandScreen = new QWaylandScreen(this,output,geometry);
    mScreens.append(waylandScreen);
}

void QWaylandDisplay::syncCallback(wl_display_sync_func_t func, void *data)
{
    wl_display_sync_callback(mDisplay, func, data);
}

void QWaylandDisplay::frameCallback(wl_display_frame_func_t func, struct wl_surface *surface, void *data)
{
    wl_display_frame_callback(mDisplay, surface, func, data);
}

void QWaylandDisplay::flushRequests()
{
    if (mSocketMask & WL_DISPLAY_WRITABLE)
        wl_display_iterate(mDisplay, WL_DISPLAY_WRITABLE);
}

void QWaylandDisplay::readEvents()
{
// verify that there is still data on the socket
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(mFd, &fds);
    fd_set nds;
    FD_ZERO(&nds);
    fd_set rs = fds;
    fd_set ws = nds;
    fd_set es = nds;
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int ret = ::select(mFd+1, &rs, &ws, &es, &timeout );

    if (ret <= 0) {
        //qDebug("QWaylandDisplay::readEvents() No data... blocking avoided");
        return;
    }

    wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);
}

void QWaylandDisplay::blockingReadEvents()
{
    wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);
}

int QWaylandDisplay::sourceUpdate(uint32_t mask, void *data)
{
    QWaylandDisplay *waylandDisplay = static_cast<QWaylandDisplay *>(data);
    waylandDisplay->mSocketMask = mask;

    return 0;
}

void QWaylandDisplay::outputHandleGeometry(void *data,
                                           struct wl_output *output,
                                           int32_t x, int32_t y,
                                           int32_t width, int32_t height)
{
    QWaylandDisplay *waylandDisplay = static_cast<QWaylandDisplay *>(data);
    QRect outputRect = QRect(x, y, width, height);
    waylandDisplay->createNewScreen(output,outputRect);
}

const struct wl_output_listener QWaylandDisplay::outputListener = {
    QWaylandDisplay::outputHandleGeometry
};

void QWaylandDisplay::waitForScreens()
{
    flushRequests();
    while (mScreens.isEmpty())
        blockingReadEvents();
}

void QWaylandDisplay::displayHandleGlobal(struct wl_display *display,
                                          uint32_t id,
                                          const char *interface,
                                          uint32_t version,
                                          void *data)
{
    Q_UNUSED(display);
    QWaylandDisplay *that = static_cast<QWaylandDisplay *>(data);
    that->displayHandleGlobal(id, QByteArray(interface), version);
}

void QWaylandDisplay::displayHandleGlobal(uint32_t id,
                                          const QByteArray &interface,
                                          uint32_t version)
{
    Q_UNUSED(version);
    if (interface == "wl_output") {
        struct wl_output *output = wl_output_create(mDisplay, id, 1);
        wl_output_add_listener(output, &outputListener, this);
    } else if (interface == "wl_compositor") {
        mCompositor = wl_compositor_create(mDisplay, id, 1);
    } else if (interface == "wl_shm") {
        mShm = wl_shm_create(mDisplay, id, 1);
    } else if (interface == "wl_shell"){
        mShell = wl_shell_create(mDisplay, id, 1);
        wl_shell_add_listener(mShell, &shellListener, this);
    } else if (interface == "wl_input_device") {
        QWaylandInputDevice *inputDevice =
            new QWaylandInputDevice(mDisplay, id);
        mInputDevices.append(inputDevice);
    } else if (interface == "wl_selection_offer") {
        QPlatformIntegration *plat = QApplicationPrivate::platformIntegration();
        QWaylandClipboard *clipboard = static_cast<QWaylandClipboard *>(plat->clipboard());
        clipboard->createSelectionOffer(id);
    }
}
