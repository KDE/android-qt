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

    THIS SOFTWARE IS PROVIDED BY BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL BogDan Vatra <bog_dan_ro@yahoo.com> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <android/log.h>
#include <pthread.h>
#include <qcoreapplication.h>
#include <qimage.h>
#include <qpoint.h>
#include <qplugin.h>
#include <qsemaphore.h>
#include <qmutex.h>
#include <qdebug.h>
#include <qglobal.h>
#include <jni.h>

#include "androidjnimain.h"
#include "qandroidplatformintegration.h"
#include <QWindowSystemInterface>
#include <QApplication>
#include <QTouchEvent>

#include <qabstracteventdispatcher.h>

#include <android/bitmap.h>
#include <android/asset_manager_jni.h>
#include "qandroidassetsfileenginehandler.h"
#include <android/api-level.h>

#if __ANDROID_API__ > 8
# include <android/native_window_jni.h>
#endif

static jmethodID m_redrawSurfaceMethodID=0;

Q_IMPORT_PLUGIN (QtAndroid)

static JavaVM *m_javaVM = NULL;
static jclass m_applicationClass  = NULL;
static AAssetManager* m_assetManager = NULL;


#ifndef ANDROID_PLUGIN_OPENGL
static jobject m_surface = NULL;

#else

#if __ANDROID_API__ < 9
# define ANDROID_VIEW_SURFACE_JNI_ID "mSurface"
#else
# define ANDROID_VIEW_SURFACE_JNI_ID "mNativeSurface"
#endif

static EGLNativeWindowType m_nativeWindow=0;
static QSemaphore m_waitForWindowSemaphore;
static bool m_waitForWindow=false;

static const char* const SurfaceClassPathName = "android/view/Surface";
static jfieldID m_surfaceFieldID=0;
#endif

static QList<QWindowSystemInterface::TouchPoint> m_touchPoints;

static QSemaphore m_quitAppSemaphore;
static QMutex m_surfaceMutex;
static QSemaphore m_pauseApplicationSemaphore;
static QMutex m_pauseApplicationMutex;

static QAndroidPlatformIntegration * m_androidGraphicsSystem=0;
static int m_desktopWidthPixels=0, m_desktopHeightPixels=0;
static const char * const QtNativeClassPathName = "org/kde/necessitas/industrius/QtNative";
static const char * const ContextWrapperClassPathName = "android/content/ContextWrapper";


static volatile bool m_pauseApplication;

// Software keyboard support
static jmethodID m_showSoftwareKeyboardMethodID=0;
static jmethodID m_hideSoftwareKeyboardMethodID=0;
// Software keyboard support

static jmethodID m_setFullScreenMethodID=0;

static AndroidAssetsFileEngineHandler * m_androidAssetsFileEngineHandler = 0;

static inline void checkPauseApplication()
{
    m_pauseApplicationMutex.lock();
    if (m_pauseApplication)
    {
        m_pauseApplicationMutex.unlock();
        m_pauseApplicationSemaphore.acquire(); // wait unitl surface is created

        m_pauseApplicationMutex.lock();
        m_pauseApplication=false;
        m_pauseApplicationMutex.unlock();
        QWindowSystemInterface::handleScreenAvailableGeometryChange(0);
        QWindowSystemInterface::handleScreenGeometryChange(0);
    }
    else
        m_pauseApplicationMutex.unlock();
}

