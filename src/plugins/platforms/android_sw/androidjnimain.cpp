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

#ifdef JNIGRPAHICS
#   include <android/bitmap.h>
    static jmethodID m_redrawSurfaceMethodID=0;
#else
    static jmethodID m_flushImageMethodID=0;
#endif

Q_IMPORT_PLUGIN (QtAndroid)

static JavaVM *m_javaVM = NULL;
static JNIEnv *m_env = NULL;
static jobject m_applicationObject  = NULL;
static jobject m_surface = NULL;
static QList<QWindowSystemInterface::TouchPoint> m_touchPoints;

static QSemaphore m_quitAppSemaphore;
static QMutex m_surfaceMutex;
static QSemaphore m_pauseApplicationSemaphore;
static QMutex m_pauseApplicationMutex;

static QAndroidPlatformIntegration * mAndroidGraphicsSystem=0;
static int m_desktopWidthPixels=0, m_desktopHeightPixels=0;
static const char *QtApplicationClassPathName = "com/nokia/qt/QtApplication";

static volatile bool m_pauseApplication;


static inline void checkPauseApplication()
{
    qDebug()<<"checkPauseApplication";
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
    qDebug()<<"checkPauseApplication !!!";
}

namespace QtAndroid
{
    void flushImage(const QPoint & pos, const QImage & image, const QRect & destinationRect)
    {
        checkPauseApplication();
        JNIEnv* env;
        QMutexLocker locker(&m_surfaceMutex);
        if (!m_surface)
            return;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        int bpp=2;
#ifdef JNIGRPAHICS
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
#endif

        unsigned ibpl=image.bytesPerLine();
        unsigned iposx=destinationRect.x();
        unsigned iposy=destinationRect.y();

        unsigned char * imageBits=(unsigned char *)((const uchar*)image.bits());
        imageBits+=iposy*ibpl;

#ifdef JNIGRPAHICS
        unsigned width=swidth-sposx<(unsigned)destinationRect.width()?swidth-sposx:destinationRect.width();
        unsigned height=sheight-sposy<(unsigned)destinationRect.height()?sheight-sposy:destinationRect.height();
#else
        Q_UNUSED(pos)
        unsigned width=destinationRect.width();
        unsigned height=destinationRect.height();
#endif

        //qDebug()<<ibpl<<sbpl<<sxpos<<sypos<<width<<height<<sxpos+width<<sypos+height;

        jclass object=env->GetObjectClass(m_applicationObject);
#ifdef JNIGRPAHICS
        for (unsigned y=0;y<height;y++)
            memcpy(screenBits+y*sbpl+sposx*bpp,
                    imageBits+y*ibpl+iposx*bpp,
                   width*bpp);
        AndroidBitmap_unlockPixels(env, m_surface);
        env->CallVoidMethod(object, m_redrawSurfaceMethodID,
                            (jint)destinationRect.left(),
                            (jint)destinationRect.top(),
                            (jint)destinationRect.right(),
                            (jint)destinationRect.bottom());
#else
        jshort* screenBits=(jshort*)malloc(width*height*sizeof(jshort));
        for (unsigned y=0;y<height;y++)
            memcpy(screenBits+y*width,imageBits+y*ibpl+iposx*bpp,width*bpp);
        jshortArray img=env->NewShortArray(width*height);
        env->SetShortArrayRegion(img,0,width*height, screenBits);
        env->CallVoidMethod(object, m_flushImageMethodID,
                             img,
                             (jint)destinationRect.left(),
                             (jint)destinationRect.top(),
                             (jint)destinationRect.right(),
                             (jint)destinationRect.bottom());
        env->DeleteLocalRef(img);
        free(screenBits);
#endif
        m_javaVM->DetachCurrentThread();
    }

    void setAndroidGraphicsSystem(QAndroidPlatformIntegration * androidGraphicsSystem)
    {
        mAndroidGraphicsSystem=androidGraphicsSystem;
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
#ifdef JNIGRPAHICS
        m_surface=0;
#endif
    mAndroidGraphicsSystem=0;
    pthread_t appThread;
    return pthread_create(&appThread, NULL, startMainMethod, NULL)==0;
}

static void pauseQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
    m_pauseApplicationMutex.lock();
    if (mAndroidGraphicsSystem)
        mAndroidGraphicsSystem->pauseApp();
    m_pauseApplication=true;
    m_pauseApplicationMutex.unlock();
    m_surfaceMutex.unlock();
}

static void resumeQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
    m_pauseApplicationMutex.lock();
    if (mAndroidGraphicsSystem)
        mAndroidGraphicsSystem->resumeApp();

    if (m_pauseApplication)
        m_pauseApplicationSemaphore.release();

    m_pauseApplicationMutex.unlock();
    m_surfaceMutex.unlock();
}

