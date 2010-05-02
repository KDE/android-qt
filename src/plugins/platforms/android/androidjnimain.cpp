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
#include "androidjnimain.h"
#include "qandroidplatformintegration.h"
#include <QWindowSystemInterface>
#include <QApplication>
#include <qabstracteventdispatcher.h>

#ifdef QT_USE_CUSTOM_NDK
    #include <ui/Surface.h>
    #include <qmap.h>
#endif

Q_IMPORT_PLUGIN (QtAndroid)

static JavaVM *m_javaVM = NULL;
static jobject m_applicationObject  = NULL;
static jobject m_EglObject  = NULL;

// Application semaphore
static QSemaphore m_quitAppSemaphore;

// Surface semaphores
static QSemaphore m_createSurfaceSemaphore;
static QSemaphore m_resizeSurfaceSemaphore;
static QSemaphore m_destroySurfaceSemaphore;
// Surface semaphores

// Java surface methods
static jmethodID m_createSurfaceMethodID=0;
static jmethodID m_resizeSurfaceMethodID=0;
static jmethodID m_destroySurfaceMethodID=0;
static jmethodID m_setSurfaceVisiblityMethodID=0;
static jmethodID m_setSurfaceOpacityMethodID=0;
static jmethodID m_setWindowTitleMethodID=0;
static jmethodID m_raiseSurfaceMethodID=0;
// Java surface methods

// Java EGL methods
static jmethodID m_makeCurrentMethodID=0;
static jmethodID m_doneCurrentMethodID=0;
static jmethodID m_swapBuffersMethodID=0;
//static jmethodID m_getProcAddressMethodID=0;
// Java EGL methods

#ifndef QT_USE_CUSTOM_NDK
// Java methods
static jmethodID m_flushImageMethodID=0;
// Java methods
#else
static jfieldID  IDsurface;
static QMutex m_surfaceMutex;
static QMap<int,android::Surface*> m_surfaces;
#endif

static QMutex m_resizeMutex;
static bool   m_requestResize=false;

static QThread * m_qtThread=0;
static QAndroidPlatformIntegration * mAndroidGraphicsSystem=0;


static const char *QtApplicationClassPathName = "com/nokia/qt/QtApplication";
static const char *QtEglClassPathName = "com/nokia/qt/QtEgl";


