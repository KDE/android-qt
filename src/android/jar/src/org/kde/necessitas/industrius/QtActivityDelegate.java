/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL BogDan Vatra <bog_dan_ro@yahoo.com> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package org.kde.necessitas.industrius;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.content.res.Resources.Theme;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.method.MetaKeyKeyListener;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.view.inputmethod.InputMethodManager;

public class QtActivityDelegate implements QtLoaderInterface, QtActivityDelegateInterface
{
	private QtActivityInterface m_activityInterface=null;

    private final static int MINISTRO_INSTALL_REQUEST_CODE = 0xf3ee;
    private static final int MINISTRO_API_LEVEL=1; // Ministro api level (check IMinistro.aidl file)
    private static final int NECESSITAS_API_LEVEL=1; // Necessitas api level.

    private String m_qtLibs[]=null;
    private int m_id=-1;
    private boolean softwareKeyboardIsVisible=false;
    private long m_metaState;
    private int m_lastChar=0;
    private boolean m_fullScreen=false;
    private boolean m_started = false;
    private QtSurface m_surface=null;
    private QtLayout m_layout=null;

    private void startApplication(String [] libs, String environment, String params)
    {
        QtNative.loadQtLibraries(libs);
        try
        {
            ActivityInfo ai=getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
            if (ai.metaData.containsKey("android.app.bundled_libs_resource_id"))
            {
                // Load bundle libs
                int resourceId = ai.metaData.getInt("android.app.bundled_libs_resource_id");
                QtNative.loadBundledLibraries(getResources().getStringArray(resourceId));
            }

            if (ai.metaData.containsKey("android.app.lib_name")) // Load application
                QtNative.loadBundledLibraries(new String[]{ai.metaData.getString("android.app.lib_name")});

            // if the applications is debuggable and it has a native debug request
            if ( /*(ai.flags&ApplicationInfo.FLAG_DEBUGGABLE) != 0
                    &&*/ getIntent().getExtras() != null
                    && getIntent().getExtras().containsKey("native_debug")
                    && getIntent().getExtras().getString("native_debug").equals("true"))
            {
                try
                {
                    String packagePath=getPackageManager().getApplicationInfo(getPackageName(), PackageManager.GET_CONFIGURATIONS).dataDir+"/";
                    String gdbserverPath=getIntent().getExtras().containsKey("gdbserver_path")?getIntent().getExtras().getString("gdbserver_path"):packagePath+"lib/gdbserver ";
                    String socket=getIntent().getExtras().containsKey("gdbserver_socket")?getIntent().getExtras().getString("gdbserver_socket"):"+debug-socket";
                    // start debugger
                    m_debuggerProcess =Runtime.getRuntime().exec(gdbserverPath+socket+" --attach "+android.os.Process.myPid(),null, new File(packagePath));
                }
                catch (IOException ioe)
                {
                        Log.e(QtNative.QtTAG,"Can't start debugging"+ioe.getMessage());
                }
                catch (SecurityException se)
                {
                        Log.e(QtNative.QtTAG,"Can't start debugging"+se.getMessage());
                }
            }
            // start application

            if (ai.metaData.containsKey("android.app.application_startup_params"))
            {
                if (params.length()>0)
                    params+="\t";
                params += ai.metaData.getString("android.app.application_startup_params");
            }

            final String envPaths="HOME="+getFilesDir().getAbsolutePath()+
                                "\tTMPDIR="+getFilesDir().getAbsolutePath()+
                                "\tCACHE_PATH="+getCacheDir().getAbsolutePath();
            if (environment != null && environment.length()>0)
                environment=envPaths+"\t"+environment;
            else
                environment=envPaths;

            m_surface.applicationStared( QtNative.startApplication(params, environment) );
            m_started = true;
        }
        catch (NameNotFoundException e)
        {
            Log.e(QtNative.QtTAG, "Can't package metadata", e);
        }
    }

    private IMinistroCallback m_ministroCallback = new IMinistroCallback.Stub() {
//        @Override
//        public void libs(final String[] libs, final String evnVars, final String params,
//                         int errorCode, String errorMessage) throws RemoteException {
//            runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    startApplication(libs, evnVars, params);
//                }
//            });
//        }
		@Override
		public void loaderReady(Bundle loaderParams) throws RemoteException {
			// TODO Auto-generated method stub
			
		}
    };

