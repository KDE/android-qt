package com.nokia.qt;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;

public class QtActivity extends Activity
{
	private boolean quitApp = true;
	private String appName = "";
	private String[] libraries = { "QtCore", "QtNetwork", "QtXml",
    	/* "QtScript", "QtSql", */"QtGui", "QtOpenGL", "QtSvg"
    	/* , "QtScriptTools", "QtDeclarative" ,"QtMultimedia", "QtWebKit" */};
	// By default try to load all Qt libraries

	public QtActivity()
	{
		QtApplication.setActivity(this);
	}

	public void setApplication(String app)
	{
		appName = app;
	}

	public void setLibraries(String[] libs)
	{
		libraries = libs;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		QtApplication.keyDown(keyCode, event.getUnicodeChar(), event.getMetaState());

		if (keyCode == KeyEvent.KEYCODE_BACK)
			return super.onKeyDown(keyCode, event);
		return true;
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event)
	{
		QtApplication.keyUp(keyCode, event.getUnicodeChar(), event.getMetaState());
		
		if (keyCode == KeyEvent.KEYCODE_BACK)
			return super.onKeyUp(keyCode, event);
		return true;
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		try
		{
			QtMainView view = new QtMainView(this);
			QtApplication.setView(view);
			setContentView(view);

			if (null == getLastNonConfigurationInstance())
			{
				QtApplication.loadLibraries(libraries);
				QtApplication.loadApplication(appName);
				Log.i(QtApplication.QtTAG, "onCreate");
			}
			quitApp = true;
		}
		catch (Exception e)
		{
			Log.e(QtApplication.QtTAG, "Can't create main activity", e);
		}
	}

//	@Override
//	protected void onPause()
//	{
//		Log.i(QtApplication.QtTAG, "onPause");
//		QtApplication.pauseQtApp();
//		super.onPause();
//	}
//
//	@Override
//	protected void onResume()
//	{
//		Log.i(QtApplication.QtTAG, "onResume");
//		QtApplication.resumeQtApp();
//		super.onRestart();
//	}

	@Override
	public Object onRetainNonConfigurationInstance()
	{
		super.onRetainNonConfigurationInstance();
		quitApp = false;
		return true;
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		if (quitApp)
		{
			Log.i(QtApplication.QtTAG, "onDestroy");
			QtApplication.resumeQtApp();
			QtApplication.unlockSurface();
			QtApplication.quitQtApp();
		}
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		Log.i(QtApplication.QtTAG, "onSaveInstanceState");
		super.onSaveInstanceState(outState);
		Log.i(QtApplication.QtTAG, "onSaveInstanceState");
		QtMainView view = QtApplication.getView();
		outState.putInt("Surfaces", view.getChildCount());
		for (int i=0;i<view.getChildCount();i++)
		{
			QtSurface surface=(QtSurface) view.getChildAt(i);
			int surfaceInfo[]={surface.isOpenGL(), surface.getId(), surface.getLeft(), surface.getTop(), surface.getRight(), surface.getBottom()};
			outState.putIntArray("Surface_"+i, surfaceInfo);
		}
	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		Log.i(QtApplication.QtTAG, "onRestoreInstanceState");
		super.onRestoreInstanceState(savedInstanceState);
		Log.i(QtApplication.QtTAG, "onRestoreInstanceState");
		QtMainView view = QtApplication.getView();
		int surfaces=savedInstanceState.getInt("Surfaces");
		for (int i=0;i<surfaces;i++)
		{
			int surfaceInfo[]= {0,0,0,0,0,0};
			surfaceInfo=savedInstanceState.getIntArray("Surface_"+i);
			view.addView(new QtSurface(this, (surfaceInfo[0]==1)?true:false, surfaceInfo[1], surfaceInfo[2], surfaceInfo[3], surfaceInfo[4], surfaceInfo[5]),i);
		}		
	}
}