namespace QtAndroid
{
    void flushImage(int surfaceId, const QPoint & pos, const QImage & image, const QRect & destinationRect)
    {

#ifdef QT_USE_CUSTOM_NDK
        QMutexLocker locker(&m_surfaceMutex);
        if (!m_surfaces.contains(surfaceId))
            return;

        android::Surface* m_surface=m_surfaces[surfaceId];

        //qDebug()<<"flushImage"<<pos<<destinationRect;
        android::Surface::SurfaceInfo info;
        android::Region r(android::Rect(pos.x()+destinationRect.x(), pos.y()+destinationRect.y(), pos.x()+destinationRect.right(), pos.y()+destinationRect.bottom() ));
        android::status_t err = m_surface->lock(&info, &r);
        if(err < 0)
        {
            qWarning()<<"Unable to lock the surface";
            return;
        }

//        qDebug()<<pos<<destinationRect<<info.w<<info.h;

        if ((unsigned)pos.x()>=info.w|| (unsigned)pos.y()>=info.h)
        {
            qWarning()<<"Invalid coordonates";
            m_surface->unlockAndPost();
            return;
        }

#if 1
        Q_UNUSED(destinationRect)
        int bpp=2;

        if (pos.x()==0 && pos.y()==0 && info.w==(unsigned)image.size().width() && info.h==(unsigned)image.size().height()
#if ANDROID_PLATFORM<5
            && info.bpr==(unsigned)image.bytesPerLine()
#else
            && info.s*bpp==(unsigned)image.bytesPerLine()
#endif
            )
        {
#if ANDROID_PLATFORM<5
            memcpy(info.bits, (const uchar*)image.bits(), info.bpr*info.h);
#else
            memcpy(info.bits, (const uchar*)image.bits(), info.s*2*info.h);
#endif
            m_surface->unlockAndPost();
            return;
        }
#if ANDROID_PLATFORM<5
        int sbpl=info.bpr;
#else
        int sbpl=info.s*bpp;
#endif
        unsigned sposx=pos.x();
        unsigned sposy=pos.y();

        unsigned char * screenBits=(unsigned char*)info.bits;
        screenBits+=sposy*sbpl;

        unsigned ibpl=image.bytesPerLine();

        unsigned char * imageBits=(unsigned char *)((const uchar*)image.bits());

        unsigned width=info.w-sposx<(unsigned)image.size().width()?info.w-sposx:(unsigned)image.size().width();
        unsigned height=info.h-sposy<(unsigned)image.size().height()?info.h-sposy:(unsigned)image.size().height();

        for (unsigned y=0;y<height;y++)
            memcpy(screenBits+y*sbpl+sposx*bpp,
                    imageBits+y*ibpl,
                   width*bpp);
#else
        int bpp=2;
#if ANDROID_PLATFORM<5
        int sbpl=info.bpr;
#else
        int sbpl=info.s*bpp;
#endif
        unsigned sposx=pos.x()+destinationRect.x();
        unsigned sposy=pos.y()+destinationRect.y();

        unsigned char * screenBits=(unsigned char*)info.bits;
        screenBits+=sposy*sbpl;

        unsigned ibpl=image.bytesPerLine();
        unsigned iposx=destinationRect.x();
        unsigned iposy=destinationRect.y();

        unsigned char * imageBits=(unsigned char *)((const uchar*)image.bits());
        imageBits+=iposy*ibpl;

        unsigned width=info.w-sposx<(unsigned)destinationRect.width()?info.w-sposx:destinationRect.width();
        unsigned height=info.h-sposy<(unsigned)destinationRect.height()?info.h-sposy:destinationRect.height();

        //qDebug()<<ibpl<<sbpl<<sxpos<<sypos<<width<<height<<sxpos+width<<sypos+height;

        for (unsigned y=0;y<height;y++)
            memcpy(screenBits+y*sbpl+sposx*bpp,
                    imageBits+y*ibpl+iposx*bpp,
                   width*bpp);
#endif
        m_surface->unlockAndPost();
#else
        JNIEnv* env;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jclass object=env->GetObjectClass(m_applicationObject);
        QImage imag=image.copy(destinationRect);
        jshortArray img=env->NewShortArray(image.byteCount()/2);
        env->SetShortArrayRegion(img,0,imag.byteCount(), (const jshort*)(const uchar *)imag.bits());
        env->CallVoidMethod(object, m_flushImageMethodID,
                            (jint)img, (jint)image.bytesPerLine(),
                            (jint)destinationRect.x(), (jint)destinationRect.y(),
                            (jint)destinationRect.right(), (jint)destinationRect.bottom());
        env->DeleteLocalRef(img);
        m_javaVM->DetachCurrentThread();
#endif
    }

    void setAndroidGraphicsSystem(QAndroidPlatformIntegration * androidGraphicsSystem)
    {
        mAndroidGraphicsSystem=androidGraphicsSystem;
    }

    void setQtThread(QThread * thread)
    {
        m_qtThread=thread;
    }

    void quitApplication()
    {
    }