namespace QtAndroid
{
#ifndef ANDROID_PLUGIN_OPENGL
    void flushImage(const QPoint & pos, const QImage & image, const QRect & destinationRect)
    {
        checkPauseApplication();
        QMutexLocker locker(&m_surfaceMutex);
        if (!m_surface)
            return;

        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        int bpp=2;
        AndroidBitmapInfo  info;
        int ret;

        if ((ret = AndroidBitmap_getInfo(env, m_surface, &info)) < 0)
        {
            qWarning()<<"AndroidBitmap_getInfo() failed ! error="<<ret;
            m_javaVM->DetachCurrentThread();
            return;
        }
        if (info.format != ANDROID_BITMAP_FORMAT_RGB_565)
        {
            qWarning()<<"Bitmap format is not RGB_565 !";
            m_javaVM->DetachCurrentThread();
            return;
        }
        void * pixels;
        unsigned char * screenBits;
        if ((ret = AndroidBitmap_lockPixels(env, m_surface, &pixels)) < 0)
        {
            qWarning()<<"AndroidBitmap_lockPixels() failed ! error="<<ret;
            m_javaVM->DetachCurrentThread();
            return;
        }
        screenBits=(unsigned char *)pixels;
        int sbpl=info.stride;
        int swidth=info.width;
        int sheight=info.height;

        unsigned sposx=pos.x()+destinationRect.x();
        unsigned sposy=pos.y()+destinationRect.y();

        screenBits+=sposy*sbpl;

        unsigned ibpl=image.bytesPerLine();
        unsigned iposx=destinationRect.x();
        unsigned iposy=destinationRect.y();

        unsigned char * imageBits=(unsigned char *)((const uchar*)image.bits());
        imageBits+=iposy*ibpl;

        unsigned width=swidth-sposx<(unsigned)destinationRect.width()?swidth-sposx:destinationRect.width();
        unsigned height=sheight-sposy<(unsigned)destinationRect.height()?sheight-sposy:destinationRect.height();

        for (unsigned y=0;y<height;y++)
            memcpy(screenBits+y*sbpl+sposx*bpp,
                    imageBits+y*ibpl+iposx*bpp,
                   width*bpp);
        AndroidBitmap_unlockPixels(env, m_surface);

        env->CallStaticVoidMethod(m_applicationClass, m_redrawSurfaceMethodID,
                            (jint)destinationRect.left(),
                            (jint)destinationRect.top(),
                            (jint)destinationRect.right()+1,
                            (jint)destinationRect.bottom()+1);
#warning FIXME dirty hack, figure out why it needs to add 1 to right and bottom !!!!
        m_javaVM->DetachCurrentThread();
    }

#else // for #ifndef ANDROID_PLUGIN_OPENGL
    EGLNativeWindowType getNativeWindow(bool waitForWindow)
    {
        m_surfaceMutex.lock();
        if (!m_nativeWindow && waitForWindow)
        {
            m_waitForWindow = true;
            m_surfaceMutex.unlock();
            m_waitForWindowSemaphore.acquire();
            m_waitForWindow = false;
            return m_nativeWindow;
        }
        m_surfaceMutex.unlock();
        return m_nativeWindow;
    }
#endif

    void setAndroidPlatformIntegration(QAndroidPlatformIntegration * androidGraphicsSystem)
    {
        m_surfaceMutex.lock();
        m_androidGraphicsSystem=androidGraphicsSystem;
        m_surfaceMutex.unlock();
    }

    void showSoftwareKeyboard()
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        qDebug()<<"showSoftwareKeyboard";
        env->CallStaticVoidMethod(m_applicationClass, m_showSoftwareKeyboardMethodID);
        m_javaVM->DetachCurrentThread();
    }

    void hideSoftwareKeyboard()
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        qDebug()<<"hideSoftwareKeyboard";
        env->CallStaticVoidMethod(m_applicationClass, m_hideSoftwareKeyboardMethodID);
        m_javaVM->DetachCurrentThread();
    }

    void setFullScreen(bool fullScreen)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        qDebug()<<"setFullScreen"<<fullScreen;
        env->CallStaticVoidMethod(m_applicationClass, m_setFullScreenMethodID, fullScreen);
        m_javaVM->DetachCurrentThread();
    }

    JavaVM * javaVM()
    {
        return m_javaVM;
    }

    AAssetManager * assetManager()
    {
        return m_assetManager;
    }

    jclass applicationClass()
    {
        return m_applicationClass;
    }

}

static jboolean startQtAndroidPlugin(JNIEnv* env, jobject /*object*//*, jobject applicationAssetManager*/)
{
#ifndef ANDROID_PLUGIN_OPENGL
    m_surface = 0;
#else
    m_nativeWindow=0;
    m_waitForWindow=false;
#endif

    m_androidGraphicsSystem=0;
    m_androidAssetsFileEngineHandler = new AndroidAssetsFileEngineHandler();

#ifdef ANDROID_PLUGIN_OPENGL
    return true;
#else
    return false;
#endif
}

static void pauseQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
    m_pauseApplicationMutex.lock();
    if (m_androidGraphicsSystem)
        m_androidGraphicsSystem->pauseApp();
    m_pauseApplication=true;
    m_pauseApplicationMutex.unlock();
    m_surfaceMutex.unlock();
}

