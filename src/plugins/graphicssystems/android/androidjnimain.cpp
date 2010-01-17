#include <android/log.h>
#include <pthread.h>
#include <qcoreapplication.h>
#include <qimage.h>
#include <qplugin.h>
#include "androidjnimain.h"

Q_IMPORT_PLUGIN (QtAndroid)

#ifdef QT_USE_CUSTOM_NDK
    #include <ui/Surface.h>
    #include <qmap.h>
#endif

static JavaVM *m_javaVM = NULL;
static jobject m_object  = NULL;
static jmethodID m_createWindowMethodID=0;
static jmethodID m_destroyWindowMethodID=0;
static jmethodID m_flushImageMethodID=0;
static jmethodID m_setWindowGeomatryMethodID=0;
#ifdef QT_USE_CUSTOM_NDK
static jfieldID  IDsurface;
static QMap<long, android::Surface* > m_surfaces;
#endif
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

    long createWindow()
    {
        JNIEnv* env;
        m_javaVM->AttachCurrentThread(&env, NULL);
        long ret=env->CallLongMethod(m_object, m_createWindowMethodID);
        m_javaVM->DetachCurrentThread();
        return ret;
    }

    void destroyWindow(long winId)
    {
        JNIEnv* env;
        m_javaVM->AttachCurrentThread(&env, NULL);
        env->CallVoidMethod(m_object, m_destroyWindowMethodID, (jlong)winId);
        m_javaVM->DetachCurrentThread();
    }

    void flushImage(long winId, const QImage & image, const QRect & destinationRect)
    {
#ifdef QT_USE_CUSTOM_NDK
        android::Surface *surface=m_surfaces.value(winId);
        if (!surface)
            return;

        android::Surface::SurfaceInfo info;
        android::status_t err = surface->lock(&info);
        if(err < 0)
        {
            __android_log_print(ANDROID_LOG_ERROR,"Qt", "Unable to lock the surface");
            return;
        }
        if (info.w != (unsigned)image.size().width() || info.h != (unsigned)image.size().height())
        {
            surface->unlockAndPost();
            __android_log_print(ANDROID_LOG_ERROR,"Qt", "Screen surface size != internal buffer size");
            return;
        }

        unsigned char * screenBits=(unsigned char*)info.bits;
        int bpl=image.bytesPerLine();
        int bpp=2;

        for (int y=0;y<destinationRect.height();y++)
        {
            memcpy(screenBits+info.s*(y+destinationRect.y())+destinationRect.x()*bpp,
                   ((const uchar*)image.bits())+bpl*(y+destinationRect.y())+destinationRect.x()*bpp,
                   destinationRect.x()*bpp);
        }
        surface->unlockAndPost();
#else
        JNIEnv* env;
        m_javaVM->AttachCurrentThread(&env, NULL);
        QImage imag=image.copy(destinationRect);
        jshortArray img=env->NewShortArray(image.byteCount()/2);
        env->SetShortArrayRegion(img,0,imag.byteCount(), (const jshort*)(const uchar *)imag.bits());
        env->CallVoidMethod(m_object, m_flushImageMethodID, (jlong)winId,
                            (jint)img, (jint)image.bytesPerLine(),
                            (jint)destinationRect.x(), (jint)destinationRect.y(),
                            (jint)destinationRect.right(), (jint)destinationRect.bottom());
        env->DeleteLocalRef(img);
        m_javaVM->DetachCurrentThread();
#endif
    }

    void setWindowGeometry(long winId, const QRect &rect)
    {
        JNIEnv* env;
        m_javaVM->AttachCurrentThread(&env, NULL);
        env->CallVoidMethod(m_object, m_setWindowGeomatryMethodID, (jlong)winId,
                            (jint)rect.x(), (jint)rect.y(),
                            (jint)rect.right(), (jint)rect.bottom());
        m_javaVM->DetachCurrentThread();
   }

}

extern "C" int main(int, char **); //use the standard main method to start the application
static void * startMainMethod(void * /*data*/)
{
    char ** params;//[2][50]={"qtApp","-graphicssystem=android"}; // default use raster as default graphics system
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
}

#ifdef QT_USE_CUSTOM_NDK
static void setWindowSurface(JNIEnv *env, jobject /*thiz*/, jlong winId, jobject jSurface)
{
    m_surfaces[winId]=reinterpret_cast<android::Surface*>(env->GetIntField(jSurface, IDsurface));;
}
#endif

static const char *classPathName = "com/nokia/qt/QtApplication";

static JNINativeMethod methods[] = {
    {"startQtApp", "()V", (void *)startQtApp},
    {"quitQtApp", "()V", (void *)quitQtApp},
#ifdef QT_USE_CUSTOM_NDK
    {"setWindowSurface", "(JLandroid/view/Surface;)V", (void *)setWindowSurface},
#endif
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

    m_createWindowMethodID = env->GetMethodID(clazz, "createWindow", "()J");
    m_destroyWindowMethodID = env->GetMethodID((jclass)m_object, "destroyWindow", "(J)V");
    //m_flushImageMethodID = env->GetMethodID((jclass)m_object, "flushImage", "(JSIIIII)V");
    m_setWindowGeomatryMethodID = env->GetMethodID((jclass)m_object, "setWindowGeometry", "(JIIII)V");

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
