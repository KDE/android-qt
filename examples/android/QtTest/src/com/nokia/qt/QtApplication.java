package com.nokia.qt;

import java.io.File;
import java.nio.ShortBuffer;
import java.util.HashMap;

import android.app.Application;
import android.util.Log;
import android.view.Surface;

public class QtApplication {
	private static final String TAG = "Java Gps";
	
	static
	{
		System.load("/data/local/qt/lib/libAndroidSetQtEnv.so");
	}
	
	public static void loadApplication(String lib)
	{
		 File f = new File("/data/local/lib/lib"+lib+".so");
		 if (f.exists())
			 System.load("/data/local/lib/lib"+lib+".so");
		 else
			 System.loadLibrary(lib);
		 startQtApp();
	}

	private static 	Application main=null;
	private static HashMap<Long, QtActivity> windows = new HashMap<Long, QtActivity>();
	
	public static void setApplication(Application main) {
		QtApplication.main = main;
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
	static native void setWindowSurface(long windowId, Surface surface);
}
