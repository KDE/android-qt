#include <android/log.h>
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

#include <qmap.h>

Q_IMPORT_PLUGIN (QtAndroid)

static JavaVM *m_javaVM = 0;
static JNIEnv *m_env = 0;

template <class T>
class JavaGolbalObject
{
    public:
        JavaGolbalObject(const T & value = NULL)
        {
            if (m_env && value)
                m_value=m_env->NewGlobalRef(value);
            else
                m_value=value;
        };
        virtual ~JavaGolbalObject()
        {
            if (m_value && m_env)
                m_env->DeleteGlobalRef(m_value);
        };
        T& operator =(const T& value)
        {
            if (m_value && m_env)
                m_env->DeleteGlobalRef(m_value);
            if (m_env && value)
                m_value=m_env->NewGlobalRef(value);
            else
                m_value=value;
            return m_value;
        }
        T& operator()()
        {
            return m_value;
        }

private:
        T m_value;
};

static JavaGolbalObject<jobject> m_EglObject = 0;
static JavaGolbalObject<jobject> m_applicationObject = 0;

static QList<QWindowSystemInterface::TouchPoint> m_touchPoints;
struct ApplicationControl
{
    // Application semaphores
    QSemaphore m_quitAppSemaphore;
    QSemaphore m_pauseSemaphore;
    // Application semaphores


    // Window semaphores
    QSemaphore m_createWindowSemaphore;
    QSemaphore m_resizeWindowSemaphore;
    QSemaphore m_destroyWindowSemaphore;
    // Window semaphores

    QMutex m_windowMutex;
    QMutex m_pauseApplicationMutex;
    QSemaphore m_pauseApplicationSemaphore;
} * m_applicationControl=0;

// Java window methods
static jmethodID m_createWindowMethodID=0;
static jmethodID m_resizeWindowMethodID=0;
static jmethodID m_destroyWindowMethodID=0;
static jmethodID m_setWindowVisiblityMethodID=0;
static jmethodID m_setWindowOpacityMethodID=0;
static jmethodID m_setWindowTitleMethodID=0;
static jmethodID m_raiseWindowMethodID=0;
static jmethodID m_redrawWindowMethodID=0;
// Java window methods

// Java EGL methods
static jmethodID m_makeCurrentMethodID=0;
static jmethodID m_doneCurrentMethodID=0;
static jmethodID m_swapBuffersMethodID=0;
//static jmethodID m_getProcAddressMethodID=0;
// Java EGL methods


// Software keyboard support
static jmethodID m_showSoftwareKeyboardMethodID=0;
static jmethodID m_hideSoftwareKeyboardMethodID=0;
// Software keyboard support

struct TLWStruct
{
    JavaGolbalObject<jobject> javaObject;
    QWidget * tlw;
    ~TLWStruct()
    {
        javaObject.~JavaGolbalObject();
    }
};

static QMap<int, TLWStruct> m_windows;

static bool   m_pauseApplication;
static QAndroidPlatformIntegration * mAndroidGraphicsSystem=0;

static const char *QtApplicationClassPathName = "com/nokia/qt/QtApplication";
static const char *QtEglClassPathName = "com/nokia/qt/QtEgl";
static int m_desktopWidthPixels, m_desktopHeightPixels;


static int setQtApplicationObject(JNIEnv* env)
{
    jclass clazz;

    clazz = env->FindClass(QtApplicationClassPathName);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find class '%s'", QtApplicationClassPathName);
        return JNI_FALSE;
    }
    m_applicationObject = clazz;
    return true;
}

static inline void checkPauseApplication()
{
    m_applicationControl->m_pauseApplicationMutex.lock();
    if (m_pauseApplication)
    {
        qDebug()<<"pauseApplication"<<m_windows.size();
        m_applicationControl->m_pauseApplicationMutex.unlock();
        m_applicationControl->m_pauseApplicationSemaphore.acquire();
        //wait until all windows are created
        m_applicationControl->m_createWindowSemaphore.acquire(m_windows.size());
        m_applicationControl->m_pauseApplicationMutex.lock();
        m_pauseApplication=false;
        m_applicationControl->m_pauseApplicationMutex.unlock();
        qDebug()<<"pauseApplication done";
    }
    else
        m_applicationControl->m_pauseApplicationMutex.unlock();
}

