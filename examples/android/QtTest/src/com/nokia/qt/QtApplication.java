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
	static native void setSurface(Surface surface);
	public static native void actionDown(int x, int y);
	public static native void actionUp(int x, int y);
	public static native void actionMove(int x, int y);
}
