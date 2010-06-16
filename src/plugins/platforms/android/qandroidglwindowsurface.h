#ifndef QANDROIDGLWINDOWSURFACE_H
#define QANDROIDGLWINDOWSURFACE_H

#include <private/qwindowsurface_gl_p.h>

class QAndroidGLWindowSurface : public QGLWindowSurface
{
public:
    QAndroidGLWindowSurface(QWidget *window);
    virtual bool create(QGLWidget* widget, QGLFormat& format);
    virtual void setGeometry(const QRect& rect);
    virtual void beginPaint(const QRegion &region);
    virtual void endPaint(const QRegion &region);

private:
    WId     m_surfaceId;
    bool m_locked;
};

#endif // QANDROIDGLWINDOWSURFACE_H
