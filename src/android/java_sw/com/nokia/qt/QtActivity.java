package com.nokia.qt;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;

public class QtActivity extends Activity {
	private boolean quitApp=true;
	private String appName=""; 
	private String[] libraries={"QtCore", "QtNetwork", "QtXml", 
								"QtScript", "QtSql", "QtGui",
								"QtOpenGL","QtSvg", "QtScriptTools", 
								"QtDeclarative", "QtMultimedia", "QtWebKit"}; // Be default try to load all Qt libraries
	
	public void setApplication(String app)
	{
		appName=app;
	}
	
	public void setLibraries(String[] libs)
	{
		libraries=libs;
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		QtApplication.keyDown(keyCode, event.getUnicodeChar(), event.getMetaState());
		if (keyCode==KeyEvent.KEYCODE_BACK)
			return super.onKeyDown(keyCode, event);
		return true;
	}
	
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event)
	{
		QtApplication.keyUp(keyCode, event.getUnicodeChar(), event.getMetaState());
		if (keyCode==KeyEvent.KEYCODE_BACK)
			return super.onKeyUp(keyCode, event);
		return true;
	}

    @Override
    public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		try
		{
			if (null==getLastNonConfigurationInstance())
			{
				QtApplication.loadLibraries(libraries);
				QtApplication.loadApplication(appName);
			}
			quitApp=true;
			setContentView(new QtSurface(this));
			QtApplication.m_activity=this;
		}
		catch (Exception e)
		{
			Log.e(QtApplication.QtTAG, "Can't create main activity", e);
		}
    }

    @Override
    protected void onPause() {
    	QtApplication.pauseQtApp();
		QtApplication.m_activity=null;
		Log.i(QtApplication.QtTAG,"onPause");
		super.onPause();
    }

    @Override
	protected void onResume() {
    	QtApplication.resumeQtApp();
		QtApplication.m_activity=this;
    	Log.i(QtApplication.QtTAG,"onResume");
    	super.onRestart();
	}

    @Override
    public Object onRetainNonConfigurationInstance() {
    	super.onRetainNonConfigurationInstance();
    	quitApp=false;
    	return true;
    }
    
    @Override
    protected void onDestroy()
    {
    	super.onDestroy();
    	if (quitApp)
    	{
			QtApplication.m_activity=null;
    		QtApplication.quitQtApp();
    	}
    }   
}
