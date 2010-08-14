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
    QPlatformWindowFormat m_windowFormat;
    WId     m_surfaceId;
};

#endif // QANDROIDCONTEXT_H