static void resumeQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
    m_pauseApplicationMutex.lock();
    if (m_androidGraphicsSystem)
        m_androidGraphicsSystem->resumeApp();

    if (m_pauseApplication)
        m_pauseApplicationSemaphore.release();

    m_pauseApplicationMutex.unlock();
    m_surfaceMutex.unlock();
}

static void quitQtAndroidPlugin(JNIEnv* env, jclass /*clazz*/)
{
#ifndef ANDROID_PLUGIN_OPENGL
    if (m_surface)
    {
        env->DeleteGlobalRef(m_surface);
        m_surface=0;
    }
#endif
    m_androidGraphicsSystem=0;

    delete m_androidAssetsFileEngineHandler;
}

static void terminateQt(JNIEnv* env, jclass /*clazz*/)
{
    env->DeleteGlobalRef(m_applicationClass);
}

#ifdef ANDROID_PLUGIN_OPENGL
#if __ANDROID_API__ < 9
struct FakeNativeWindow
{
    long long dummyNativeWindow;// force 64 bits alignment
};

class FakeSurface: public FakeNativeWindow
{
public:
    virtual void FakeSurfaceMethod()
    {
        fakeSurface = 0;
    }
    int fakeSurface;
};

EGLNativeWindowType ANativeWindow_fromSurface(JNIEnv *env, jobject jSurface)
{
    FakeSurface * surface=(FakeSurface *)env->GetIntField(jSurface, m_surfaceFieldID);
    return (EGLNativeWindowType)static_cast<FakeNativeWindow*>(surface);
}
#endif // __ANDROID_API__ < 9
#endif // ANDROID_PLUGIN_OPENGL

static void setSurface(JNIEnv *env, jobject /*thiz*/, jobject jSurface)
{
#ifndef ANDROID_PLUGIN_OPENGL
    if (m_surface)
        env->DeleteGlobalRef(m_surface);
    m_surface = env->NewGlobalRef(jSurface);
#else
    m_surfaceMutex.lock();
    m_nativeWindow = ANativeWindow_fromSurface(env, jSurface);
    qDebug()<<"setSurface"<<ANativeWindow_fromSurface(env, jSurface)<<(EGLNativeWindowType)env->GetIntField(jSurface, m_surfaceFieldID);
    if (m_waitForWindow)
        m_waitForWindowSemaphore.release();
    if (m_androidGraphicsSystem)
    {
        m_surfaceMutex.unlock();
        m_androidGraphicsSystem->surfaceChanged();
    }
    else
        m_surfaceMutex.unlock();
#endif  // for #ifndef ANDROID_PLUGIN_OPENGL
}

static void destroySurface(JNIEnv *env, jobject /*thiz*/)
{
#ifndef ANDROID_PLUGIN_OPENGL
    env->DeleteGlobalRef(m_surface);
    m_surface = 0;
#else
    m_nativeWindow = 0;
#endif
}

static void setDisplayMetrics(JNIEnv* /*env*/, jclass /*clazz*/,
                              jint widthPixels, jint heightPixels,
                              jint desktopWidthPixels, jint desktopHeightPixels,
                              jdouble xdpi, jdouble ydpi)
{
    m_desktopWidthPixels=desktopWidthPixels;
    m_desktopHeightPixels=desktopHeightPixels;

    if (!m_androidGraphicsSystem)
        QAndroidPlatformIntegration::setDefaultDisplayMetrics(desktopWidthPixels,desktopHeightPixels,
                                                                qRound((double)desktopWidthPixels  / xdpi * 25.4 ),
                                                                qRound((double)desktopHeightPixels / ydpi * 25.4 ));
    else
    {
        m_androidGraphicsSystem->setDisplayMetrics(qRound((double)desktopWidthPixels  / xdpi * 25.4 ),
                                                   qRound((double)desktopHeightPixels / ydpi * 25.4 ));
        m_androidGraphicsSystem->setDesktopSize(desktopWidthPixels, desktopHeightPixels);
    }
}

static void mouseDown(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/, jint x, jint y)
{
    QPoint globalPos(x,y);
    QWidget *tlw = m_androidGraphicsSystem?m_androidGraphicsSystem->getPrimaryScreen()->topLevelAt(globalPos):0;
    QPoint localPos=tlw?globalPos-tlw->pos():globalPos;
    QWindowSystemInterface::handleMouseEvent(tlw, localPos, globalPos
                                             , Qt::MouseButtons(Qt::LeftButton));
}

