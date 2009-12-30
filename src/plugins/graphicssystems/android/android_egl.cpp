#include "android_app.h"
#include "EGL/egl.h"

EGLAPI EGLint EGLAPIENTRY eglGetError(void)
{
#warning "PLEASE IMPLEMENT"
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay dpy)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI const char * EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig *configs,
             EGLint config_size, EGLint *num_config)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
               EGLConfig *configs, EGLint config_size,
               EGLint *num_config)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
                  EGLint attribute, EGLint *value)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                  EGLNativeWindowType win,
                  const EGLint *attrib_list)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config,
                   const EGLint *attrib_list)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config,
                  EGLNativePixmapType pixmap,
                  const EGLint *attrib_list)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface,
               EGLint attribute, EGLint *value)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}


EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLenum EGLAPIENTRY eglQueryAPI(void)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}


EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient(void)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}


EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}


EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer(
          EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,
          EGLConfig config, const EGLint *attrib_list)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}


EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface,
                EGLint attribute, EGLint value)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}



EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}



EGLAPI EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config,
                EGLContext share_context,
                const EGLint *attrib_list)
{
#warning "PLEASE IMPLEMENT"
    return 0;
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
              EGLSurface read, EGLContext ctx)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}


EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx,
               EGLint attribute, EGLint *value)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}


EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL(void)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLBoolean eglWaitNative(EGLint engine)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers(EGLDisplay dpy, EGLSurface surface,
              EGLNativePixmapType target)
{
#warning "PLEASE IMPLEMENT"
    return EGL_FALSE;
}