namespace QtAndroid
{
    void flushImage(int windowId, const QPoint & pos, const QImage & image, const QRect & destinationRect)
    {
        checkPauseApplication();
        JNIEnv* env;
        QMutexLocker locker(&m_applicationControl->m_windowMutex);
        if (!m_windows.contains(windowId) || !m_windows[windowId].javaObject())
            return;

        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        int bpp=2;
        AndroidBitmapInfo  info;
        int ret;

        if ((ret = AndroidBitmap_getInfo(env, m_windows[windowId].javaObject(), &info)) < 0)
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

        if ((ret = AndroidBitmap_lockPixels(env, m_windows[windowId].javaObject(), &pixels)) < 0)
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

        jclass object=env->GetObjectClass(m_applicationObject());
        for (unsigned y=0;y<height;y++)
            memcpy(screenBits+y*sbpl+sposx*bpp,
                    imageBits+y*ibpl+iposx*bpp,
                   width*bpp);
        AndroidBitmap_unlockPixels(env, m_windows[windowId].javaObject());
        env->CallVoidMethod(object, m_redrawWindowMethodID, (jint)windowId,
                            (jint)destinationRect.left(),
                            (jint)destinationRect.top(),
                            (jint)destinationRect.right(),
                            (jint)destinationRect.bottom());
        m_javaVM->DetachCurrentThread();
    }

    void setAndroidGraphicsSystem(QAndroidPlatformIntegration * androidGraphicsSystem)
    {
        mAndroidGraphicsSystem=androidGraphicsSystem;
    }

    bool createWindow(bool openGl, QWidget * tlw, int windowId, int l, int t, int r, int b)
    {
        qDebug()<<"createWindow"<<openGl<<windowId<<l<<t<<r<<b;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }

        bool res=env->CallBooleanMethod(m_applicationObject(), m_createWindowMethodID,
                            (jboolean)openGl, (jint)windowId, (jint)l, (jint)t, (jint)r, (jint)b);

