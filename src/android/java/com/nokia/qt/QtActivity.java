package com.nokia.qt;

import android.app.Activity;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.RelativeLayout;

public class QtActivity extends Activity {
	private boolean quitApp=true;
	private String appName="";
	private Object jniProxyObject = null;
	private String[] libraries={"QtCore", "QtNetwork", "QtXml", 
								/*"QtScript", "QtSql", */"QtGui",
								"QtOpenGL","QtSvg"/*, "QtScriptTools", 
								"QtDeclarative","QtMultimedia", "QtWebKit"*/}; // Be default try to load all Qt libraries
	
	public QtActivity() {
		QtApplication.setActivity(this);
	}
	
	public void setApplication(String app)
	{
		appName=app;
	}
	
	public void setJniProxyObject(Object jniProxyObject)
	{
        this.jniProxyObject = jniProxyObject;
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
				//QtApplication.
			}
			ViewGroup view=new RelativeLayout(this);
			QtApplication.setView(view);
			setContentView(view);
			
//			Rect rect= new Rect();
//			Window window= getWindow();
//			window.getDecorView().getWindowVisibleDisplayFrame(rect);
//			int statusBarHeight= rect.top;
//			int contentViewTop= 
//			    window.findViewById(Window.ID_ANDROID_CONTENT).getTop();
//			int titleBarHeight= contentViewTop - statusBarHeight;
			
			DisplayMetrics metrics = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(metrics);
			QtApplication.setDisplayMetrics(metrics.widthPixels, metrics.heightPixels, metrics.xdpi, metrics.ydpi);
			quitApp=true;
		}
		catch (Exception e)
		{
			Log.e(QtApplication.QtTAG, "Can't create main activity", e);
		}
    }

    @Override
    protected void onPause() {
    	QtApplication.pauseQtApp();
		Log.i(QtApplication.QtTAG,"onPause");
		super.onPause();
    }

    @Override
	protected void onResume() {
    	QtApplication.resumeQtApp();
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
    		QtApplication.quitQtApp();
    	}
    }   
}
