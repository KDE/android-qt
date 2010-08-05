#ifndef QANDROIDCONTEXT_H
#define QANDROIDCONTEXT_H

#include <QPlatformGLContext>

class QAndroidPlatformGLContext : public QPlatformGLContext
{
public:
    virtual bool create(QPaintDevice* device, QGLFormat& format, QPlatformGLContext* shareContext);

    virtual void makeCurrent();
    virtual void doneCurrent();
    virtual void swapBuffers();
    virtual void* getProcAddress(const QString& procName);
    virtual QPlatformWindowFormat platformWindowFormat() const;
private:
    WId     m_surfaceId;
    QPlatformWindowFormat m_platformWindowFormat;
};

#endif // QANDROIDCONTEXT_H