        m_javaVM->DetachCurrentThread();
        if (!res)
            return false;
        m_applicationControl->m_createWindowSemaphore.acquire(); //wait until window is created
        if (m_windows.contains(windowId))
        {
            m_windows[windowId].tlw=tlw;
            return true;
        }
        return false;
    }

    bool resizeWindow(int windowId, int l, int t, int r, int b)
    {
        qDebug()<<"resizeWindow"<<windowId<<l<<t<<r<<b;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }

        bool res=env->CallBooleanMethod(m_applicationObject(), m_resizeWindowMethodID,
                            (jint)windowId, (jint)l, (jint)t, (jint)r, (jint)b);

        m_javaVM->DetachCurrentThread();
        if (!res)
            return false;

        m_applicationControl->m_resizeWindowSemaphore.acquire(); //wait until window is resized
        return m_windows.contains(windowId);
    }

    bool destroyWindow(int windowId)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }

        bool res=env->CallBooleanMethod(m_applicationObject(), m_destroyWindowMethodID, (jint)windowId);

        m_javaVM->DetachCurrentThread();

        if (!res)
            return false;
        m_applicationControl->m_destroyWindowSemaphore.acquire(); //wait until window is destroyed
        return !m_windows.contains(windowId);
    }

    void setWindowVisiblity(int windowId, bool visible)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }

        env->CallVoidMethod(m_applicationObject(), m_setWindowVisiblityMethodID, (jint)windowId, (jboolean)visible);

        m_javaVM->DetachCurrentThread();
    }

    void setWindowOpacity(int windowId, double level)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jclass object=env->GetObjectClass(m_applicationObject());
        env->CallVoidMethod(object, m_setWindowOpacityMethodID, (jint)windowId, (jdouble)level);
        m_javaVM->DetachCurrentThread();
    }

    void setWindowTitle(int windowId, const QString & title)
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
        env->CallVoidMethod(m_applicationObject(), m_setWindowTitleMethodID, (jint)windowId, (jstring)jtitle);
        env->DeleteLocalRef(jtitle);
        m_javaVM->DetachCurrentThread();
    }

    void raiseWindow(int windowId)
    {
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        jclass object=env->GetObjectClass(m_applicationObject());
        env->CallVoidMethod(object, m_raiseWindowMethodID, (jint)windowId);
        m_javaVM->DetachCurrentThread();
    }

    bool makeCurrent(int windowId)
    {
        qDebug()<<"makeCurrent"<<windowId;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        jboolean res=env->CallBooleanMethod(m_EglObject(), m_makeCurrentMethodID, (jint)windowId);
        m_javaVM->DetachCurrentThread();
        return res;
    }

    bool doneCurrent()
    {
        qDebug()<<"doneCurrent";
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        env->CallVoidMethod(m_EglObject(), m_doneCurrentMethodID);
        m_javaVM->DetachCurrentThread();
        return true;
    }

    bool swapBuffers(int windowId)
    {
        qDebug()<<"swapBuffers"<<windowId;
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return false;
        }
        jboolean res=env->CallBooleanMethod(m_EglObject(), m_swapBuffersMethodID, (jint)windowId);
        m_javaVM->DetachCurrentThread();
        return res;
    }

    void * getProcAddress(int windowId, const QString& procName)
    {
        qDebug()<<"getProcAddress"<<windowId<<procName;
        Q_UNUSED(procName)
        Q_UNUSED(windowId)
//        JNIEnv* env;
//        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
//        {
//            qCritical()<<"AttachCurrentThread failed";
//            return false;
//        }
//        jstring jprocName = env->NewStringUTF(procName.toUtf8().data());
//        env->CallVoidMethod(m_EglObject, m_getProcAddressMethodID, (jint)windowId, jprocName);
//        env->DeleteLocalRef(jprocName);
//        m_javaVM->DetachCurrentThread();
        return NULL;
    }

    void showSoftwareKeyboard()
    {
        qDebug()<<"showSoftwareKeyboard";
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        env->CallVoidMethod(m_applicationObject(), m_showSoftwareKeyboardMethodID);
        m_javaVM->DetachCurrentThread();
    }

    void hideSoftwareKeyboard()
    {
        qDebug()<<"hideSoftwareKeyboard";
        JNIEnv* env;
        if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
        {
            qCritical()<<"AttachCurrentThread failed";
            return;
        }
        env->CallVoidMethod(m_applicationObject(), m_hideSoftwareKeyboardMethodID);
        m_javaVM->DetachCurrentThread();
    }

    void lockWindow()
    {
        m_applicationControl->m_windowMutex.lock();
    }

    void unlockWindow()
    {
        m_applicationControl->m_windowMutex.unlock();
    }

    bool hasOpenGL()
    {
        return (bool)m_EglObject();
    }

}

static void startQtAndroidPlugin(JNIEnv* env, jobject /*object*/)
{
    if (!m_applicationObject())
        setQtApplicationObject(env);
    m_windows.clear();
    mAndroidGraphicsSystem=0;
    m_pauseApplication=false;
    m_applicationControl = new ApplicationControl();
    qDebug()<<"startQtAndroidPlugin";
}

static void pauseQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    qDebug()<<"pauseQtApp";
    m_applicationControl->m_windowMutex.lock();
    m_applicationControl->m_pauseApplicationMutex.lock();
    if (mAndroidGraphicsSystem)
        mAndroidGraphicsSystem->pauseApp();
    m_pauseApplication=true;
    foreach(TLWStruct tlw, m_windows)
        tlw.javaObject=0;
    m_applicationControl->m_pauseApplicationMutex.unlock();
    m_applicationControl->m_windowMutex.unlock();
}