    private ServiceConnection m_ministroConnection=new ServiceConnection() {
        private IMinistro m_service = null;
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            m_service = IMinistro.Stub.asInterface(service);
            try {
                    if (m_service!=null)
                            m_service.checkModules(m_ministroCallback, m_qtLibs, (String) QtActivity.this.getTitle(), MINISTRO_API_LEVEL, NECESSITAS_API_LEVEL);
            } catch (RemoteException e) {
                    e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            m_service = null;
        }
    };

    private boolean m_quitApp = true;
    private Process m_debuggerProcess=null; // debugger process
    private void exitApplication()
    {
        AlertDialog errorDialog = new AlertDialog.Builder(QtActivity.this).create();
        errorDialog.setMessage("Can't find Ministro service.\nThe application can't start.");
        errorDialog.setButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                finish();
            }
        });
        errorDialog.show();
    }

    private void startApp(final boolean firstStart)
    {
        try
        {
            ActivityInfo ai=getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
            if (!ai.metaData.containsKey("android.app.qt_libs_resource_id"))
            {
                // No required qt libs ?
                // Probably this application was compiled using static qt libs
                // or all qt libs are prebundled into the package
                m_ministroCallback.libs(null, null, null, 0, null);
                return;
            }
            int resourceId = ai.metaData.getInt("android.app.qt_libs_resource_id");
            m_qtLibs=getResources().getStringArray(resourceId);
            if (getIntent().getExtras()!= null && getIntent().getExtras().containsKey("use_local_qt_libs")
                    && getIntent().getExtras().getString("use_local_qt_libs").equals("true"))
            {
                ArrayList<String> libraryList= new ArrayList<String>();
                for(int i=0;i<m_qtLibs.length;i++)
                {
                    libraryList.add("/data/local/qt/lib/lib"+m_qtLibs[i]+".so");
                }

                if (getIntent().getExtras().containsKey("load_local_libs"))
                {
                    String []extraLibs=getIntent().getExtras().getString("load_local_libs").split(";");
                    for (int i=0;i<extraLibs.length;i++)
                        libraryList.add("/data/local/qt/"+extraLibs[i]);
                }

                String[] libs=new String[libraryList.size()];
                libs=libraryList.toArray(libs);
                m_ministroCallback.libs(libs,"QML_IMPORT_PATH=/data/local/qt/imports\tQT_PLUGIN_PATH=/data/local/qt/plugins", "-platform\tandroid", 0,null);
                return;
            }

            try {
                if (!bindService(new Intent(org.kde.necessitas.ministro.IMinistro.class.getCanonicalName()), m_ministroConnection, Context.BIND_AUTO_CREATE))
                    throw new SecurityException("");
            } catch (SecurityException e) {
                if (firstStart)
                {
                    AlertDialog.Builder downloadDialog = new AlertDialog.Builder(this);
                    downloadDialog.setMessage("This application requires Ministro, Qt libaries for Android. Would you like to install it?");
                    downloadDialog.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            try
                            {
                                Uri uri = Uri.parse("market://search?q=pname:eu.licentia.necessitas.ministro");
                                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                                startActivityForResult(intent, MINISTRO_INSTALL_REQUEST_CODE);
                            }
                            catch (Exception e) {
                                e.printStackTrace();
                                exitApplication();
                            }
                        }
                    });

                    downloadDialog.setNegativeButton("No", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            QtActivity.this.finish();
                        }
                    });
                    downloadDialog.show();
                }
                else
                {
                    exitApplication();
                }
            }
        }
        catch (Exception e)
        {
            Log.e(QtNative.QtTAG, "Can't create main activity", e);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode == MINISTRO_INSTALL_REQUEST_CODE)
                startApp(false);
        m_activityInterface.super_onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        m_quitApp = true;
        QtNative.setMainActivity(this);
        if (null == getLastNonConfigurationInstance())
        {
            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(metrics);
            QtNative.setApplicationDisplayMetrics(metrics.widthPixels, metrics.heightPixels,
                            metrics.widthPixels, metrics.heightPixels,
                            metrics.xdpi, metrics.ydpi);
        }
        // requestWindowFeature(Window.FEATURE_NO_TITLE);
        m_layout=new QtLayout(this);
        m_surface = new QtSurface(this, m_id);
        m_layout.addView(m_surface,0);
        setContentView(m_layout);
        m_layout.bringChildToFront(m_surface);
        if (null == getLastNonConfigurationInstance())
            startApp(true);
    }

    public QtLayout getQtLayout()
    {
        return m_layout;
    }

    public QtSurface getQtSurface()
    {
        return m_surface;
    }

    @Override
    public Object onRetainNonConfigurationInstance()
    {
        super.onRetainNonConfigurationInstance();
        m_quitApp = false;
        return true;
    }

    @Override
    protected void onDestroy()
    {
        QtNative.setMainActivity(null);
        super.onDestroy();
        if (m_quitApp)
        {
            Log.i(QtNative.QtTAG, "onDestroy");
            if (m_debuggerProcess != null)
                m_debuggerProcess.destroy();
            System.exit(0);// FIXME remove it or find a better way
        }
        QtNative.setMainActivity(null);
    }

    @Override
    protected void onResume()
    {
        // fire all lostActions
        synchronized (QtNative.m_mainActivityMutex)
        {
            Iterator<Runnable> itr=QtNative.getLostActions().iterator();
            while(itr.hasNext())
                runOnUiThread(itr.next());
            if (m_started)
            {
                QtNative.clearLostActions();
                QtNative.updateWindow();
            }
        }
        super.onResume();
    }

    public void redrawWindow(int left, int top, int right, int bottom) {
        m_surface.drawBitmap(new Rect(left, top, right, bottom));
    }

    public void setFullScreen(boolean enterFullScreen)
    {
        if (m_fullScreen == enterFullScreen)
            return;
        if (m_fullScreen = enterFullScreen)
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        else
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putBoolean("FullScreen",m_fullScreen);
        outState.putBoolean("Started", m_started);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        setFullScreen(savedInstanceState.getBoolean("FullScreen"));
        m_started = savedInstanceState.getBoolean("Started");
        if (m_started)
            m_surface.applicationStared( true );
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
        if (m_started && event.getAction() == KeyEvent.ACTION_MULTIPLE &&
            event.getCharacters() != null &&
            event.getCharacters().length() == 1 &&
            event.getKeyCode() == 0) {
            Log.i(QtNative.QtTAG, "dispatchKeyEvent at MULTIPLE with one character: "+event.getCharacters());
            QtNative.keyDown(0, event.getCharacters().charAt(0), event.getMetaState());
            QtNative.keyUp(0, event.getCharacters().charAt(0), event.getMetaState());
        }

        return super.dispatchKeyEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (!m_started)
            return false;
        m_metaState = MetaKeyKeyListener.handleKeyDown(m_metaState, keyCode, event);
        int c = event.getUnicodeChar(MetaKeyKeyListener.getMetaState(m_metaState));
        int lc=c;
        m_metaState = MetaKeyKeyListener.adjustMetaAfterKeypress(m_metaState);

        if ((c & KeyCharacterMap.COMBINING_ACCENT) != 0)
        {
            c = c & KeyCharacterMap.COMBINING_ACCENT_MASK;
            int composed = KeyEvent.getDeadChar(m_lastChar, c);
            c = composed;
        }
        m_lastChar = lc;
        if (keyCode != KeyEvent.KEYCODE_BACK)
                QtNative.keyDown(keyCode, c, event.getMetaState());
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (!m_started)
            return false;
        m_metaState = MetaKeyKeyListener.handleKeyUp(m_metaState, keyCode, event);
        QtNative.keyUp(keyCode, event.getUnicodeChar(), event.getMetaState());
        return true;
    }

	@Override
	public boolean startApplication(QtActivityInterface activityInterface, Bundle loaderParams)
	{
		// TODO Continue with loader implementation
		m_activityInterface = activityInterface;
		
		return true;
	}

	@Override
	public void onApplyThemeResource(Theme theme, int resid, boolean first)
	{
		m_activityInterface.super_onApplyThemeResource(theme, resid, first);
	}

	@Override
	public void onAttachedToWindow()
	{
		m_activityInterface.super_onAttachedToWindow();
	}

	@Override
	public void onBackPressed()
	{
		m_activityInterface.super_onBackPressed();
	}

	@Override
	public void onChildTitleChanged(Activity childActivity, CharSequence title)
	{
		m_activityInterface.super_onChildTitleChanged(childActivity, title);
	}

	@Override
	public void onContentChanged()
	{
		m_activityInterface.super_onContentChanged();
	}

	@Override
	public boolean onContextItemSelected(MenuItem item)
	{
		return m_activityInterface.super_onContextItemSelected(item);
	}

	@Override
	public void onContextMenuClosed(Menu menu)
	{
		m_activityInterface.super_onContextMenuClosed(menu);
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo)
	{
		m_activityInterface.super_onCreateContextMenu(menu, v, menuInfo);
	}

	@Override
	public CharSequence onCreateDescription()
	{
		return m_activityInterface.super_onCreateDescription();
	}

	@Override
	public Dialog onCreateDialog(int id)
	{
		return m_activityInterface.super_onCreateDialog(id);
	}

	@Override
	public Dialog onCreateDialog(int id, Bundle args)
	{
		return m_activityInterface.super_onCreateDialog(id, args);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		return m_activityInterface.super_onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onCreatePanelMenu(int featureId, Menu menu)
	{
		return m_activityInterface.super_onCreatePanelMenu(featureId, menu);
	}

	@Override
	public View onCreatePanelView(int featureId)
	{
		return m_activityInterface.super_onCreatePanelView(featureId);
	}

	@Override
	public boolean onCreateThumbnail(Bitmap outBitmap, Canvas canvas)
	{
		return m_activityInterface.super_onCreateThumbnail(outBitmap, canvas);
	}

	@Override
	public View onCreateView(String name, Context context, AttributeSet attrs)
	{
		return m_activityInterface.super_onCreateView(name, context, attrs);
	}

	@Override
    public void onConfigurationChanged(Configuration newConfig) {
    	m_activityInterface.super_onConfigurationChanged(newConfig);
    }

	@Override
	public void onDetachedFromWindow()
	{
		m_activityInterface.super_onDetachedFromWindow();
	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event)
	{
		return m_activityInterface.super_onKeyLongPress(keyCode, event);
	}

	@Override
	public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event)
	{
		return m_activityInterface.super_onKeyMultiple(keyCode, repeatCount, event);
	}

	@Override
	public void onLowMemory()
	{
		m_activityInterface.super_onLowMemory();
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item)
	{
		return m_activityInterface.super_onMenuItemSelected(featureId, item);
	}

	@Override
	public boolean onMenuOpened(int featureId, Menu menu)
	{
		return m_activityInterface.super_onMenuOpened(featureId, menu);
	}

	@Override
	public void onNewIntent(Intent intent)
	{
		m_activityInterface.super_onNewIntent(intent);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		return m_activityInterface.super_onOptionsItemSelected(item);
	}

	@Override
	public void onOptionsMenuClosed(Menu menu)
	{
		m_activityInterface.super_onOptionsMenuClosed(menu);
	}

	@Override
	public void onPanelClosed(int featureId, Menu menu)
	{
		m_activityInterface.super_onPanelClosed(featureId, menu);
	}

	@Override
	public void onPause()
	{
		m_activityInterface.super_onPause();		
	}

	@Override
	public void onPostCreate(Bundle savedInstanceState)
	{
		m_activityInterface.super_onPostCreate(savedInstanceState);		
	}

	@Override
	public void onPostResume()
	{
		m_activityInterface.super_onPostResume();			
	}

	@Override
	public void onPrepareDialog(int id, Dialog dialog)
	{
		m_activityInterface.super_onPrepareDialog(id, dialog);			
	}

	@Override
	public void onPrepareDialog(int id, Dialog dialog, Bundle args)
	{
		m_activityInterface.super_onPrepareDialog(id, dialog, args);			
	}

	@Override
	public boolean onPrepareOptionsMenu(Menu menu)
	{
		return m_activityInterface.super_onPrepareOptionsMenu(menu);			
	}

	@Override
	public boolean onPreparePanel(int featureId, View view, Menu menu)
	{
		return m_activityInterface.super_onPreparePanel(featureId, view, menu);
	}

	@Override
	public void onRestart()
	{
		m_activityInterface.super_onRestart();
	}

	@Override
	public boolean onSearchRequested()
	{
		return m_activityInterface.super_onSearchRequested();
	}

	@Override
	public void onStart()
	{
		m_activityInterface.super_onStart();
	}

	@Override
	public void onStop()
	{
		m_activityInterface.super_onStop();
	}

	@Override
	public void onTerminate()
	{
		QtNative.terminateQt();
	}

	@Override
	public void onTitleChanged(CharSequence title, int color)
	{
		m_activityInterface.super_onTitleChanged(title, color);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		return m_activityInterface.super_onTouchEvent(event);
	}

	@Override
	public boolean onTrackballEvent(MotionEvent event)
	{
		return m_activityInterface.super_onTrackballEvent(event);
	}

	@Override
	public void onUserInteraction()
	{
		m_activityInterface.super_onUserInteraction();
	}

	@Override
	public void onUserLeaveHint()
	{
		m_activityInterface.super_onUserLeaveHint();
	}

	@Override
	public void onWindowAttributesChanged(LayoutParams params)
	{
		m_activityInterface.super_onWindowAttributesChanged(params);
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus)
	{
		m_activityInterface.super_onWindowFocusChanged(hasFocus);
	}
}
