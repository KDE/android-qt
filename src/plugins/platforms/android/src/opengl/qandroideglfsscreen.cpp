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
#include "qandroidplatformintegration.h"

#include "qeglconvenience.h"
#include "qandroideglplatformcontext.h"

#include "androidjnimain.h"
#include "GLES2/gl2.h"

#include <android/api-level.h>

#if __ANDROID_API__>8
# include <android/native_window.h>
#endif

#include <QWindowSystemInterface>
#include <QApplication>
#include <QTimer>
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
    mPhysicalSize = QSize(QAndroidPlatformIntegration::m_defaultPhysicalSizeWidth,
                            QAndroidPlatformIntegration::m_defaultPhysicalSizeHeight);

    mGeometry = QRect(0, 0, QAndroidPlatformIntegration::m_defaultGeometryWidth,
                            QAndroidPlatformIntegration::m_defaultGeometryHeight);

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
    qt_checkAndWarnAboutEGLError(__PRETTY_FUNCTION__, "eglGetConfigAttrib(m_display, m_config, EGL_NATIVE_VISUAL_ID, &format);");
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
#endif
    qDebug()<<"QAndroidEglFSScreen::createWindowSurface"<<window;
    m_windowSurface = eglCreateWindowSurface(m_display, m_config, window, 0); 
    qt_checkAndWarnAboutEGLError(__PRETTY_FUNCTION__, "m_windowSurface = eglCreateWindowSurface(m_display, m_config, window, 0);");
    if (m_windowSurface == EGL_NO_SURFACE) {
        qWarning("Could not create the egl surface: error = 0x%x\n", eglGetError());
        eglTerminate(m_display);
        qFatal("EGL error");
    }

    EGLint w,h;                    // screen size detection
    eglQuerySurface(m_display, m_windowSurface, EGL_WIDTH, &w);
    qt_checkAndWarnAboutEGLError(__PRETTY_FUNCTION__, "eglQuerySurface(m_display, m_windowSurface, EGL_WIDTH, &w);");
    eglQuerySurface(m_display, m_windowSurface, EGL_HEIGHT, &h);
    qt_checkAndWarnAboutEGLError(__PRETTY_FUNCTION__, "eglQuerySurface(m_display, m_windowSurface, EGL_HEIGHT, &h);");

#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "eglQuerySurface(m_display, m_windowSurface,...) returned:" << w << "for EGL_WIDTH and" << h << "for EGL_HEIGHT";
#endif

    if(w <= 0 || h <= 0)
        qFatal("EGL-WindowSurface has invalid size!");
    QWindowSystemInterface::handleScreenAvailableGeometryChange(0);
    QWindowSystemInterface::handleScreenGeometryChange(0);
    QTimer::singleShot(50, this, SLOT(updateTLWindows())); // repaint the whole scene
}

void QAndroidEglFSScreen::createAndSetPlatformContext() const {
    const_cast<QAndroidEglFSScreen *>(this)->createAndSetPlatformContext();
}

void QAndroidEglFSScreen::createAndSetPlatformContext()
{
    QPlatformWindowFormat platformFormat = QPlatformWindowFormat::defaultFormat();

    platformFormat.setWindowApi(QPlatformWindowFormat::OpenGL);

    if (qgetenv("QT_QPA_EGLFS_DEPTH").toInt()==16)
    {
        platformFormat.setDepth(16);
        platformFormat.setRedBufferSize(6);
        platformFormat.setGreenBufferSize(5);
        platformFormat.setBlueBufferSize(6);
        m_depth = 16;
        m_format = QImage::Format_RGB16;
    }
    else
    {
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
    m_platformContext->makeCurrent();              // Is this necessary?
}

void QAndroidEglFSScreen::setGeometry(QRect rect)
{
    qDebug()<<"QAndroidEglFSScreen::setGeometry"<<rect;
    mGeometry = rect;
}

void QAndroidEglFSScreen::setPhysicalSize(QSize size)
{
    qDebug()<<"QAndroidEglFSScreen::setPhysicalSize"<<size;
    mPhysicalSize = size;
}

void QAndroidEglFSScreen::setDirty(const QRect &rect)
{
    QPlatformScreen::setDirty(rect);
}

void QAndroidEglFSScreen::updateTLWindows()
{
    foreach(QWidget * w, qApp->topLevelWidgets())
        w->update();
}

QRect QAndroidEglFSScreen::geometry() const
{
    return mGeometry;
}

QSize QAndroidEglFSScreen::physicalSize() const
{
    return mPhysicalSize;
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
    if (!m_platformContext)
        createAndSetPlatformContext();
    return m_platformContext;
}

void QAndroidEglFSScreen::addWindow(QPlatformWindow *window)
{
    windowStack.prepend(window);
    QApplication::setActiveWindow(topWindow());
}

void QAndroidEglFSScreen::removeWindow(QPlatformWindow * window)
{
    windowStack.removeOne(window);
    QApplication::setActiveWindow(topWindow());
}

void QAndroidEglFSScreen::raise(QPlatformWindow * window)
{
    int index = windowStack.indexOf(window);
    if (index <= 0)
        return;
    windowStack.move(index, 0);
    QApplication::setActiveWindow(topWindow());
}

void QAndroidEglFSScreen::lower(QPlatformWindow * window)
{
    int index = windowStack.indexOf(window);
    if (index == -1 || index == (windowStack.size() - 1))
        return;
    windowStack.move(index, windowStack.size() - 1);
    QApplication::setActiveWindow(topWindow());
}

QWidget * QAndroidEglFSScreen::topWindow()
{
    foreach (QPlatformWindow * platformWindow, windowStack )
        if ( platformWindow->widget()->windowType()==Qt::Window || platformWindow->widget()->windowType()==Qt::Dialog )
            return platformWindow->widget();
    return 0;
}

void QAndroidEglFSScreen::surfaceChanged()
{
    if (!m_platformContext)
        createAndSetPlatformContext();
    else
        createWindowSurface();

    platformContext()->setSurface(m_windowSurface);
}

QT_END_NAMESPACE