static void mouseUp(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/, jint x, jint y)
{
    QPoint globalPos(x,y);
    QWidget *tlw = m_androidGraphicsSystem?m_androidGraphicsSystem->getPrimaryScreen()->topLevelAt(globalPos):0;
    QPoint localPos=tlw?globalPos-tlw->pos():globalPos;
    QWindowSystemInterface::handleMouseEvent(tlw, localPos, globalPos
                                             , Qt::MouseButtons(Qt::NoButton));
}

static void mouseMove(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/, jint x, jint y)
{
    QPoint globalPos(x,y);
    QWidget *tlw = m_androidGraphicsSystem?m_androidGraphicsSystem->getPrimaryScreen()->topLevelAt(globalPos):0;
    QPoint localPos=tlw?globalPos-tlw->pos():globalPos;
    QWindowSystemInterface::handleMouseEvent(tlw, localPos, globalPos
                                             , Qt::MouseButtons(Qt::LeftButton));
}

static void touchBegin(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/)
{
    m_touchPoints.clear();
}

static void touchAdd(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/, jint id, jint action, jboolean primary, jint x, jint y, jfloat size, jfloat pressure)
{
    Qt::TouchPointState state=Qt::TouchPointStationary;
    switch(action)
    {
    case 0:
        state=Qt::TouchPointPressed;
        break;
    case 1:
        state=Qt::TouchPointMoved;
        break;
    case 2:
        state=Qt::TouchPointStationary;
        break;
    case 3:
        state=Qt::TouchPointReleased;
        break;
    }
    QWindowSystemInterface::TouchPoint touchPoint;
    touchPoint.id=id;
    touchPoint.pressure=pressure;
    touchPoint.normalPosition=QPointF((double)x/m_desktopWidthPixels, (double)y/m_desktopHeightPixels);
    touchPoint.isPrimary=primary;
    touchPoint.state=state;
    touchPoint.area=QRectF(x-((double)m_desktopWidthPixels*size)/2,
                           y-((double)m_desktopHeightPixels*size)/2,
                           (double)m_desktopWidthPixels*size,
                           (double)m_desktopHeightPixels*size);
    m_touchPoints.push_back(touchPoint);
}

static void touchEnd(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/, jint action)
{
    QEvent::Type eventType=QEvent::None;
    switch (action)
    {
        case 0:
            eventType=QEvent::TouchBegin;
            break;
        case 1:
            eventType=QEvent::TouchUpdate;
            break;
        case 2:
            eventType=QEvent::TouchEnd;
            break;
    }

    QWindowSystemInterface::handleTouchEvent(0, eventType, QTouchEvent::TouchScreen, m_touchPoints);
}

