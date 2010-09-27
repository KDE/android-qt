#include "qtandroidbridge.h"
#include "qtandroidbridge_p.h"

namespace QtAndroidBridge
{
    static JavaVM *m_javaVM = 0;
    static jobject m_jniProxyObject = 0;
    
    JavaVM *javaVM()
    {
        return m_javaVM;
    }
    
    jobject jniProxyObject()
    {
        return m_jniProxyObject;
    }


    void setJavaVM(JavaVM *value)
    {
        m_javaVM = value;
    }

    void setJniProxyObject(jobject value)
    {
        m_jniProxyObject = value;
    }
}
