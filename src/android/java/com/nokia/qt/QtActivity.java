package com.nokia.qt;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.method.MetaKeyKeyListener;
import android.util.Log;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.Window;
import android.view.inputmethod.InputMethodManager;

public class QtActivity extends Activity
{

	public enum QtLibrary {
        QtCore, QtNetwork, QtXml, QtXmlPatterns, QtScript, QtSql, QtGui, QtOpenGL, QtSvg, QtScriptTools, QtDeclarative, QtMultimedia, QtWebKit, QtAndroid_mw, QtAndroid_sw, QtAndroidBridge
    }
    private boolean singleWindow=true;
    private Object jniProxyObject = null;
    private boolean quitApp = true;
    private String appName = "";
    private List<String> libraries = new ArrayList<String>();
    private boolean softwareKeyboardIsVisible=false;
    private long metaState;
    private int lastChar=0;
   
    private static final int ProcessEvents = 1;
	private Handler mHandler = new Handler()
	{
		@Override
		public void handleMessage(Message msg)
		{
			switch(msg.what)
			{
				case ProcessEvents:
					QtApplication.processQtEvents();
					break;
			}
		}
	};

	public QtActivity()
    {
        // By default try to load all Qt libraries
        addQtLibrary(QtLibrary.QtCore);
        addQtLibrary(QtLibrary.QtNetwork);
        addQtLibrary(QtLibrary.QtXml);
        addQtLibrary(QtLibrary.QtScript);
        addQtLibrary(QtLibrary.QtSql);
        addQtLibrary(QtLibrary.QtGui);
        addQtLibrary(QtLibrary.QtOpenGL);
        addQtLibrary(QtLibrary.QtSvg);
        addQtLibrary(QtLibrary.QtScriptTools);
        addQtLibrary(QtLibrary.QtMultimedia);
        addQtLibrary(QtLibrary.QtWebKit);
        addQtLibrary(QtLibrary.QtXmlPatterns);
        addQtLibrary(QtLibrary.QtDeclarative);
        if (singleWindow)
        	addQtLibrary(QtLibrary.QtAndroid_sw);
        else
        	addQtLibrary(QtLibrary.QtAndroid_mw);
        addQtLibrary(QtLibrary.QtAndroidBridge);
        QtApplication.setActivity(this);
    }

    public void setApplication(String app)
    {
        appName = app;
    }

    public void setLibraries(List<String> libs)
    {
        libraries.clear();
        libraries.addAll(libs);
    }
    
    public void addLibrary(String lib)
    {
        if (!libraries.contains(lib))
            libraries.add(lib);
    }
    
    public void addQtLibrary(QtLibrary lib)
    {
        if (!libraries.contains(lib.toString()))
            libraries.add(lib.toString());
    }
    
    public void removeQtLibrary(QtLibrary lib)
    {
        int index = libraries.indexOf(lib.toString());
        if (index != -1)
            libraries.remove(index);
    }
    
    public void removeLibrary(String lib)
    {
        int index = libraries.indexOf(lib);
        if (index != -1)
            libraries.remove(index);
    }
    
    public void setJniProxyObject(Object jniProxyObject)
    {
        this.jniProxyObject = jniProxyObject;
    }
    
