package com.nokia.qt;

import java.io.File;
import java.nio.ShortBuffer;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.Log;

public class QtApplication {

	public static final String QtTAG = "Qt JAVA";
	public static Activity m_activity = null;
	public static QtSurface m_surface = null;
//	static
//	{
//	//	System.load("/data/local/qt/lib/libAndroidSetQtEnv.so");
//	}
//	
	public static void loadLibraries(String[] libraries)
	{
		for (int i=0;i<libraries.length;i++)
		{
			try{
				String library="/data/local/qt/lib/lib"+libraries[i]+".so";
				File f = new File(library);
				if (f.exists())
					System.load(library);
			}
			catch (Exception e){
				Log.i(QtTAG, "Can't load '/data/local/qt/lib/lib"+libraries[i]+".so'", e);
			}
		}
	}
	
	public static void loadApplication(String lib)
	{
		try{
			 File f = new File("/data/local/lib/lib"+lib+".so");
			 if (f.exists())
				 System.load("/data/local/lib/lib"+lib+".so");
			 else
				 System.loadLibrary(lib);
			 Log.i(QtTAG, "Try to start QtApp");
			 startQtApp();
			 Log.i(QtTAG, "QtApp Started");
		}
		catch (Exception e){
			Log.i(QtTAG, "Can't load 'lib"+lib+".so'", e);
		}
	}

	@SuppressWarnings("unused")
	private void flushImage(short[] img, final int left, final int top, final int right, final int bottom)
	{
		if (m_activity == null || m_surface == null)
			return;

		ShortBuffer image=ShortBuffer.wrap(img);
		final Bitmap bmp=Bitmap.createBitmap(right-left+1, bottom-top+1, Bitmap.Config.RGB_565);
		bmp.copyPixelsFromBuffer(image);
		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
					Rect rect = new Rect(left, top, right, bottom);
					Canvas cv=m_surface.getHolder().lockCanvas(rect);
					cv.drawBitmap(bmp, new Rect(0, 0, right-left+1, bottom-top+1), rect, null);
					m_surface.getHolder().unlockCanvasAndPost(cv);			
				}
		});
	}

	@SuppressWarnings("unused")
	private void redrawSurface(final int left, final int top, final int right, final int bottom )
	{
		if (m_activity == null || m_surface == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_surface.drawBitmap(new Rect(left, top, right+1, bottom+1));
			}
		});
	}
	// application methods
	public static native void startQtApp();
	public static native void pauseQtApp();
	public static native void resumeQtApp();
	public static native void quitQtApp();
	// application methods
	
	// screen methods
	public static native void setDisplayMetrics(int screenWidthPixels,
			int screenHeightPixels, int desktopWidthPixels,
			int desktopHeightPixels, float xdpi, float ydpi);
	// screen methods
	
	// input methods
	public static native void mouseDown(int x, int y);
	public static native void mouseUp(int x, int y);
	public static native void mouseMove(int x, int y);
	public static native void touchBegin();
	public static native void touchAdd(int pointerId, int action, boolean primary, int x, int y, float size, float pressure);
	public static native void touchEnd(int action);
	public static native void keyDown(int key, int unicode, int modifier);
	public static native void keyUp(int key, int unicode, int modifier);
	// input methods

	// surface methods
	public static native void setSurface(Object surface);
	public static native void destroySurface();
	public static native void lockSurface();
	public static native void unlockSurface();
	// surface methods
}
