package com.nokia.qt;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.ViewGroup;
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
			ViewGroup view = new QtMainView(this);
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

	@Override
	protected void onPause()
	{
		QtApplication.pauseQtApp();
		Log.i(QtApplication.QtTAG, "onPause");
		super.onPause();
	}

	@Override
	protected void onResume()
	{
		QtApplication.resumeQtApp();
		Log.i(QtApplication.QtTAG, "onResume");
		super.onRestart();
	}

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
		super.onSaveInstanceState(outState);
		Log.i(QtApplication.QtTAG, "onSaveInstanceState");
		ViewGroup view = QtApplication.getView();
		outState.putInt("Surfaces", view.getChildCount());
		for (int i=0;i<view.getChildCount();i++)
		{
			QtSurface surface=(QtSurface) view.getChildAt(i);
			int surfaceInfo[]={surface.getId(), surface.getLeft(), surface.getTop(), surface.getRight(), surface.getBottom()};
			outState.putIntArray("Surface_"+i, surfaceInfo);
		}
	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		super.onRestoreInstanceState(savedInstanceState);
		Log.i(QtApplication.QtTAG, "onRestoreInstanceState");
		ViewGroup view = QtApplication.getView();
		int surfaces=savedInstanceState.getInt("Surfaces");
		for (int i=0;i<surfaces;i++)
		{
			int surfaceInfo[]= {0,0,0,0,0};
			surfaceInfo=savedInstanceState.getIntArray("Surface_"+i);
			view.addView(new QtSurface(this, surfaceInfo[0], surfaceInfo[1], surfaceInfo[2], surfaceInfo[3], surfaceInfo[4]),i);
		}		
	}
}
