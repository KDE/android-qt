package com.nokia.qt;

import java.io.File;
import java.nio.ShortBuffer;

import android.util.Log;
import android.view.Surface;

public class QtApplication {

	public static final String QtTAG = "Qt JAVA";
	
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
	private void flushImage(ShortBuffer image, int bytesPerRow,int x, int y, int r, int b)
	{
	}

	public static native void startQtApp();
	public static native void pauseQtApp();
	public static native void resumeQtApp();
	public static native void quitQtApp();
	public static native void setDisplayMetrics(int widthPixels, int heightPixels,
			float xdpi, float ydpi);
	public static native void setSurface(Surface surface, int width, int height);
	
	public static native void destroySurface();
	public static native void mouseDown(int x, int y);
	public static native void mouseUp(int x, int y);
	public static native void mouseMove(int x, int y);
	public static native void keyDown(int key, int unicode, int modifier);
	public static native void keyUp(int key, int unicode, int modifier);


}
