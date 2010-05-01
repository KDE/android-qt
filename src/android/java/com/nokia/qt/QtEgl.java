package com.nokia.qt;

import java.util.HashMap;
import java.util.Map;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

import android.view.SurfaceHolder;

public class QtEgl
{
    EGL10 mEgl=null;
    EGLDisplay mEglDisplay;
    EGLConfig mEglConfig;
    EGLContext mEglContext;
    EGLSurface mCurrentEglSurface;
    Map<Integer, EGLSurface> mEglSurfaces= new HashMap<Integer, EGLSurface>();

    public boolean initialize(boolean useAA)
    {
        mEgl = (EGL10) EGLContext.getEGL();
        mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        int[] version = new int[2];
        mEgl.eglInitialize(mEglDisplay, version);
        int[] configSpec;
        if (useAA)
        {
            configSpec = new int[]{
                    EGL10.EGL_RED_SIZE, 		5,
                    EGL10.EGL_GREEN_SIZE, 		6,
                    EGL10.EGL_BLUE_SIZE, 		5,
                    EGL10.EGL_DEPTH_SIZE,  	   16,
                    EGL10.EGL_STENCIL_SIZE, 	4,
                    EGL10.EGL_SAMPLES, 	    	4,
                    EGL10.EGL_SAMPLE_BUFFERS,	1,
                    EGL10.EGL_NONE };
        }
        else
        {
            configSpec = new int[]{
                    EGL10.EGL_RED_SIZE, 		5,
                    EGL10.EGL_GREEN_SIZE, 		6,
                    EGL10.EGL_BLUE_SIZE, 		5,
                    EGL10.EGL_DEPTH_SIZE,  	   16,
                    EGL10.EGL_STENCIL_SIZE, 	4,
                    EGL10.EGL_NONE };
        }

        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfig = new int[1];
        mEgl.eglChooseConfig(mEglDisplay, configSpec, configs, 1, 
              numConfig);
        mEglConfig = configs[0];
        mEglContext = mEgl.eglCreateContext(mEglDisplay, mEglConfig, EGL10.EGL_NO_CONTEXT, null);
        if (mEglContext == EGL10.EGL_NO_CONTEXT) {
            throw new RuntimeException("CreateContext failed");
        }
        mCurrentEglSurface = EGL10.EGL_NO_SURFACE;
		return true;
    }

    public boolean isInitialized()
    {
    	return mEgl!=null;
    }

    public void finish()
    {
        if (mEglContext != EGL10.EGL_NO_CONTEXT) {
            mEgl.eglDestroyContext(mEglDisplay, mEglContext);
            mEglContext = EGL10.EGL_NO_CONTEXT;
        }
        if (mEglDisplay != EGL10.EGL_NO_DISPLAY) {
            mEgl.eglTerminate(mEglDisplay);
            mEglDisplay = EGL10.EGL_NO_DISPLAY;
        }
        mEgl=null;
    }

    public EGLSurface createSurface(SurfaceHolder holder, int surfaceId)
    {
    	synchronized(this)
    	{
	    	if (mEgl == null)
	    		return EGL10.EGL_NO_SURFACE;
	
	    	mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
	                EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
	
	        if (mEglSurfaces.containsKey(surfaceId) && mEglSurfaces.get(surfaceId) != EGL10.EGL_NO_SURFACE)
	        {
	            mEgl.eglDestroySurface(mEglDisplay, mEglSurfaces.get(surfaceId));
	        }
	
	    	EGLSurface eglSurface = mEgl.eglCreateWindowSurface(mEglDisplay, mEglConfig, holder, null);
	
	        if (eglSurface == EGL10.EGL_NO_SURFACE) {
	            throw new RuntimeException("createWindowSurface failed");
	        }
	
	        if (!mEgl.eglMakeCurrent(mEglDisplay, eglSurface, eglSurface, mEglContext)) {
	            throw new RuntimeException("eglMakeCurrent failed.");
	        }
	        mEglSurfaces.put(surfaceId, eglSurface);
	        return eglSurface;
    	}
    }

    public boolean makeCurrent(int surfaceId)
    {
    	synchronized(this)
    	{
			try
			{
				if (mEgl == null)
					return false;
		
				if (mEglSurfaces.containsKey(surfaceId) && mCurrentEglSurface!=mEglSurfaces.get(surfaceId))
				{
					if( mEgl.eglMakeCurrent(mEglDisplay, mEglSurfaces.get(surfaceId), mEglSurfaces.get(surfaceId), mEglContext) )
						mCurrentEglSurface = mEglSurfaces.get(surfaceId);
					else
					{
						mCurrentEglSurface = EGL10.EGL_NO_SURFACE;
						return false;
					}
				}
				return true;
			}
			catch (Exception e)
			{
				return false;
			}
    	}
    }

    public void doneCurrent()
    {
    	synchronized(this)
    	{
	    	mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
	            EGL10.EGL_NO_SURFACE,
	            EGL10.EGL_NO_CONTEXT);
	    	mCurrentEglSurface=EGL10.EGL_NO_SURFACE;
    	}
    }

    public boolean swapBuffers(int surfaceId)
    {
    	synchronized(this)
    	{
	    	if (mEgl==null || !mEglSurfaces.containsKey(surfaceId) || mEglSurfaces.get(surfaceId)==EGL10.EGL_NO_SURFACE)
	    		return false;
	        boolean ret = mEgl.eglSwapBuffers(mEglDisplay, mEglSurfaces.get(surfaceId));
	        if (!ret)
	        	mCurrentEglSurface=EGL10.EGL_NO_SURFACE;
	        return ret;
    	}
    }

    public void destroySurface(int surfaceId)
    {
    	synchronized(this)
    	{
	    	if (mEgl==null || !mEglSurfaces.containsKey(surfaceId) || mEglSurfaces.get(surfaceId) == EGL10.EGL_NO_SURFACE)
	    		return;
	    	
	        mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
	                EGL10.EGL_NO_SURFACE,
	                EGL10.EGL_NO_CONTEXT);
	        
	        mEgl.eglDestroySurface(mEglDisplay, mEglSurfaces.get(surfaceId));
	        if (mCurrentEglSurface==mEglSurfaces.get(surfaceId))
	        	mCurrentEglSurface = null;
	        mEglSurfaces.remove(surfaceId);
    	}
    }
}