    bool createSurface(int surfaceId, int l, int t, int r, int b)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }

        bool res=env->CallBooleanMethod(m_applicationObject, m_createSurfaceMethodID,
                            (jint)surfaceId, (jint)l, (jint)t, (jint)r, (jint)b);

        m_javaVM->DetachCurrentThread();
        if (!res)
            return false;
        m_createSurfaceSemaphore.acquire(); //wait until surface is created
        return m_surfaces.contains(surfaceId);
    }

    bool resizeSurface(int surfaceId, int l, int t, int r, int b)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        m_resizeMutex.lock();
        m_requestResize=false;

        bool res=env->CallBooleanMethod(m_applicationObject, m_resizeSurfaceMethodID,
                            (jint)surfaceId, (jint)l, (jint)t, (jint)r, (jint)b);

        m_javaVM->DetachCurrentThread();
        if (!res)
        {
            m_resizeMutex.unlock();
            return false;
        }

        m_requestResize=true;
        m_resizeMutex.unlock();

        m_resizeSurfaceSemaphore.acquire(); //wait until surface is resized
        return m_surfaces.contains(surfaceId);
    }

    bool destroySurface(int surfaceId)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }

        bool res=env->CallBooleanMethod(m_applicationObject, m_destroySurfaceMethodID, (jint)surfaceId);

        m_javaVM->DetachCurrentThread();

        if (!res)
            return false;
        m_destroySurfaceSemaphore.acquire(); //wait until surface is destroyed
        return !m_surfaces.contains(surfaceId);
    }

    void setSurfaceVisiblity(int surfaceId, bool visible)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        env->CallVoidMethod(m_applicationObject, m_setSurfaceVisiblityMethodID, (jint)surfaceId, (jboolean)visible);

        m_javaVM->DetachCurrentThread();
    }

    void setSurfaceOpacity(int surfaceId, double level)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jclass object=env->GetObjectClass(m_applicationObject);
        env->CallVoidMethod(object, m_setSurfaceOpacityMethodID, (jint)surfaceId, (jdouble)level);
        m_javaVM->DetachCurrentThread();
    }

    void setWindowTitle(int surfaceId, const QString & title)
    {
        if (!title.length())
            return;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jstring jtitle = env->NewStringUTF(title.toUtf8().data());
        env->CallVoidMethod(m_applicationObject, m_setWindowTitleMethodID, (jint)surfaceId, (jstring)jtitle);
        env->DeleteLocalRef(jtitle);
        m_javaVM->DetachCurrentThread();
    }

    void raiseSurface(int surfaceId)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jclass object=env->GetObjectClass(m_applicationObject);
        env->CallVoidMethod(object, m_raiseSurfaceMethodID, (jint)surfaceId);
        m_javaVM->DetachCurrentThread();
    }

    bool makeCurrent(int surfaceId)
    {
        qDebug()<<"makeCurrent"<<surfaceId;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        env->CallVoidMethod(m_EglObject, m_makeCurrentMethodID, (jint)surfaceId);
        m_javaVM->DetachCurrentThread();
        return false;
    }

    bool doneCurrent()
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        env->CallVoidMethod(m_EglObject, m_doneCurrentMethodID);
        m_javaVM->DetachCurrentThread();
        return false;
    }

    bool swapBuffers(int surfaceId)
    {
        qDebug()<<"swapBuffers"<<surfaceId;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        env->CallVoidMethod(m_EglObject, m_swapBuffersMethodID, (jint)surfaceId);
        m_javaVM->DetachCurrentThread();
        return false;
    }

    void * getProcAddress(int surfaceId, const QString& procName)
    {
        qDebug()<<"getProcAddress"<<surfaceId<<procName;
        Q_UNUSED(procName)
        Q_UNUSED(surfaceId)
//        JNIEnv* env;
//        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
//        {
//            qCritical()<<"AttachCurrentThread failed";
//            return false;
//        }
//        jstring jprocName = env->NewStringUTF(procName.toUtf8().data());
//        env->CallVoidMethod(m_EglObject, m_getProcAddressMethodID, (jint)surfaceId, jprocName);
//        env->DeleteLocalRef(jprocName);
//        m_javaVM->DetachCurrentThread();
        return NULL;
    }
    bool hasOpenGL()
    {
        return (bool)m_EglObject;
    }

}

extern "C" int main(int, char **); //use the standard main method to start the application
static void * startMainMethod(void * /*data*/)
{
    char ** params;
    params=(char**)malloc(sizeof(char*)*2);
    params[0]=(char*)malloc(20);
    strcpy(params[0],"QtApp");
    params[1]=(char*)malloc(20);
    strcpy(params[1],"-platform");
    params[2]=(char*)malloc(20);
    strcpy(params[2],"android");
    int ret = main(3, params);
    free(params[2]);
    free(params[1]);
    free(params[0]);
    free(params);
    Q_UNUSED(ret);
    m_quitAppSemaphore.release();
    return NULL;
}

static jboolean startQtApp(JNIEnv* /*env*/, jobject /*object*/)
{
    m_qtThread=0;
#ifdef QT_USE_CUSTOM_NDK
    m_surfaces.clear();
#endif
    mAndroidGraphicsSystem=0;
    m_requestResize=false;
    pthread_t appThread;
    return pthread_create(&appThread, NULL, startMainMethod, NULL)==0;
}