static int mapAndroidKey(int key)
{
    //0--9        0x00000007 -- 0x00000010
    if (key>=0x00000007 && key<=0x00000010)
        return Qt::Key_0+key-0x00000007;
    //A--Z        0x0000001d -- 0x00000036
    if (key>=0x0000001d && key<=0x00000036)
        return Qt::Key_A+key-0x0000001d;

    switch(key)
    {
        case 0x00000039:
        case 0x0000003a:
            return Qt::Key_Alt;

        case 0x0000004b:
            return Qt::Key_Apostrophe;

        case 0x00000004: //KEYCODE_BACK
            qDebug()<<"KEYCODE_BACK !!!!";
            return Qt::Key_Close;

        case 0x00000049:
            return Qt::Key_Backslash;

        case 0x00000005:
            return Qt::Key_Call;

        case 0x0000001b:
            return Qt::Key_WebCam;

        case 0x0000001c:
            return Qt::Key_Clear;

        case 0x00000037:
            return Qt::Key_Comma;

        case 0x00000043:
            return Qt::Key_Backspace;

        case 0x00000017: // KEYCODE_DPAD_CENTER
            return Qt::Key_Enter;

        case 0x00000014: // KEYCODE_DPAD_DOWN
            return Qt::Key_Down;

        case 0x00000015: //KEYCODE_DPAD_LEFT
            return Qt::Key_Left;

        case 0x00000016: //KEYCODE_DPAD_RIGHT
            return Qt::Key_Right;

        case 0x00000013: //KEYCODE_DPAD_UP
            return Qt::Key_Up;

        case 0x00000006: //KEYCODE_ENDCALL
            return Qt::Key_Hangup;

        case 0x00000042:
            return Qt::Key_Return;

        case 0x00000041: //KEYCODE_ENVELOPE
            return Qt::Key_LaunchMail;

        case 0x00000046:
            return Qt::Key_Equal;

        case 0x00000040:
            return Qt::Key_Explorer;

        case 0x00000003:
            return Qt::Key_Home;

        case 0x00000047:
            return Qt::Key_BracketLeft;

        case 0x0000005a: // KEYCODE_MEDIA_FAST_FORWARD
            return Qt::Key_Forward;

        case 0x00000057:
            return Qt::Key_MediaNext;

        case 0x00000055:
            return Qt::Key_MediaPlay;

        case 0x00000058:
            return Qt::Key_MediaPrevious;

        case 0x00000059:
            return Qt::Key_AudioRewind;

        case 0x00000056:
            return Qt::Key_MediaStop;

        case 0x00000052: //KEYCODE_MENU
            return Qt::Key_Menu;

        case 0x00000045:
            return Qt::Key_Minus;

        case 0x0000005b:
            return Qt::Key_VolumeMute;

        case 0x0000004e:
            return Qt::Key_NumLock;

        case 0x00000038:
            return Qt::Key_Period;

        case 0x00000051:
            return Qt::Key_Plus;

        case 0x0000001a:
            return Qt::Key_PowerOff;

        case 0x00000048:
            return Qt::Key_BracketRight;

        case 0x00000054:
            return Qt::Key_Search;

        case 0x0000004a:
            return Qt::Key_Semicolon;

        case 0x0000003b:
        case 0x0000003c:
            return Qt::Key_Shift;

        case 0x0000004c:
            return Qt::Key_Slash;

        case 0x00000001:
            return Qt::Key_Left;

        case 0x00000002:
            return Qt::Key_Right;

        case 0x0000003e:
            return Qt::Key_Space;

        case 0x0000003f: // KEYCODE_SYM
            return Qt::Key_Meta;

        case 0x0000003d:
            return Qt::Key_Tab;

        case 0x00000019:
            return Qt::Key_VolumeDown;

        case 0x00000018:
            return Qt::Key_VolumeUp;

        case 0x00000000: // KEYCODE_UNKNOWN
        case 0x00000011: // KEYCODE_STAR ?!?!?
        case 0x00000012: // KEYCODE_POUND ?!?!?
        case 0x00000053: // KEYCODE_NOTIFICATION ?!?!?
        case 0x0000004f: // KEYCODE_HEADSETHOOK ?!?!?
        case 0x00000044: // KEYCODE_GRAVE ?!?!?
        case 0x00000050: // KEYCODE_FOCUS ?!?!?
            return Qt::Key_Any;

        default:
            return 0;

    }
}

static void keyDown(JNIEnv */*env*/, jobject /*thiz*/, jint key, jint unicode, jint modifier)
{
    qDebug()<<"keyDown";
    Qt::KeyboardModifiers modifiers;
    if (modifier & 1)
        modifiers|=Qt::AltModifier;

    if (modifier & 2)
        modifiers|=Qt::ShiftModifier;

    if (modifier & 4)
        modifiers|=Qt::MetaModifier;

    int mappedKey=mapAndroidKey(key);
//    if (mappedKey==Qt::Key_Close)
//    {
//        qDebug()<<"handleCloseEvent";
//        QWindowSystemInterface::handleCloseEvent(QApplication::topLevelWidgets().last());
//    }
//    else
    // sometimes this method doesn't wakeup the loop, report this issue to nokia !!!
    QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyPress, mappedKey, modifiers, QChar(unicode),true);
}

static void keyUp(JNIEnv */*env*/, jobject /*thiz*/, jint key, jint unicode, jint modifier)
{
    qDebug()<<"keyUp";
    Qt::KeyboardModifiers modifiers;
    if (modifier & 1)
        modifiers|=Qt::AltModifier;

    if (modifier & 2)
        modifiers|=Qt::ShiftModifier;

    if (modifier & 4)
        modifiers|=Qt::MetaModifier;

    int mappedKey=mapAndroidKey(key);
    if (mappedKey==Qt::Key_Close)
    {
        if(!m_androidGraphicsSystem || !qApp)
            return;
        qApp->quit();
//        qDebug()<<"handleCloseEvent"<<m_androidGraphicsSystem->getPrimaryScreen()->topWindow()<<qApp->activeWindow();
//#warning FIXME
//        // sometimes qApp->activeWindow() is null, it should never be null when you have at least one widget visible, report this issue to nokia !!!
//        // how to reproduce it ?
//        // create a dialog, create another dialog form previous dialog, close the last dialog.
//        QWindowSystemInterface::handleCloseEvent(qApp->activeWindow()?qApp->activeWindow():m_androidGraphicsSystem->getPrimaryScreen()->topWindow());
    }
    else
        // sometimes this method doesn't wakeup the loop, report this issue to nokia !!!
        QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyRelease, mapAndroidKey(key), modifiers, QChar(unicode),true);
}

