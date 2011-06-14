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
    createAndSetPlatformContext();
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


int nextBiggerPow2(int start, int max){
    int i = 2;
    while(i < start)
        i *=2;

    if(i > max) return max;
    else        return i;
}

void QAndroidEglFSScreen::createWindowSurface()
{
    if (m_windowSurface!=EGL_NO_SURFACE)
    { // unbind and destroy old surface
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_display, m_windowSurface);
        m_windowSurface = EGL_NO_SURFACE;
    }
    EGLint maxWidth = 2048; // just to have something if eglGetConfigAttrib doesn't work
    eglGetConfigAttrib(m_display, m_config, EGL_MAX_PBUFFER_WIDTH, &maxWidth);
    EGLint maxHeight = 2048;
    eglGetConfigAttrib(m_display, m_config, EGL_MAX_PBUFFER_HEIGHT, &maxHeight);
    EGLint temp = 0;
    EGLint surfaceAttribList[32];
    surfaceAttribList[temp++] = EGL_WIDTH;
    surfaceAttribList[temp++] = nextBiggerPow2(mGeometry.width(), maxWidth);

    surfaceAttribList[temp++] = EGL_HEIGHT;
    surfaceAttribList[temp++] = nextBiggerPow2(mGeometry.height(), maxHeight);

    surfaceAttribList[temp++] = EGL_TEXTURE_FORMAT;
    surfaceAttribList[temp++] = EGL_TEXTURE_RGBA;

    surfaceAttribList[temp++] = EGL_TEXTURE_TARGET;
    surfaceAttribList[temp++] = EGL_TEXTURE_2D;

    surfaceAttribList[temp++] = EGL_NONE;
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
//    m_windowSurface = eglCreatePbufferSurface(m_display, m_config, surfaceAttribList);
    qt_checkAndWarnAboutEGLError("QEglFSScreen::createAndSetPlatformContext()", "eglCreatePbufferSurface(m_display, m_config, surfaceAttribList).    (Note: a known error on a lot of devices is:  EGL_BAD_PARAMETER (0x300C / 12300))");
    if (m_windowSurface == EGL_NO_SURFACE) {
        qWarning("Could not create the egl surface: error = 0x%x\n", eglGetError());
        eglTerminate(m_display);
        qFatal("EGL error");
    }
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

//    const EGLint attribs[] = {
//            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
//            EGL_BLUE_SIZE, 5,
//            EGL_GREEN_SIZE, 6,
//            EGL_RED_SIZE, 5,
//            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//            EGL_NONE
//    };

//    EGLint numConfigs;
//    eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs);
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

    EGLint temp = 0;
//    EGLint surfaceAttribList[32];
//    surfaceAttribList[temp++] = EGL_WIDTH;
//    surfaceAttribList[temp++] = nextBiggerPow2(mGeometry.width(), maxWidth);

//    surfaceAttribList[temp++] = EGL_HEIGHT;
//    surfaceAttribList[temp++] = nextBiggerPow2(mGeometry.height(), maxHeight);

//    surfaceAttribList[temp++] = EGL_TEXTURE_FORMAT;
//    surfaceAttribList[temp++] = EGL_TEXTURE_RGBA;

//    surfaceAttribList[temp++] = EGL_TEXTURE_TARGET;
//    surfaceAttribList[temp++] = EGL_TEXTURE_2D;

//    surfaceAttribList[temp++] = EGL_NONE;


//    m_windowSurface = eglCreatePbufferSurface(m_display, m_config, surfaceAttribList);
//    qt_checkAndWarnAboutEGLError("QEglFSScreen::createAndSetPlatformContext()", "eglCreatePbufferSurface(m_display, m_config, surfaceAttribList).    (Note: a known error on a lot of devices is:  EGL_BAD_PARAMETER (0x300C / 12300))");
//    if (m_windowSurface == EGL_NO_SURFACE) {
//        qWarning("Could not create the egl surface: error = 0x%x\n", eglGetError());
//        eglTerminate(m_display);
//        qFatal("EGL error");
//    }
    EGLint attribList[32];
    temp = 0;

    attribList[temp++] = EGL_CONTEXT_CLIENT_VERSION;
    attribList[temp++] = 2; // GLES version 2
    attribList[temp++] = EGL_NONE;


    createWindowSurface();

    QAndroidEglFSPlatformContext *platformContext = new QAndroidEglFSPlatformContext(m_display,m_config,attribList,m_windowSurface,EGL_OPENGL_ES_API);
#warning FIXME
    platformContext->makeDefaultSharedContext(); // This is deprecated ... only working with reverting f7c8ac6e59906ab9fda9bbe1420e7b9a0ebb153d
    EGLint w,h;                    // screen size detection
    eglQuerySurface(m_display, m_windowSurface, EGL_WIDTH, &w);
    eglQuerySurface(m_display, m_windowSurface, EGL_HEIGHT, &h);
#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "eglQuerySurface(m_display, mPBSurface,...) returned:" << w << "for EGL_WIDTH and" << h << "for EGL_HEIGHT";
#endif
    if(w <= 0 || h <= 0)
        qFatal("PBufferSurface has invalid size!!!");
    mPbufferSize = QSize(w,h);
    mGeometry.setSize(mPbufferSize);

    platformContext->makeCurrent();              // Is this necessary?

    m_platformContext = platformContext;
}


