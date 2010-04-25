#ifndef QANDROIDCONTEXT_H
#define QANDROIDCONTEXT_H

#include <QPlatformGLContext>

class QAndroidContext : public QPlatformGLContext
{
public:
    QAndroidContext();
    ~QAndroidContext();
    virtual bool create(QPaintDevice* device, const QGLFormat& format, QPlatformGLContext* shareContext);

    virtual void makeCurrent();
    virtual void doneCurrent();
    virtual void swapBuffers();
    virtual void* getProcAddress(const QString& procName);
};

#endif // QANDROIDCONTEXT_H
