#include <android/log.h>
#include <pthread.h>
#include <QSemaphore>
#include <QDebug>
#include <qglobal.h>

#include <stdlib.h>

#include <jni.h>

static JavaVM *m_javaVM = NULL;
static JNIEnv *m_env = NULL;
static jobject objptr;
static QSemaphore m_quitAppSemaphore;
static const char *QtApplicationClassPathName = "com/nokia/qt/android/QtApplication";


extern "C" int main(int, char **); //use the standard main method to start the application
static void * startMainMethod(void * /*data*/)
{
    qDebug()<<"startMainMethod";
    char ** params;
    params=(char**)malloc(sizeof(char*)*3);
    params[0]=(char*)malloc(20);
    strcpy(params[0],"QtApp");
    params[1]=(char*)malloc(20);
    strcpy(params[1],"-platform");
    params[2]=(char*)malloc(20);
    strcpy(params[2],"android");
    int ret = main(3, params);    
    qDebug()<<"MainMethod finished, it's time to cleanup";
    
    free(params[2]);
    free(params[1]);
    free(params[0]);
    free(params);
    Q_UNUSED(ret);
    
    JNIEnv* env;
    if (m_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    jclass applicationClass = env->GetObjectClass(objptr);
    if (applicationClass){
        jmethodID quitApp = env->GetStaticMethodID(applicationClass, "quitApp", "()V");
	env->CallStaticVoidMethod(applicationClass, quitApp);
    }
    m_javaVM->DetachCurrentThread();
    return NULL;
}

static jboolean startQtApp(JNIEnv* /*env*/, jobject /*object*/, jobject /*jniProxyObject*/)
{
    qDebug()<<"startQtApp";
    pthread_t appThread;
    return pthread_create(&appThread, NULL, startMainMethod, NULL)==0;
}


static JNINativeMethod methods[] = {
    {"startQtApp", "(Ljava/lang/Object;)V", (void *)startQtApp}
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
    jmethodID constr = env->GetMethodID(clazz, "<init>", "()V");
    if(!constr) {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "Native registration unable to find  constructor for class '%s'", className);
        return JNI_FALSE;;
    }
    jobject obj = env->NewObject(clazz, constr);
    objptr = env->NewGlobalRef(obj);
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
    {
        __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
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
    m_env = uenv.nativeEnvironment;
    if (!registerNatives(m_env))
    {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "registerNatives failed");
        return -1;
    }
    m_javaVM = vm;
    return JNI_VERSION_1_4;
}