static void quitQtApp(JNIEnv* /*env*/, jclass /*clazz*/)
{
    QApplication::postEvent(qApp, new QEvent(QEvent::Quit));
    m_quitAppSemaphore.acquire();
    qDebug()<<"***** quitQtApp *****";
}

static void setSurface(JNIEnv *env, jobject /*thiz*/, jobject jSurface)
{
    m_surface = env->NewGlobalRef(jSurface);
}

static void destroySurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surface = 0;
}

static void setDisplayMetrics(JNIEnv* /*env*/, jclass /*clazz*/,
                              jint widthPixels, jint heightPixels,
                              jint desktopWidthPixels, jint desktopHeightPixels,
                              jfloat xdpi, jfloat ydpi)
{
    m_desktopWidthPixels=desktopWidthPixels;
    m_desktopHeightPixels=desktopHeightPixels;
    if (!mAndroidGraphicsSystem)
        QAndroidPlatformIntegration::setDefaultDisplayMetrics(desktopWidthPixels,desktopHeightPixels,
                                                     qRound((double)widthPixels   / xdpi * 100 / 2.54 ),
                                                     qRound((double)heightPixels / ydpi *100  / 2.54 ));
    else
    {
        mAndroidGraphicsSystem->setDesktopSize(desktopWidthPixels,desktopHeightPixels);
        mAndroidGraphicsSystem->setDisplayMetrics(qRound((double)widthPixels   / xdpi * 100 / 2.54 ),
                                                  qRound((double)heightPixels / ydpi *100  / 2.54 ));
        if (m_surface)
        {
            QWindowSystemInterface::handleScreenAvailableGeometryChange(0);
            QWindowSystemInterface::handleScreenGeometryChange(0);
        }
    }
}


static void mouseDown(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(0, QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButtons(Qt::LeftButton));
}

static void mouseUp(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(0, QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButtons(Qt::NoButton));
}

static void mouseMove(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(0, QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButtons(Qt::LeftButton));
}

static void touchBegin(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_touchPoints.clear();
}

static void touchAdd(JNIEnv */*env*/, jobject /*thiz*/, jint id, jint action, jboolean primary, jint x, jint y, jfloat size, jfloat pressure)
{
    Qt::TouchPointStates state=Qt::TouchPointStationary;
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
    if (!size)
        size=0.1;
    touchPoint.area=QRectF(x-((double)m_desktopWidthPixels*size)/2,
                           y-((double)m_desktopHeightPixels*size)/2,
                           (double)m_desktopWidthPixels*size,
                           (double)m_desktopHeightPixels*size);
    m_touchPoints.push_back(touchPoint);
//    if (id==1)
//        qDebug()<<"Duda"<<x<<y<<touchPoint.normalPosition;
//        qDebug()<<"Duda"<<id<<action<<primary<<x<<y<<size<<pressure
//                <<touchPoint.area<<touchPoint.normalPosition
//                <<m_desktopWidthPixels<<m_desktopHeightPixels;
}

static void touchEnd(JNIEnv */*env*/, jobject /*thiz*/, jint action)
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
}

static void lockSurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.lock();
}

static void unlockSurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surfaceMutex.unlock();
}

static JNINativeMethod methods[] = {
    {"startQtApp", "()V", (void *)startQtApp},
    {"pauseQtApp", "()V", (void *)pauseQtApp},
    {"resumeQtApp", "()V", (void *)resumeQtApp},
    {"quitQtApp", "()V", (void *)quitQtApp},
    {"setDisplayMetrics", "(IIIIFF)V", (void *)setDisplayMetrics},
    {"setSurface", "(Ljava/lang/Object;)V", (void *)setSurface},
    {"destroySurface", "()V", (void *)destroySurface},
    {"lockSurface", "()V", (void *)lockSurface},
    {"unlockSurface", "()V", (void *)unlockSurface},
    {"touchBegin","()V",(void*)touchBegin},
    {"touchAdd","(IIZIIFF)V",(void*)touchAdd},
    {"touchEnd","(I)V",(void*)touchEnd},
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
    m_applicationObject = env->NewGlobalRef(clazz);

#ifdef JNIGRPAHICS
    m_redrawSurfaceMethodID = env->GetMethodID((jclass)m_applicationObject, "redrawSurface", "(IIII)V");
#else
    m_flushImageMethodID = env->GetMethodID((jclass)m_applicationObject, "flushImage", "([SIIII)V");
#endif

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, QtApplicationClassPathName, methods, sizeof(methods) / sizeof(methods[0])))
        return JNI_FALSE;

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
    m_javaVM = 0;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt","GetEnv failed");
        return -1;
    }
    m_env = uenv.nativeEnvironment;
    if (!registerNatives(m_env))
    {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "registerNatives failed");
        return -1;
    }
    m_javaVM = vm;
    return JNI_VERSION_1_4;
}