QRect QAndroidEglFSScreen::geometry() const
{
    while(mGeometry.isNull()) {
        qWarning() << "WARNING: The android-gl-thread hasn't setup it's opengl-surface yet. Wait 200ms and check again...";
        usleep(100 * 1000);
    }

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
        QAndroidEglFSScreen *that = const_cast<QAndroidEglFSScreen *>(this);
        that->createAndSetPlatformContext();
    }
    return m_platformContext;
}

void QAndroidEglFSScreen::surfaceChanged()
{
    createWindowSurface();
    platformContext()->setSurface(m_windowSurface);
}


#include "gl_code.h"

static const char gVertexShader[] =
    "attribute vec4 a_PositionVector;           \n"
    "attribute vec2 a_TexCoordVector;           \n"
    "varying vec2 v_TexCoordVector;             \n"
    "void main()                                \n"
    "{                                          \n"
    "   gl_Position = a_PositionVector;         \n"
    "   v_TexCoordVector = a_TexCoordVector;    \n"
    "}                                          \n";

static const char gFragmentShader[] =
    "precision mediump float;                                           \n"
    "varying vec2 v_TexCoordVector;                                     \n"
    "uniform sampler2D s_TextureUnit;                                   \n"
    "void main()                                                        \n"
    "{                                                                  \n"
    "  gl_FragColor = texture2D( s_TextureUnit, v_TexCoordVector );     \n"
    "}                                                                  \n";

bool QAndroidEglFSScreen::setupGraphics(int w, int h)
{

    mGeometry.setRect(0,0,w,h);

    mGLSLProgram = createProgram(gVertexShader, gFragmentShader);
    if (!mGLSLProgram) {
        qWarning() << "Could not create program for Android-On-Screen-Blitting.";
        return false;
    }

    // ...just error-check the first gl-primitiv-function ... if this one is working the rest should as well...
    mPositionVertexAttrib = glGetAttribLocation(mGLSLProgram, "a_PositionVector");
    qt_checkAndWarnAboutGLError("QEglFSScreen::setupGraphics()", "glGetAttribLocation(mGLSLProgram, \"a_PositionVector\")");

    mTexCoordAttrib = glGetAttribLocation(mGLSLProgram, "a_TexCoordVector");

    mTextureUnitUniform = glGetUniformLocation(mGLSLProgram, "s_TextureUnit");

    glViewport(0, 0, w, h);

    glGenTextures ( 1, &mPBTextureID );
    glBindTexture ( GL_TEXTURE_2D, mPBTextureID );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    return true;
}

void QAndroidEglFSScreen::renderFrame()
{
    if (m_platformContext == 0) {           // no context -> no surface -> nothing to blit -> just clean colorbuffer
        glClearColor ( 0.35f, 0.66f, 0.21f, 1.0f ); // Qt-green .. i think :)
        glClear ( GL_COLOR_BUFFER_BIT );

        return;
    }

    GLfloat minX = 0.0f;
    GLfloat maxX =        ((GLfloat)mGeometry.width()  / (GLfloat)mPbufferSize.width() );  /*Q_ASSERT(maxX <= 1.0);  */// Q_ASSERT fails if pbuffer can't be as big as geomety ... this would be a strange gfx-driver!!
    GLfloat minY = 1.0f - ((GLfloat)mGeometry.height() / (GLfloat)mPbufferSize.height());  /*Q_ASSERT(minY <= 1.0);*/  // Q_ASSERT fails if pbuffer can't be as big as geomety ... this would be a strange gfx-driver!!
    GLfloat maxY = 1.0f;

    GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,  /*Position 0*/
                            -1.0f, -1.0f, 0.0f,  /*Position 1*/
                             1.0f, -1.0f, 0.0f,  /*Position 2*/
                             1.0f,  1.0f, 0.0f,  /*Position 3*/ };

    GLfloat vTexCoord[] = {  minX,  minY,        /*TexCoord 0*/
                             minX,  maxY,        /*TexCoord 1*/
                             maxX,  maxY,        /*TexCoord 2*/
                             maxX,  minY         /*TexCoord 3*/ };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    glUseProgram(mGLSLProgram);
    qt_checkAndWarnAboutGLError("QEglFSScreen::renderFrame()", "glUseProgram( mGLSLProgram )");

    glVertexAttribPointer(mPositionVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(mPositionVertexAttrib );

    glVertexAttribPointer(mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, vTexCoord);
    glEnableVertexAttribArray(mTexCoordAttrib);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mPBTextureID);
    glUniform1i(mTextureUnitUniform, 0);


    eglBindTexImage(m_display, m_windowSurface, EGL_BACK_BUFFER);
    qt_checkAndWarnAboutEGLError("QEglFSScreen::renderFrame()", "eglBindTexImage(m_display, mPBSurface, EGL_BACK_BUFFER)");

    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
    qt_checkAndWarnAboutGLError("QEglFSScreen::renderFrame()", "glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );");

    eglReleaseTexImage(m_display, m_windowSurface, EGL_BACK_BUFFER);
    qt_checkAndWarnAboutEGLError("QEglFSScreen::renderFrame()", "eglReleaseTexImage(m_display, mPBSurface, EGL_BACK_BUFFER);");

}

QT_END_NAMESPACE
