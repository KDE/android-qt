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

#include "qeglfsscreen.h"

#include "../eglconvenience/qeglconvenience.h"
#include "../eglconvenience/qeglplatformcontext.h"

#ifdef Q_OPENKODE
#include <KD/kd.h>
#include <KD/NV_initialize.h>
#endif //Q_OPENKODE

QT_BEGIN_NAMESPACE

// #define QEGL_EXTRA_DEBUG

#ifdef QEGL_EXTRA_DEBUG
struct AttrInfo { EGLint attr; const char *name; };
static struct AttrInfo attrs[] = {
    {EGL_BUFFER_SIZE, "EGL_BUFFER_SIZE"},
    {EGL_ALPHA_SIZE, "EGL_ALPHA_SIZE"},
    {EGL_BLUE_SIZE, "EGL_BLUE_SIZE"},
    {EGL_GREEN_SIZE, "EGL_GREEN_SIZE"},
    {EGL_RED_SIZE, "EGL_RED_SIZE"},
    {EGL_DEPTH_SIZE, "EGL_DEPTH_SIZE"},
    {EGL_STENCIL_SIZE, "EGL_STENCIL_SIZE"},
    {EGL_CONFIG_CAVEAT, "EGL_CONFIG_CAVEAT"},
    {EGL_CONFIG_ID, "EGL_CONFIG_ID"},
    {EGL_LEVEL, "EGL_LEVEL"},
    {EGL_MAX_PBUFFER_HEIGHT, "EGL_MAX_PBUFFER_HEIGHT"},
    {EGL_MAX_PBUFFER_PIXELS, "EGL_MAX_PBUFFER_PIXELS"},
    {EGL_MAX_PBUFFER_WIDTH, "EGL_MAX_PBUFFER_WIDTH"},
    {EGL_NATIVE_RENDERABLE, "EGL_NATIVE_RENDERABLE"},
    {EGL_NATIVE_VISUAL_ID, "EGL_NATIVE_VISUAL_ID"},
    {EGL_NATIVE_VISUAL_TYPE, "EGL_NATIVE_VISUAL_TYPE"},
    {EGL_SAMPLES, "EGL_SAMPLES"},
    {EGL_SAMPLE_BUFFERS, "EGL_SAMPLE_BUFFERS"},
    {EGL_SURFACE_TYPE, "EGL_SURFACE_TYPE"},
    {EGL_TRANSPARENT_TYPE, "EGL_TRANSPARENT_TYPE"},
    {EGL_TRANSPARENT_BLUE_VALUE, "EGL_TRANSPARENT_BLUE_VALUE"},
    {EGL_TRANSPARENT_GREEN_VALUE, "EGL_TRANSPARENT_GREEN_VALUE"},
    {EGL_TRANSPARENT_RED_VALUE, "EGL_TRANSPARENT_RED_VALUE"},
    {EGL_BIND_TO_TEXTURE_RGB, "EGL_BIND_TO_TEXTURE_RGB"},
    {EGL_BIND_TO_TEXTURE_RGBA, "EGL_BIND_TO_TEXTURE_RGBA"},
    {EGL_MIN_SWAP_INTERVAL, "EGL_MIN_SWAP_INTERVAL"},
    {EGL_MAX_SWAP_INTERVAL, "EGL_MAX_SWAP_INTERVAL"},
    {-1, 0}};
#endif //QEGL_EXTRA_DEBUG

QEglFSScreen::QEglFSScreen(EGLNativeDisplayType display)
    : m_depth(32), m_format(QImage::Format_ARGB32_Premultiplied), m_platformContext(0)
{
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QEglScreen %p\n", this);
#endif

    EGLint major, minor;
#ifdef QEGL_EXTRA_DEBUG
    EGLint index;
#endif
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        qWarning("Could not bind GL_ES API\n");
        qFatal("EGL error");
    }

    m_dpy = eglGetDisplay(display);
    if (m_dpy == EGL_NO_DISPLAY) {
        qWarning("Could not open egl display\n");
        qFatal("EGL error");
    }
    qWarning("Opened display %p\n", m_dpy);

    if (!eglInitialize(m_dpy, &major, &minor)) {
        qWarning("Could not initialize egl display\n");
        qFatal("EGL error");
    }

    qWarning("Initialized display %d %d\n", major, minor);

    QPlatformWindowFormat platformFormat;
    platformFormat.setWindowApi(QPlatformWindowFormat::OpenGL);

    QByteArray depthString = qgetenv("QT_QPA_EGLFS_DEPTH");
    if (depthString.toInt() == 16) {
        platformFormat.setDepth(16);
        platformFormat.setRedBufferSize(5);
        platformFormat.setGreenBufferSize(6);
        platformFormat.setBlueBufferSize(5);
        m_depth = 16;
        m_format = QImage::Format_RGB16;
    } else {
        platformFormat.setDepth(32);
        platformFormat.setRedBufferSize(8);
        platformFormat.setGreenBufferSize(8);
        platformFormat.setBlueBufferSize(8);
    }

    if (!qgetenv("QT_QPA_EGLFS_MULTISAMPLE").isEmpty()) {
        platformFormat.setSampleBuffers(true);
    }

    int swapInterval = 1;
    QByteArray swapIntervalString = qgetenv("QT_QPA_EGLFS_SWAPINTERVAL");
    if (!swapIntervalString.isEmpty()) {
        bool ok;
        swapInterval = swapIntervalString.toInt(&ok);
        if (!ok)
            swapInterval = 1;
    }

    EGLConfig config = q_configFromQPlatformWindowFormat(m_dpy, platformFormat);
    eglSwapInterval(display, swapInterval);

    EGLNativeWindowType eglWindow = 0;
#ifdef Q_OPENKODE
    if (kdInitializeNV() == KD_ENOTINITIALIZED) {
        qFatal("Did not manage to initialize openkode");
    }
    KDWindow *window = kdCreateWindow(m_dpy,config,0);

    kdRealizeWindow(window,&eglWindow);
#endif

    m_surface = eglCreateWindowSurface(m_dpy, config, eglWindow, NULL);
    if (m_surface == EGL_NO_SURFACE) {
        qWarning("Could not create the egl surface: error = 0x%x\n", eglGetError());
        eglTerminate(m_dpy);
        qFatal("EGL error");
    }
    //    qWarning("Created surface %dx%d\n", w, h);

#ifdef QEGL_EXTRA_DEBUG
    qWarning("Configuration %d matches requirements\n", (int)config);

    for (index = 0; attrs[index].attr != -1; ++index) {
        EGLint value;
        if (eglGetConfigAttrib(m_dpy, config, attrs[index].attr, &value)) {
            qWarning("\t%s: %d\n", attrs[index].name, (int)value);
        }
    }
    qWarning("\n");
#endif

    EGLint temp;
    EGLint attribList[32];

    temp = 0;

    attribList[temp++] = EGL_CONTEXT_CLIENT_VERSION;
    attribList[temp++] = 2; // GLES version 2
    attribList[temp++] = EGL_NONE;

    m_platformContext = new QEGLPlatformContext(m_dpy,config,attribList,m_surface,EGL_OPENGL_ES_API);

//    qWarning("Created platformcontext");
    EGLint w,h;

    eglQuerySurface(m_dpy, m_surface, EGL_WIDTH, &w);
    eglQuerySurface(m_dpy, m_surface, EGL_HEIGHT, &h);

    m_geometry = QRect(0,0,w,h);
}

QT_END_NAMESPACE
