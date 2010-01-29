package com.nokia.qt;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;

public class QtActivity extends Activity {
	private boolean quitApp=true;
	private String appName=""; 

	public void setApplication(String app)
	{
		appName=app;
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
				QtApplication.loadApplication(appName);
			}
			quitApp=true;
			setContentView(new QtSurface(this));
			Log.i(QtApplication.QtTAG,"Application started");
		}
		catch (Exception e)
		{
			Log.e(QtApplication.QtTAG, "Can't create main activity", e);
		}
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
    		QtApplication.quitQtApp();
    	}
    }   
}
