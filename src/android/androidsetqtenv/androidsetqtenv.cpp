#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    #define QT_PATH "/data/local/qt"
    #define QT_LIB_PATH QT_PATH"/lib"
    #define QT_PLUGIN_PATH QT_PATH"/plugins"
    
    char qtLibsPath[2048];
    if (getenv("LD_LIBRARY_PATH"))
        snprintf(qtLibsPath, 1024, "%s:%s", getenv("LD_LIBRARY_PATH"), QT_LIB_PATH);
    else
        snprintf(qtLibsPath, 1024, "%s", QT_LIB_PATH);
    
    if (setenv("LD_LIBRARY_PATH",qtLibsPath,1))
	__android_log_print(ANDROID_LOG_FATAL,"Qt", "Can't set 'LD_LIBRARY_PATH' enviroument variable");
    else
	__android_log_print(ANDROID_LOG_INFO,"Qt", "LD_LIBRARY_PATH=%s",getenv("LD_LIBRARY_PATH"));

    if (setenv("QT_PLUGIN_PATH",QT_PLUGIN_PATH,1))
	__android_log_print(ANDROID_LOG_FATAL,"Qt", "Can't set 'QT_PLUGIN_PATH' enviroument variable");
    else
	__android_log_print(ANDROID_LOG_INFO,"Qt", "QT_PLUGIN_PATH=%s",getenv("QT_PLUGIN_PATH"));
   
    return JNI_VERSION_1_4;
}