static void pauseQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{

    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->interrupt();
}

static void resumeQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    if (mAndroidGraphicsSystem)
        mAndroidGraphicsSystem->updateScreen();
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
}

static void quitQtApp(JNIEnv* /*env*/, jclass /*clazz*/)
{
    QApplication::postEvent(qApp, new QEvent(QEvent::Quit));
    m_quitAppSemaphore.acquire();
}

static void setDisplayMetrics(JNIEnv* /*env*/, jclass /*clazz*/,
                              jint widthPixels, jint heightPixels,
                              jfloat xdpi, jfloat ydpi)
{
    if (!mAndroidGraphicsSystem)
        QAndroidPlatformIntegration::setDefaultDisplayMetrics(widthPixels,heightPixels-25,
                                                         qRound((double)widthPixels   / xdpi * 100 / 2.54 ),
                                                         qRound((double)heightPixels / ydpi *100  / 2.54 ));
    else
    {
        mAndroidGraphicsSystem->setDesktopSize(widthPixels, heightPixels);
        mAndroidGraphicsSystem->setDisplayMetrics(qRound((double)widthPixels   / xdpi * 100 / 2.54 ),
                                                  qRound((double)heightPixels / ydpi *100  / 2.54 ));
    }
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
}


static void mouseDown(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(0, QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButtons(Qt::LeftButton));
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
}

static void mouseUp(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(0, QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButtons(Qt::NoButton));
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
}

static void mouseMove(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(0, QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButtons(Qt::LeftButton));
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
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
            return Qt::Key_TopMenu;

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
        default:
            return Qt::Key_Any;

    }
}

static void keyDown(JNIEnv */*env*/, jobject /*thiz*/, jint key, jint unicode, jint modifier)
{
    Qt::KeyboardModifiers modifiers;
    if (modifier & 1)
        modifiers|=Qt::AltModifier;

    if (modifier & 2)
        modifiers|=Qt::ShiftModifier;

    if (modifier & 4)
        modifiers|=Qt::MetaModifier;
    QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyPress, mapAndroidKey(key), modifiers, QChar(unicode),true);
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
}

static void keyUp(JNIEnv */*env*/, jobject /*thiz*/, jint key, jint unicode, jint modifier)
{
    Qt::KeyboardModifiers modifiers;
    if (modifier & 1)
        modifiers|=Qt::AltModifier;

    if (modifier & 2)
        modifiers|=Qt::ShiftModifier;

    if (modifier & 4)
        modifiers|=Qt::MetaModifier;

    QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyRelease, mapAndroidKey(key), modifiers, QChar(unicode),true);
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
}

static void surfaceCreated(JNIEnv *env, jobject /*thiz*/, jobject jSurface, jint surfaceId)
{
    qDebug()<<"surfaceCreated"<<surfaceId;
    m_surfaceMutex.lock();
    m_surfaces[surfaceId] = reinterpret_cast<android::Surface*>(env->GetIntField(jSurface, IDsurface));
    m_surfaceMutex.unlock();
    m_createSurfaceSemaphore.release();
}

static void surfaceChanged(JNIEnv *env, jobject /*thiz*/, jobject jSurface, jint surfaceId)
{
    qDebug()<<"surfaceChanged"<<surfaceId;
    m_surfaceMutex.lock();
    m_surfaces[surfaceId] = reinterpret_cast<android::Surface*>(env->GetIntField(jSurface, IDsurface));
    m_surfaceMutex.unlock();
    m_resizeMutex.lock();
    if (m_requestResize)
        m_resizeSurfaceSemaphore.release();
    m_requestResize=false;
    m_resizeMutex.unlock();
}

static void surfaceDestroyed(JNIEnv */*env*/, jobject /*thiz*/, jint surfaceId)
{
    qDebug()<<"surfaceDestroyed"<<surfaceId;
    m_surfaceMutex.lock();
    m_surfaces.remove(surfaceId);
    m_surfaceMutex.unlock();
    m_destroySurfaceSemaphore.release();
}

