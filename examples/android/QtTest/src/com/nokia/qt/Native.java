package com.nokia.qt;

import java.io.File;
import java.nio.ShortBuffer;
import java.util.HashMap;

import android.app.Application;
import android.util.Log;

class Native {
	private static final String TAG = "Java Gps";
	private static final String LIB = "QtTest";
	
	static
	{
		//Runtime.getRuntime().;
		 File f = new File("/data/local/lib"+LIB+".so");
		 if (f.exists())
			 System.load("/data/local/lib"+LIB+".so");
		 else
			 System.loadLibrary(LIB);
		Log.i(TAG,"loadLibrary");
	}
	
	private static 	Application main=null;
	private static HashMap<Long, QtActivity> windows = new HashMap<Long, QtActivity>();
	
	public static void setApplication(Application main) {
		Native.main = main;
	}


	@SuppressWarnings("unused")
	private long createWindow()
	{
		QtActivity window = new QtActivity();
		main.startActivity(window.getIntent());
		windows.put(window.getId(), window);
		return window.getId();
	}
	
	@SuppressWarnings("unused")
	private void destroyWindow(long windowId)
	{
		windows.get(windowId).finish();
		windows.remove(windowId);
	}
	
	@SuppressWarnings("unused")
	private void flushImage(long windowId, ShortBuffer image, int bytesPerRow,int x, int y, int r, int b)
	{
		
	}
	
	@SuppressWarnings("unused")
	private void setWindowGeometry(long windowId, int x, int y, int r, int b)
	{
		
	}

	static native void startQtApp();
	static native void quitQtApp();
}
