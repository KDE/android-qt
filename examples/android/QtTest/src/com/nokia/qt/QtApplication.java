package com.nokia.qt;

import java.io.File;
import java.nio.ShortBuffer;

import android.view.Surface;

public class QtApplication {

	public static final String QtTAG = "Qt JAVA";
	
	static
	{
	//	System.load("/data/local/qt/lib/libAndroidSetQtEnv.so");
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

	@SuppressWarnings("unused")
	private void flushImage(ShortBuffer image, int bytesPerRow,int x, int y, int r, int b)
	{
	}

	static native void startQtApp();
	public static native void quitQtApp();
	public static native void setDisplayMetrics(int widthPixels, int heightPixels,
			float xdpi, float ydpi);
	public static native void setSurface(Surface surface);
	public static native void destroySurface();
	public static native void mouseDown(int x, int y);
	public static native void mouseUp(int x, int y);
	public static native void mouseMove(int x, int y);
	public static native void keyDown(int key, int unicode, int modifier);
	public static native void keyUp(int key, int unicode, int modifier);
}