static void setEglObject(JNIEnv *env, jobject /*thiz*/,  jobject eglObject)
{
    jclass clazz = env->FindClass(QtEglClassPathName);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Unable to find class '%s'", QtEglClassPathName);
        return;
    }

    m_EglObject=env->NewGlobalRef(eglObject);
    m_makeCurrentMethodID = env->GetMethodID((jclass)clazz, "makeCurrent", "(I)Z");
    m_doneCurrentMethodID = env->GetMethodID((jclass)clazz, "doneCurrent", "()V");
    m_swapBuffersMethodID = env->GetMethodID((jclass)clazz, "swapBuffers", "(I)Z");
//    m_getProcAddressMethodID = env->GetMethodID((jclass)m_EglObject, "getProcAddress", "(I)Z");
}

static JNINativeMethod methods[] = {
    {"startQtApp", "()V", (void *)startQtApp},
    {"pauseQtApp", "()V", (void *)pauseQtApp},
    {"resumeQtApp", "()V", (void *)resumeQtApp},
    {"quitQtApp", "()V", (void *)quitQtApp},
    {"setEglObject", "(Ljava/lang/Object;)V", (void *)setEglObject},
    {"setDisplayMetrics", "(IIFF)V", (void *)setDisplayMetrics},
    {"surfaceCreated", "(Landroid/view/Surface;I)V", (void *)surfaceCreated},
    {"surfaceChanged", "(Landroid/view/Surface;I)V", (void *)surfaceChanged},
    {"surfaceDestroyed", "(I)V", (void *)surfaceDestroyed},
    {"mouseDown", "(II)V", (void *)mouseDown},
    {"mouseUp", "(II)V", (void *)mouseUp},
    {"mouseMove", "(II)V", (void *)mouseMove},
    {"keyDown", "(III)V", (void *)keyDown},
    {"keyUp", "(III)V", (void *)keyUp}
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    m_applicationObject = clazz;
    m_applicationObject = env->NewGlobalRef(m_applicationObject);
#ifndef QT_USE_CUSTOM_NDK
    m_flushImageMethodID = env->GetMethodID((jclass)m_applicationObject, "flushImage", "(Ljava/nio/ShortBuffer;IIIII)V");
#endif
    m_createSurfaceMethodID = env->GetMethodID((jclass)m_applicationObject, "createSurface", "(IIIII)Z");
    m_resizeSurfaceMethodID = env->GetMethodID((jclass)m_applicationObject, "resizeSurface", "(IIIII)Z");
    m_destroySurfaceMethodID = env->GetMethodID((jclass)m_applicationObject, "destroySurface", "(I)Z");
    m_setSurfaceVisiblityMethodID = env->GetMethodID((jclass)m_applicationObject, "setSurfaceVisiblity", "(IZ)V");
    m_setSurfaceOpacityMethodID = env->GetMethodID((jclass)m_applicationObject, "setSurfaceOpacity", "(ID)V");
    m_setWindowTitleMethodID = env->GetMethodID((jclass)m_applicationObject, "setWindowTitle", "(ILjava/lang/String;)V");
    m_raiseSurfaceMethodID = env->GetMethodID((jclass)m_applicationObject, "raiseSurface", "(I)V");
    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, QtApplicationClassPathName, methods, sizeof(methods) / sizeof(methods[0])))
        return JNI_FALSE;

#ifdef QT_USE_CUSTOM_NDK
    jclass clazz = env->FindClass("android/view/Surface");
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Can't find android/view/Surface");
        return JNI_FALSE;
    }

    IDsurface = env->GetFieldID(clazz, "mSurface", "I");
    if (IDsurface == NULL) {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Can't find android/view/Surface mSurface");
        return JNI_FALSE;
    }
#endif
    return JNI_TRUE;
}

typedef union {
    JNIEnv* nativeEnvironment;
    void* venv;
} UnionJNIEnvToVoid;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    __android_log_print(ANDROID_LOG_INFO,"Qt", "qt start");
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    JNIEnv* env = NULL;

    m_javaVM = 0;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }
    env = uenv.nativeEnvironment;
    if (!registerNatives(env))
    {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "registerNatives failed");
        return -1;
    }
    m_javaVM = vm;
    return JNI_VERSION_1_4;
}
