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
#include "qandroidinput.h"

Q_IMPORT_PLUGIN (QtAndroid)

#ifdef QT_USE_CUSTOM_NDK
    #include <ui/Surface.h>
    #include <qmap.h>
#endif

static JavaVM *m_javaVM = NULL;
static jobject m_object  = NULL;
static jmethodID m_flushImageMethodID=0;

static QSemaphore m_quitAppSemaphore;
static QSemaphore m_windowSemaphore;
static QMutex m_windowMutex;

static jfieldID  IDsurface;
android::Surface* m_surface;


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

    void flushImage(const QPoint & pos, const QImage & image, const QRect & /*destinationRect*/)
    {
#ifdef QT_USE_CUSTOM_NDK
        if (!m_surface)
            return;

        android::Surface::SurfaceInfo info;
        android::status_t err = m_surface->lock(&info);
        if(err < 0)
        {
            qWarning()<<"Unable to lock the surface";
            return;
        }
        if ((unsigned)pos.x()>=info.w|| (unsigned)pos.y()>=info.h)
        {
            qWarning()<<"Invalid coordonates";
            m_surface->unlockAndPost();
            return;
        }

        int bpp=2;

        if (pos.x()==0 && pos.y()==0 && info.w==image.size().width() && info.h==image.size().height()
            && info.s*bpp==image.bytesPerLine())
        {
            memcpy(info.bits, (const uchar*)image.bits(), info.s*2*info.h);
            m_surface->unlockAndPost();
            return;
        }

        int sbpl=info.s*bpp;
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

#if 0
        int bpp=2;
        int sbpl=info.s*bpp;
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

//        qDebug()<<ibpl<<sbpl<<sxpos<<sypos<<width<<height<<sxpos+width<<sypos+height;

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

}

extern "C" int main(int, char **); //use the standard main method to start the application
static void * startMainMethod(void * /*data*/)
{
    char ** params;
    params=(char**)malloc(sizeof(char*)*2);
    params[0]=(char*)malloc(20);
    strcpy(params[0],"QtApp");
    params[1]=(char*)malloc(20);
    strcpy(params[1],"-graphicssystem");
    params[2]=(char*)malloc(20);
    strcpy(params[2],"android");
    int ret = main(3, params);
    free(params[2]);
    free(params[1]);
    free(params[0]);
    free(params);
    Q_UNUSED(ret);
    pthread_exit(NULL);
//    m_quitAppSemaphore.release();
    qDebug()<<"Thread application exited";
    return NULL;
}

static jboolean startQtApp(JNIEnv* /*env*/, jobject /*object*/)
{
    pthread_t appThread;
    return pthread_create(&appThread, NULL, startMainMethod, NULL)==0;
}

static void quitQtApp(JNIEnv* /*env*/, jclass /*clazz*/)
{
    qApp->quit();
//    m_quitAppSemaphore.acquire();
    qDebug()<<"Application closed";
}

static void setSurface(JNIEnv *env, jobject /*thiz*/, jobject jSurface)
{
    m_surface = reinterpret_cast<android::Surface*>(env->GetIntField(jSurface, IDsurface));
    qDebug()<<"Set surface"<<m_surface;
    if (!m_surface)
        return;

    android::Surface::SurfaceInfo info;
    android::status_t err = m_surface->lock(&info);
    if(err < 0)
    {
        qWarning()<<"Unable to lock the surface";
        return;
    }
    qDebug()<<info.w<<info.h;
    m_surface->unlockAndPost();
}

static void destroySurface(JNIEnv */*env*/, jobject /*thiz*/)
{
    m_surface = 0;
}

static void actionDown(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    if (!QAndroidInput::androidInput())
        return;
    QAndroidInput::androidInput()->addMouseEvent(new QMouseEvent(QEvent::MouseButtonPress,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButton(Qt::LeftButton),
                                                             Qt::MouseButtons(Qt::LeftButton),
                                                             Qt::NoModifier));
}

static void actionUp(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    if (!QAndroidInput::androidInput())
        return;
    QAndroidInput::androidInput()->addMouseEvent(new QMouseEvent(QEvent::MouseButtonRelease,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButton(Qt::LeftButton),
                                                             Qt::MouseButtons(Qt::LeftButton),
                                                             Qt::NoModifier));
}

static void actionMove(JNIEnv */*env*/, jobject /*thiz*/, jint x, jint y)
{
    if (!QAndroidInput::androidInput())
        return;
    QAndroidInput::androidInput()->addMouseEvent(new QMouseEvent(QEvent::MouseMove,QPoint(x,y),QPoint(x,y),
                                                             Qt::MouseButton(Qt::LeftButton),
                                                             Qt::MouseButtons(Qt::LeftButton),
                                                             Qt::NoModifier));
}

static const char *classPathName = "com/nokia/qt/QtApplication";

static JNINativeMethod methods[] = {
    {"startQtApp", "()V", (void *)startQtApp},
    {"quitQtApp", "()V", (void *)quitQtApp},
    {"setSurface", "(Landroid/view/Surface;)V", (void *)setSurface},
    {"destroySurface", "()V", (void *)destroySurface},
    {"actionDown", "(II)V", (void *)actionDown},
    {"actionUp", "(II)V", (void *)actionUp},
    {"actionMove", "(II)V", (void *)actionMove}
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
