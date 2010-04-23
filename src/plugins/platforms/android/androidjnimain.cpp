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
static jobject m_object  = NULL;
static jmethodID m_flushImageMethodID=0;

static QSemaphore m_quitAppSemaphore;
static QMutex m_surfaceMutex;
static QThread * m_qtThread=0;

#ifdef QT_USE_CUSTOM_NDK
    static jfieldID  IDsurface;
    static android::Surface* m_surface;
#endif

static QAndroidPlatformIntegration * mAndroidGraphicsSystem=0;
namespace QtAndroid
{
    JavaVM * getJavaVM()
    {
        return m_javaVM;
    }

    jobject getJniObject()
    {
        return m_object;
    }

    void flushImage(const QPoint & pos, const QImage & image, const QRect & destinationRect)
    {
#ifdef QT_USE_CUSTOM_NDK
        QMutexLocker locker(&m_surfaceMutex);

        if (!m_surface)
            return;
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
        m_javaVM->AttachCurrentThread(&env, NULL);
        QImage imag=image.copy(destinationRect);
        jshortArray img=env->NewShortArray(image.byteCount()/2);
        env->SetShortArrayRegion(img,0,imag.byteCount(), (const jshort*)(const uchar *)imag.bits());
        env->CallVoidMethod(m_object, m_flushImageMethodID,
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
        m_surface=0;
    #endif
    mAndroidGraphicsSystem=0;
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
    if (QAbstractEventDispatcher::instance(m_qtThread))
            QAbstractEventDispatcher::instance(m_qtThread)->wakeUp();
    if (mAndroidGraphicsSystem)
        mAndroidGraphicsSystem->updateScreen();
}

static void quitQtApp(JNIEnv* /*env*/, jclass /*clazz*/)
{
    QApplication::postEvent(qApp, new QEvent(QEvent::Quit));
    m_quitAppSemaphore.acquire();
    qDebug()<<"***** quitQtApp *****";

}

static void setDisplayMetrics(JNIEnv* /*env*/, jclass /*clazz*/,
                              jint widthPixels, jint heightPixels,
                              jfloat xdpi, jfloat ydpi)
{
    qDebug()<<"***** setDisplayMetrics ****"<<widthPixels << heightPixels;
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
}

static void setSurface(JNIEnv *env, jobject /*thiz*/, jobject jSurface, jint w, jint h)
{
    qDebug()<<"***** setSurface ****"<<w << h;
    m_surfaceMutex.lock();
    m_surface = reinterpret_cast<android::Surface*>(env->GetIntField(jSurface, IDsurface));
    m_surfaceMutex.unlock();
    if (mAndroidGraphicsSystem)
    {
        mAndroidGraphicsSystem->setDesktopSize(w, h);
        mAndroidGraphicsSystem->updateScreen();
    }
    else
        QAndroidPlatformIntegration::setDefaultDesktopSize(w,h);
}

static void destroySurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
    m_surface = 0;
    m_surfaceMutex.unlock();
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


static const char *classPathName = "com/nokia/qt/QtApplication";

static JNINativeMethod methods[] = {
    {"startQtApp", "()V", (void *)startQtApp},
    {"pauseQtApp", "()V", (void *)pauseQtApp},
    {"resumeQtApp", "()V", (void *)resumeQtApp},
    {"quitQtApp", "()V", (void *)quitQtApp},
    {"setDisplayMetrics", "(IIFF)V", (void *)setDisplayMetrics},
    {"setSurface", "(Landroid/view/Surface;II)V", (void *)setSurface},
    {"destroySurface", "()V", (void *)destroySurface},
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
    m_object = clazz;

    m_flushImageMethodID = env->GetMethodID((jclass)m_object, "flushImage", "(Ljava/nio/ShortBuffer;IIIII)V");

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0])))
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
