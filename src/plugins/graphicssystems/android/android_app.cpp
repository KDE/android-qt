#include "android_app.h"
#include <android/log.h>
#include <pthread.h>
#include <qcoreapplication.h>

static JavaVM *m_javaVM = NULL;
static jobject m_object  = NULL;
static jmethodID m_createWindowMethodID;

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
}


static void * startMainMethod(void * /*data*/)
{
    extern int main(int, char **); //use the standard main methos to start the application

    const char params[][50]={"qtApp","-graphicssystem=android"}; // default use raster as default graphics system

    main(2, (char**)params);
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

static const char *classPathName = "com/nokia/qt/Native";

static JNINativeMethod methods[] = {
    {"startQtApp", "()Z", (void *)startQtApp},
    {"quitQtApp", "()V", (void *)quitQtApp},
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

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0])))
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