static void resumeQtApp(JNIEnv */*env*/, jobject /*thiz*/)
{
    qDebug()<<"resumeQtApp";
    m_applicationControl->m_windowMutex.lock();
    m_applicationControl->m_pauseApplicationMutex.lock();
    if (mAndroidGraphicsSystem)
        mAndroidGraphicsSystem->resumeApp();

    if (m_pauseApplication)
        m_applicationControl->m_pauseApplicationSemaphore.release();

    m_applicationControl->m_pauseApplicationMutex.unlock();
    m_applicationControl->m_windowMutex.unlock();
}

static void quitQtAndroidPlugin(JNIEnv* /*env*/, jclass /*clazz*/)
{
    // clean all java global objects
    m_EglObject = 0;
    m_applicationObject = 0;
    m_windows.clear();
    delete m_applicationControl;
    m_applicationControl=0;
    qDebug()<<"quitQtAndroidPlugin";
}

static void setDisplayMetrics(JNIEnv* /*env*/, jclass /*clazz*/,
                              jint widthPixels, jint heightPixels,
                              jint desktopWidthPixels, jint desktopHeightPixels,
                              jfloat xdpi, jfloat ydpi)
{
    qDebug()<<"setDisplayMetrics";
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
        QWindowSystemInterface::handleScreenAvailableGeometryChange(0);
        QWindowSystemInterface::handleScreenGeometryChange(0);
    }
    qDebug()<<"setDisplayMetrics done";
}


static void mouseDown(JNIEnv */*env*/, jobject /*thiz*/, jint winId, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(m_windows.contains(winId)?m_windows[winId].tlw:0,
                                             QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                             Qt::MouseButtons(Qt::LeftButton));
}

static void mouseUp(JNIEnv */*env*/, jobject /*thiz*/, jint winId, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(m_windows.contains(winId)?m_windows[winId].tlw:0,
                                             QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                             Qt::MouseButtons(Qt::NoButton));
}

static void mouseMove(JNIEnv */*env*/, jobject /*thiz*/, jint winId, jint x, jint y)
{
    QWindowSystemInterface::handleMouseEvent(m_windows.contains(winId)?m_windows[winId].tlw:0,
                                             QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                             Qt::MouseButtons(Qt::LeftButton));
}

static void touchBegin(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/)
{
    m_touchPoints.clear();
}

static void touchAdd(JNIEnv */*env*/, jobject /*thiz*/, jint /*winId*/, jint id, jint action, jboolean primary, jint x, jint y, jfloat size, jfloat pressure)
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
    touchPoint.area=QRectF(x-((double)m_desktopWidthPixels*size)/2,
                           y-((double)m_desktopHeightPixels*size)/2,
                           (double)m_desktopWidthPixels*size,
                           (double)m_desktopHeightPixels*size);
    m_touchPoints.push_back(touchPoint);
}

static void touchEnd(JNIEnv */*env*/, jobject /*thiz*/, jint winId, jint action)
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
    QWindowSystemInterface::handleTouchEvent(m_windows.contains(winId)?m_windows[winId].tlw:0, eventType, QTouchEvent::TouchScreen, m_touchPoints);
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
    QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyPress, mapAndroidKey(key), modifiers, QChar(unicode),true);
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

    QWindowSystemInterface::handleKeyEvent(0, QEvent::KeyRelease, mapAndroidKey(key), modifiers, QChar(unicode),true);
}

static void windowCreated(JNIEnv *env, jobject /*thiz*/, jobject jWindow, jint windowId)
{
    qDebug()<<"windowCreated"<<windowId;
    Q_UNUSED(env);
    m_windows[windowId].javaObject = jWindow;
    m_applicationControl->m_createWindowSemaphore.release();
}

static void windowChanged(JNIEnv *env, jobject /*thiz*/, jobject jWindow, jint windowId)
{
    qDebug()<<"windowChanged"<<windowId;
    Q_UNUSED(env);
    m_windows[windowId].javaObject = jWindow;
    m_applicationControl->m_resizeWindowSemaphore.release();
}

