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

#include "qandroideglfsscreen.h"

#include "qeglconvenience.h"
#include "qandroideglplatformcontext.h"

#include "androidjnimain.h"
#include "GLES2/gl2.h"

#include <android/api-level.h>

#if __ANDROID_API__>8
# include <android/native_window.h>
#endif

#include <QWindowSystemInterface>
#include <QThread>
#include <QDebug>

QT_BEGIN_NAMESPACE

//#define QEGL_EXTRA_DEBUG

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

QAndroidEglFSScreen::QAndroidEglFSScreen(EGLNativeDisplayType display)
    : m_depth(32)
    , m_format(QImage::Format_Invalid)
    , m_platformContext(0)
    , m_windowSurface(EGL_NO_SURFACE)
{
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QEglScreen %p\n", this);
#endif

    EGLint major, minor;
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        qWarning("Could not bind GL_ES API\n");
        qFatal("EGL error");
    }

    m_display = eglGetDisplay(display);
    if (m_display == EGL_NO_DISPLAY) {
        qWarning("Could not open egl display\n");
        qFatal("EGL error");
    }
    qWarning("Opened display %p\n", m_display);

    if (!eglInitialize(m_display, &major, &minor)) {
        qWarning("Could not initialize egl display\n");
        qFatal("EGL error");
    }

    qWarning("Initialized display %d %d\n", major, minor);

    int swapInterval = 1;
    QByteArray swapIntervalString = qgetenv("QT_QPA_EGLFS_SWAPINTERVAL");
    if (!swapIntervalString.isEmpty()) {
        bool ok;
        swapInterval = swapIntervalString.toInt(&ok);
        if (!ok)
            swapInterval = 1;
    }
    eglSwapInterval(m_display, swapInterval);
}

#define NAME_NOT_FOUND 1
#define NO_ERROR 0

static int32_t getConfigFormatInfo(EGLint configID,
        int32_t& /*pixelFormat*/, int32_t& /*depthFormat*/)
{
    switch(configID) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    default:
        return NAME_NOT_FOUND;
    }
    return NO_ERROR;
}


static void checkBadMatch(EGLDisplay dpy, EGLConfig config,
                     NativeWindowType window, const EGLint */*attrib_list*/)
{
    qDebug()<<"checkBadMatch";
    if (window == 0)
    {
        qDebug()<<"(window == 0)";
        return;
    }
    EGLint surfaceType;
    if (eglGetConfigAttrib(dpy, config, EGL_SURFACE_TYPE, &surfaceType) == EGL_FALSE)
        return;

    if (!(surfaceType & EGL_WINDOW_BIT))
    {
        qDebug()<<"(!(surfaceType & EGL_WINDOW_BIT))";
        return;
    }

    EGLint configID;
    if (eglGetConfigAttrib(dpy, config, EGL_CONFIG_ID, &configID) == EGL_FALSE)
    {
        qDebug()<<"eglGetConfigAttrib(dpy, config, EGL_CONFIG_ID, &configID) == EGL_FALSE";
        return;
    }
    int32_t depthFormat;
    int32_t pixelFormat;
    if (getConfigFormatInfo(configID, pixelFormat, depthFormat) != NO_ERROR) {
        qDebug()<<"getConfigFormatInfo(configID, pixelFormat, depthFormat) != NO_ERROR";
        return;
    }
}

void QAndroidEglFSScreen::createWindowSurface()
{
    if (m_windowSurface!=EGL_NO_SURFACE)
    { // unbind and destroy old surface
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_display, m_windowSurface);
        m_windowSurface = EGL_NO_SURFACE;
    }

    EGLNativeWindowType window=QtAndroid::getNativeWindow();