static void lockSurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
}

static void unlockSurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.unlock();
}

static void updateWindow(JNIEnv */*env*/, jobject /*thiz*/)
{
    if(!m_androidGraphicsSystem ||  !qApp)
        return;

    foreach(QWidget * w, qApp->topLevelWidgets())
        w->update();
}

static JNINativeMethod methods[] = {
    {"startQtAndroidPlugin", "()Z", (void *)startQtAndroidPlugin},
    {"pauseQtApp", "()V", (void *)pauseQtApp},
    {"resumeQtApp", "()V", (void *)resumeQtApp},
    {"quitQtAndroidPlugin", "()V", (void *)quitQtAndroidPlugin},
    {"terminateQt", "()V", (void *)terminateQt},
    {"setDisplayMetrics", "(IIIIDD)V", (void *)setDisplayMetrics},
    {"setSurface", "(Ljava/lang/Object;)V", (void *)setSurface},
    {"destroySurface", "()V", (void *)destroySurface},
    {"lockSurface", "()V", (void *)lockSurface},
    {"unlockSurface", "()V", (void *)unlockSurface},
    {"updateWindow", "()V", (void *)updateWindow},
    {"touchBegin","(I)V",(void*)touchBegin},
    {"touchAdd","(IIIZIIFF)V",(void*)touchAdd},
    {"touchEnd","(II)V",(void*)touchEnd},
    {"mouseDown", "(III)V", (void *)mouseDown},
    {"mouseUp", "(III)V", (void *)mouseUp},
    {"mouseMove", "(III)V", (void *)mouseMove},
    {"keyDown", "(III)V", (void *)keyDown},
    {"keyUp", "(III)V", (void *)keyUp}
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz=env->FindClass(className);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    m_applicationClass = (jclass)env->NewGlobalRef(clazz);
    if (env->RegisterNatives(m_applicationClass, gMethods, numMethods) < 0)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    m_redrawSurfaceMethodID = env->GetStaticMethodID(m_applicationClass, "redrawSurface", "(IIII)V");
    m_showSoftwareKeyboardMethodID = env->GetStaticMethodID(m_applicationClass, "showSoftwareKeyboard", "()V");
    m_hideSoftwareKeyboardMethodID = env->GetStaticMethodID(m_applicationClass, "hideSoftwareKeyboard", "()V");
    m_setFullScreenMethodID = env->GetStaticMethodID(m_applicationClass, "setFullScreen", "(Z)V");

#ifdef ANDROID_PLUGIN_OPENGL
    clazz=env->FindClass(SurfaceClassPathName);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    m_surfaceFieldID = env->GetFieldID(clazz, ANDROID_VIEW_SURFACE_JNI_ID, "I");
#endif

    jmethodID methodID=env->GetStaticMethodID(m_applicationClass, "activity", "()Landroid/app/Activity;");
    jobject activityObject=env->CallStaticObjectMethod(m_applicationClass, methodID);

    clazz=env->FindClass(ContextWrapperClassPathName);
    methodID=env->GetMethodID(clazz, "getAssets", "()Landroid/content/res/AssetManager;");
    m_assetManager=AAssetManager_fromJava(env, env->CallObjectMethod(activityObject, methodID));


    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, QtNativeClassPathName, methods, sizeof(methods) / sizeof(methods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

typedef union {
    JNIEnv* nativeEnvironment;
    void* venv;
} UnionJNIEnvToVoid;

Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    __android_log_print(ANDROID_LOG_INFO,"Qt", "qt start");
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    m_javaVM = 0;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }
    JNIEnv *env = uenv.nativeEnvironment;
    if (!registerNatives(env))
    {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "registerNatives failed");
        return -1;
    }
    m_javaVM = vm;
    return JNI_VERSION_1_4;
}