static void windowDestroyed(JNIEnv */*env*/, jobject /*thiz*/, jint windowId)
{
    qDebug()<<"windowDestroyed";
    m_applicationControl->m_pauseApplicationMutex.lock();
    if (!m_pauseApplication)
    {
        qDebug()<<"windowDestroyed"<<windowId;
        m_windows.remove(windowId);
        m_applicationControl->m_destroyWindowSemaphore.release();
    }
    m_applicationControl->m_pauseApplicationMutex.unlock();
}

static void setEglObject(JNIEnv *env, jobject /*thiz*/,  jobject eglObject)
{
    jclass clazz = env->FindClass(QtEglClassPathName);
    if (clazz == NULL)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Unable to find class '%s'", QtEglClassPathName);
        return;
    }

    m_EglObject=eglObject;
    m_makeCurrentMethodID = env->GetMethodID((jclass)clazz, "makeCurrent", "(I)Z");
    m_doneCurrentMethodID = env->GetMethodID((jclass)clazz, "doneCurrent", "()V");
    m_swapBuffersMethodID = env->GetMethodID((jclass)clazz, "swapBuffers", "(I)Z");
//    m_getProcAddressMethodID = env->GetMethodID((jclass)m_EglObject, "getProcAddress", "(I)Z");
}

static void lockWindow(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_applicationControl->m_windowMutex.lock();
}

static void unlockWindow(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_applicationControl->m_windowMutex.unlock();
}

static JNINativeMethod methods[] = {
    {"startQtAndroidPlugin", "()V", (void *)startQtAndroidPlugin},
    {"pauseQtApp", "()V", (void *)pauseQtApp},
    {"resumeQtApp", "()V", (void *)resumeQtApp},
    {"quitQtAndroidPlugin", "()V", (void *)quitQtAndroidPlugin},
    {"setEglObject", "(Ljava/lang/Object;)V", (void *)setEglObject},
    {"setDisplayMetrics", "(IIIIFF)V", (void *)setDisplayMetrics},
    {"windowCreated", "(Ljava/lang/Object;I)V", (void *)windowCreated},
    {"windowChanged", "(Ljava/lang/Object;I)V", (void *)windowChanged},
    {"windowDestroyed", "(I)V", (void *)windowDestroyed},
    {"lockWindow", "()V", (void *)lockWindow},
    {"unlockWindow", "()V", (void *)unlockWindow},
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
    if (!setQtApplicationObject(env))
        return JNI_FALSE;

    if (env->RegisterNatives((jclass)m_applicationObject(), gMethods, numMethods) < 0)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    m_createWindowMethodID = env->GetMethodID((jclass)m_applicationObject(), "createWindow", "(ZIIIII)Z");
    m_resizeWindowMethodID = env->GetMethodID((jclass)m_applicationObject(), "resizeWindow", "(IIIII)Z");
    m_destroyWindowMethodID = env->GetMethodID((jclass)m_applicationObject(), "destroyWindow", "(I)Z");
    m_setWindowVisiblityMethodID = env->GetMethodID((jclass)m_applicationObject(), "setWindowVisiblity", "(IZ)V");
    m_setWindowOpacityMethodID = env->GetMethodID((jclass)m_applicationObject(), "setWindowOpacity", "(ID)V");
    m_setWindowTitleMethodID = env->GetMethodID((jclass)m_applicationObject(), "setWindowTitle", "(ILjava/lang/String;)V");
    m_raiseWindowMethodID = env->GetMethodID((jclass)m_applicationObject(), "raiseWindow", "(I)V");
    m_redrawWindowMethodID = env->GetMethodID((jclass)m_applicationObject(), "redrawWindow", "(IIIII)V");
    m_showSoftwareKeyboardMethodID = env->GetMethodID((jclass)m_applicationObject(), "showSoftwareKeyboard", "()V");
    m_hideSoftwareKeyboardMethodID = env->GetMethodID((jclass)m_applicationObject(), "hideSoftwareKeyboard", "()V");
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
    __android_log_print(ANDROID_LOG_INFO,"Qt", "qt android plugin start");
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