#if __ANDROID_API__>8
    qDebug()<<"ANativeWindow_settings width "<<ANativeWindow_getWidth(window)<<" height "<<ANativeWindow_getHeight(window)<<" format "<< ANativeWindow_getFormat(window);
    EGLint format;
    eglGetConfigAttrib(m_display, m_config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
#endif
    qDebug()<<"QAndroidEglFSScreen::createWindowSurface"<<window;
    checkBadMatch(m_display, m_config, window, 0);
    m_windowSurface = eglCreateWindowSurface(m_display, m_config, window, 0);

    if (m_windowSurface == EGL_NO_SURFACE) {
        qWarning("Could not create the egl surface: error = 0x%x\n", eglGetError());
        eglTerminate(m_display);
        qFatal("EGL error");
    }

    EGLint w,h;                    // screen size detection
    eglQuerySurface(m_display, m_windowSurface, EGL_WIDTH, &w);
    eglQuerySurface(m_display, m_windowSurface, EGL_HEIGHT, &h);

#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "eglQuerySurface(m_display, m_windowSurface,...) returned:" << w << "for EGL_WIDTH and" << h << "for EGL_HEIGHT";
#endif

    if(w <= 0 || h <= 0)
        qFatal("EGL-WindowSurface has invalid size!");

    mGeometry.setWidth(w);
    mGeometry.setHeight(h);
}

void QAndroidEglFSScreen::createAndSetPlatformContext() const {
    const_cast<QAndroidEglFSScreen *>(this)->createAndSetPlatformContext();
}

void QAndroidEglFSScreen::createAndSetPlatformContext()
{
    QPlatformWindowFormat platformFormat = QPlatformWindowFormat::defaultFormat();

    platformFormat.setWindowApi(QPlatformWindowFormat::OpenGL);

    QByteArray depthString = qgetenv("QT_QPA_EGLFS_DEPTH");
    if (true)//depthString.toInt() == 16)
    {
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
        m_depth = 32;
        m_format = QImage::Format_RGB32;
    }
    if (!qgetenv("QT_QPA_EGLFS_MULTISAMPLE").isEmpty()) {
        platformFormat.setSampleBuffers(true);
    }

    m_config = q_configFromQPlatformWindowFormat(m_display, platformFormat);

#ifdef QEGL_EXTRA_DEBUG
    qWarning("Configuration %d matches requirements\n", (int)m_config);

    for (int index = 0; attrs[index].attr != -1; ++index) {
        EGLint value;
        if (eglGetConfigAttrib(m_display, m_config, attrs[index].attr, &value)) {
            qWarning("\t%s: %d\n", attrs[index].name, (int)value);
        }
    }
    qWarning("\n");
#endif

    createWindowSurface();

    Q_ASSERT(m_platformContext == 0);
    m_platformContext = new QAndroidEglFSPlatformContext(m_display, m_config, m_windowSurface, EGL_OPENGL_ES_API);

#warning FIXME
    m_platformContext->makeDefaultSharedContext(); // This is deprecated ... only working with reverting f7c8ac6e59906ab9fda9bbe1420e7b9a0ebb153d
    m_platformContext->makeCurrent();              // Is this necessary?
}


QRect QAndroidEglFSScreen::geometry() const
{
    if(!mGeometry.isValid()) {
        createAndSetPlatformContext();
    }
    Q_ASSERT(mGeometry.isValid());

    return mGeometry;
}

int QAndroidEglFSScreen::depth() const
{
    return m_depth;
}

QImage::Format QAndroidEglFSScreen::format() const
{
    if (m_format == QImage::Format_Invalid) {
        createAndSetPlatformContext();
    }
    return m_format;
}

QAndroidEglFSPlatformContext *QAndroidEglFSScreen::platformContext() const
{
    if (!m_platformContext) {
        createAndSetPlatformContext();
    }
    return m_platformContext;
}

void QAndroidEglFSScreen::surfaceChanged()
{
    createWindowSurface();
    platformContext()->setSurface(m_windowSurface);


    QWindowSystemInterface::handleScreenAvailableGeometryChange(0);
    QWindowSystemInterface::handleScreenGeometryChange(0);
}

QT_END_NAMESPACE