    public void showSoftwareKeyboard()
    {
    	softwareKeyboardIsVisible = true;
    	InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
    	imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY );
    }

    public void hideSoftwareKeyboard()
    {
    	if (softwareKeyboardIsVisible)
    	{
    		InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
    		imm.toggleSoftInput(0, 0);
    	}
    	softwareKeyboardIsVisible = false;
    }
    
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_MULTIPLE && 
            event.getCharacters() != null && 
            event.getCharacters().length() == 1 &&
            event.getKeyCode() == 0) {
            Log.i(QtApplication.QtTAG, "dispatchKeyEvent at MULTIPLE with one character: "+event.getCharacters());
            QtApplication.keyDown(0, event.getCharacters().charAt(0), event.getMetaState());
            QtApplication.keyUp(0, event.getCharacters().charAt(0), event.getMetaState());
        }
    
        return super.dispatchKeyEvent(event);
    }

   
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
    	metaState = MetaKeyKeyListener.handleKeyDown(metaState,
                keyCode, event);
        int c = event.getUnicodeChar(MetaKeyKeyListener.getMetaState(metaState));
        int lc=c;
        metaState = MetaKeyKeyListener.adjustMetaAfterKeypress(metaState);

        if ((c & KeyCharacterMap.COMBINING_ACCENT) != 0)
        {
            c = c & KeyCharacterMap.COMBINING_ACCENT_MASK;
            int composed = KeyEvent.getDeadChar(lastChar, c);
            c = composed;
        }
        lastChar = lc;
        if (keyCode != KeyEvent.KEYCODE_BACK)
        	QtApplication.keyDown(keyCode, c, event.getMetaState());
        return true;
    }

  
   
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
    	metaState = MetaKeyKeyListener.handleKeyUp(metaState, keyCode, event);
        QtApplication.keyUp(keyCode, event.getUnicodeChar(), event.getMetaState());
        return true;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        try
        {
            String path=getFilesDir().getAbsolutePath();
            Log.i(QtApplication.QtTAG, path);
            if (singleWindow)
            {
    			setContentView(new QtSurface(this));
            }
            else
            {
	            QtMainView view = new QtMainView(this);
	            setContentView(view);
	            QtApplication.setView(view);
            }
            if (null == getLastNonConfigurationInstance())
            {
                QtApplication.loadLibraries(libraries);
                QtApplication.loadApplication(appName, (jniProxyObject == null) ? this : jniProxyObject);
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
        if (!quitApp)
        {
            Log.i(QtApplication.QtTAG, "onPause");
            QtApplication.pauseQtApp();
        }
        super.onPause();
    }

    @Override
    protected void onResume()
    {
        Log.i(QtApplication.QtTAG, "onResume");
        QtApplication.resumeQtApp();
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
//            QtApplication.quitQtAndroidPlugin();
            System.exit(0);
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        Log.i(QtApplication.QtTAG, "onSaveInstanceState");
        super.onSaveInstanceState(outState);
        Log.i(QtApplication.QtTAG, "onSaveInstanceState");
        QtMainView view = QtApplication.getView();
        if (view != null)
        {
	        outState.putInt("Surfaces", view.getChildCount());
	        for (int i=0;i<view.getChildCount();i++)
	        {
	            QtWindow surface=(QtWindow) view.getChildAt(i);
	            Log.i(QtApplication.QtTAG,"id"+surface.getId()+","+surface.getLeft()+","+surface.getTop()+","+surface.getRight()+","+surface.getBottom());
	            int surfaceInfo[]={((QtWindowInterface)surface).isOpenGl(), surface.getId(), surface.getLeft(), surface.getTop(), surface.getRight(), surface.getBottom()};
	            outState.putIntArray("Surface_"+i, surfaceInfo);
	        }
        }
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        Log.i(QtApplication.QtTAG, "onRestoreInstanceState");
        super.onRestoreInstanceState(savedInstanceState);
        Log.i(QtApplication.QtTAG, "onRestoreInstanceState");
        QtMainView view = QtApplication.getView();
        if (view != null)
        {
	        int surfaces=savedInstanceState.getInt("Surfaces");
	        for (int i=0;i<surfaces;i++)
	        {
	            int surfaceInfo[]= {0,0,0,0,0,0};
	            surfaceInfo=savedInstanceState.getIntArray("Surface_"+i);
	            if (surfaceInfo[0]==1) // OpenGl Surface
	                view.addView(new QtGlWindow(this, surfaceInfo[1], surfaceInfo[2], surfaceInfo[3], surfaceInfo[4], surfaceInfo[5]),i);
	            else
	                view.addView(new QtWindow(this, surfaceInfo[1], surfaceInfo[2], surfaceInfo[3], surfaceInfo[4], surfaceInfo[5]), i);
	        }
        }
    }
    
    void processEvents(long miliseconds)
    {
    	mHandler.sendMessageDelayed(mHandler.obtainMessage(ProcessEvents), miliseconds);
    }
}
