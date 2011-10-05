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

import java.util.ArrayList;
import java.util.Arrays;

import org.kde.necessitas.ministro.IMinistro;
import org.kde.necessitas.ministro.IMinistroCallback;

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
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import dalvik.system.DexClassLoader;

public class QtActivity extends Activity implements QtActivityInterface
{
    private final static int MINISTRO_INSTALL_REQUEST_CODE = 0xf3ee; // request code used to know when Ministro instalation is finished
    private static final int MINISTRO_API_LEVEL=1; // Ministro api level (check IMinistro.aidl file)
    private static final int QT_API_LEVEL=4; // Necessitas api level.

    private ActivityInfo m_activityInfo = null; // activity info object, used to access the libs and the strings
	private DexClassLoader m_classLoader = null; // loader object
    private String[] m_qtLibs = null; // required qt libs

	// this function is used to load and start the loader
	private void loadApplication(Bundle loaderParams)
	{ 
		try
		{
			if (loaderParams.getInt("error.code") != 0)
			{ // fatal error, show the error and quit
		        AlertDialog errorDialog = new AlertDialog.Builder(QtActivity.this).create();
		        errorDialog.setMessage(loaderParams.getString("error.message"));
		        errorDialog.setButton(getResources().getString(android.R.string.ok), new DialogInterface.OnClickListener() {
		            public void onClick(DialogInterface dialog, int which) {
		                finish();
		            }
		        });
		        errorDialog.show();
		        return;
			}

			// add all bundled libs to loader params
			ArrayList<String> libs = new ArrayList<String>();
			if ( m_activityInfo.metaData.containsKey("android.app.bundled_libs_resource_id") )
				libs.addAll(Arrays.asList(getResources().getStringArray(m_activityInfo.metaData.getInt("android.app.bundled_libs_resource_id"))));

			if ( m_activityInfo.metaData.containsKey("android.app.lib_name") )
				libs.add(m_activityInfo.metaData.getString("lib_name"));
			loaderParams.putStringArrayList("bundled.libs", libs);

			// load and start QtLoader class
			m_classLoader = new DexClassLoader(loaderParams.getString("dex.path") // .jar/.apk files 
											, getDir("outdex", Context.MODE_PRIVATE).getAbsolutePath() // directory where optimized DEX files should be written. 
											, loaderParams.containsKey("lib.path")?loaderParams.getString("lib.path"):null // libs folder (if exists)
											, getClassLoader()); // parent loader

			@SuppressWarnings("rawtypes")
			Class loaderClass = m_classLoader.loadClass("loader.class.name"); // load QtLoader class 
			QtLoaderInterface qtLoader=(QtLoaderInterface)loaderClass.newInstance(); // create an instance
			if (!qtLoader.loadApplication(this, loaderParams))
				throw new Exception("");

			if (!qtLoader.statApplication())
				throw new Exception("");
			
		} catch (Exception e) {
	        AlertDialog errorDialog = new AlertDialog.Builder(QtActivity.this).create();
	        if (m_activityInfo != null && m_activityInfo.metaData.containsKey("android.app.fatal_error_msg"))
				errorDialog.setMessage(m_activityInfo.metaData.getString("android.app.fatal_error_msg"));
			else
				errorDialog.setMessage("Fatal error, your application can't be started.");
	        errorDialog.setButton(getResources().getString(android.R.string.ok), new DialogInterface.OnClickListener() {
	            public void onClick(DialogInterface dialog, int which) {
	                finish();
	            }
	        });
	        errorDialog.show();
		}
	}
	
    private IMinistroCallback m_ministroCallback = new IMinistroCallback.Stub() {
    	// this function is called back by Ministro.
		@Override
		public void loaderReady(Bundle loaderParams) throws RemoteException
		{
			loadApplication(loaderParams);
		}
    };

    private ServiceConnection m_ministroConnection=new ServiceConnection() {
        private IMinistro m_service = null;

        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            m_service = IMinistro.Stub.asInterface(service);
            try {
	            if (m_service!=null)
	                    m_service.checkModules(m_ministroCallback, m_qtLibs, (String) QtActivity.this.getTitle(), MINISTRO_API_LEVEL, QT_API_LEVEL);
            } catch (RemoteException e) {
                    e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name)
        {
            m_service = null;
        }
    };

    private void ministroNotFound()
    {
        AlertDialog errorDialog = new AlertDialog.Builder(QtActivity.this).create();

        if (m_activityInfo != null && m_activityInfo.metaData.containsKey("android.app.ministro_not_found_msg"))
			errorDialog.setMessage(m_activityInfo.metaData.getString("android.app.ministro_not_found_msg"));
		else
			errorDialog.setMessage("Can't find Ministro service.\nThe application can't start.");

        errorDialog.setButton(getResources().getString(android.R.string.ok), new DialogInterface.OnClickListener() {
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
            if (ai.metaData.containsKey("android.app.qt_libs_resource_id"))
            {
                int resourceId = ai.metaData.getInt("android.app.qt_libs_resource_id");
                m_qtLibs=getResources().getStringArray(resourceId);
            }

            if (getIntent().getExtras()!= null && getIntent().getExtras().containsKey("use_local_qt_libs")
                    && getIntent().getExtras().getString("use_local_qt_libs").equals("true"))
            {
                ArrayList<String> libraryList= new ArrayList<String>();

                if (m_qtLibs != null)
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
//                m_ministroCallback.libs(libs,"QML_IMPORT_PATH=/data/local/qt/imports\tQT_PLUGIN_PATH=/data/local/qt/plugins", "-platform\tandroid", 0,null);
                return;
            }

            try {
                if (!bindService(new Intent(org.kde.necessitas.ministro.IMinistro.class.getCanonicalName()), m_ministroConnection, Context.BIND_AUTO_CREATE))
                    throw new SecurityException("");
            } catch (SecurityException e) {
                if (firstStart)
                {
                    AlertDialog.Builder downloadDialog = new AlertDialog.Builder(this);
                    if (m_activityInfo != null && m_activityInfo.metaData.containsKey("android.app.android.app.ministro_needed_msg"))
                    	downloadDialog.setMessage(m_activityInfo.metaData.getString("android.app.android.app.ministro_needed_msg"));
                    downloadDialog.setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            try
                            {
                                Uri uri = Uri.parse("market://search?q=pname:eu.licentia.necessitas.ministro");
                                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                                startActivityForResult(intent, MINISTRO_INSTALL_REQUEST_CODE);
                            }
                            catch (Exception e) {
                                e.printStackTrace();
                                ministroNotFound();
                            }
                        }
                    });

                    downloadDialog.setNegativeButton(android.R.string.no, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            QtActivity.this.finish();
                        }
                    });
                    downloadDialog.show();
                }
                else
                {
                    ministroNotFound();
                }
            }
        }
        catch (Exception e)
        {
            Log.e(QtApplication.QtTAG, "Can't create main activity", e);
        }
    }

	@Override
	public void setQtActivityListener(QtActivityDelegate listener)
	{
		QtApplication.m_activityListener = listener;
	}

	@Override
	public Activity getActivity()
	{
		return this;
	}

	/////////////////////////// forward all notifications ////////////////////////////
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (QtApplication.m_activityListener != null)
		{
			QtApplication.m_activityListener.onActivityResult(requestCode, resultCode, data);
			return;
		}
	    if (requestCode == MINISTRO_INSTALL_REQUEST_CODE)
	            startApp(false);
	    super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	protected void onApplyThemeResource(Theme theme, int resid, boolean first)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onApplyThemeResource(theme, resid, first);
		else
			super.onApplyThemeResource(theme, resid, first);
	}
	
	@Override
	public void onAttachedToWindow()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onAttachedToWindow();
		else
			super.onAttachedToWindow();
	}
	
	@Override
	public void onBackPressed()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onBackPressed();
		else
			super.onBackPressed();
	}
	
	@Override
	protected void onChildTitleChanged(Activity childActivity, CharSequence title)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onChildTitleChanged(childActivity, title);
		else
			super.onChildTitleChanged(childActivity, title);
	}
	
	public void onConfigurationChanged(Configuration newConfig)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onDestroy();
		else
			super.onConfigurationChanged(newConfig);
	}
	
	@Override
	public void onContentChanged()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onContentChanged();
		else
			super.onContentChanged();
	}
	
	@Override
	public boolean onContextItemSelected(MenuItem item)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onContextItemSelected(item);
		else
			return super.onContextItemSelected(item);
	}
	
	@Override
	public void onContextMenuClosed(Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onContextMenuClosed(menu);
		else
			super.onContextMenuClosed(menu);
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		if (QtApplication.m_activityListener != null)
		{
			QtApplication.m_activityListener.onCreate(savedInstanceState);
			return;
		}
	    super.onCreate(savedInstanceState);
	    requestWindowFeature(Window.FEATURE_NO_TITLE);
	    try {
			m_activityInfo = getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
			finish();
			return;
		} 
	    if (null == getLastNonConfigurationInstance())
	        startApp(true);
	}
	
	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onCreateContextMenu(menu, v, menuInfo);
		else
			super.onCreateContextMenu(menu, v, menuInfo);
	}
	
	@Override
	public CharSequence onCreateDescription()
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreateDescription();
		else
			return super.onCreateDescription();
	}
	
	@Override
	protected Dialog onCreateDialog(int id)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreateDialog(id);
		else
			return super.onCreateDialog(id);
	}
	
	@Override
	protected Dialog onCreateDialog(int id, Bundle args)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreateDialog(id, args);
		else
			return super.onCreateDialog(id, args);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreateOptionsMenu(menu);
		else
			return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onCreatePanelMenu(int featureId, Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreatePanelMenu(featureId, menu);
		else
			return super.onCreatePanelMenu(featureId, menu);
	}
	
	@Override
	public View onCreatePanelView(int featureId)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreatePanelView(featureId);
		else
			return super.onCreatePanelView(featureId);
	}
	
	@Override
	public boolean onCreateThumbnail(Bitmap outBitmap, Canvas canvas)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreateThumbnail(outBitmap, canvas);
		else
			return super.onCreateThumbnail(outBitmap, canvas);
	}
	
	@Override
	public View onCreateView(String name, Context context, AttributeSet attrs)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onCreateView(name, context, attrs);
		else
			return super.onCreateView(name, context, attrs);
	}
	
	@Override
	protected void onDestroy()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onDestroy();
		else
			super.onDestroy();
	}
	
	@Override
	public void onDetachedFromWindow()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onDetachedFromWindow();
		else
			super.onDetachedFromWindow();
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onKeyDown(keyCode, event);
		else
			return super.onKeyDown(keyCode, event);
	}
	
	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onKeyLongPress(keyCode, event);
		else
			return super.onKeyLongPress(keyCode, event);
	}
	
	@Override
	public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onKeyMultiple(keyCode, repeatCount, event);
		else
			return super.onKeyMultiple(keyCode, repeatCount, event);
	}
	
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onKeyUp(keyCode, event);
		else
			return super.onKeyUp(keyCode, event);
	}
	
	@Override
	public void onLowMemory()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onLowMemory();
		else
			super.onLowMemory();
	}
	
	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onMenuItemSelected(featureId, item);
		else
			return super.onMenuItemSelected(featureId, item);
	}
	
	@Override
	public boolean onMenuOpened(int featureId, Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onMenuOpened(featureId, menu);
		else
			return super.onMenuOpened(featureId, menu);
	}
	
	@Override
	protected void onNewIntent(Intent intent)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onNewIntent(intent);
		else
			super.onNewIntent(intent);
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onOptionsItemSelected(item);
		else
			return super.onOptionsItemSelected(item);
	}
	
	@Override
	public void onOptionsMenuClosed(Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onOptionsMenuClosed(menu);
		else
			super.onOptionsMenuClosed(menu);
	}
	
	@Override
	public void onPanelClosed(int featureId, Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onPanelClosed(featureId, menu);
		else
			super.onPanelClosed(featureId, menu);
	}
	
	@Override
	protected void onPause()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onPause();
		else
			super.onPause();
	}
	
	@Override
	protected void onPostCreate(Bundle savedInstanceState)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onPostCreate(savedInstanceState);
		else
			super.onPostCreate(savedInstanceState);
	}
	
	@Override
	protected void onPostResume()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onPostResume();
		else
			super.onPostResume();
	}
	
	@Override
	protected void onPrepareDialog(int id, Dialog dialog)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onPrepareDialog(id, dialog);
		else
			super.onPrepareDialog(id, dialog);
	}
	
	@Override
	protected void onPrepareDialog(int id, Dialog dialog, Bundle args)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onPrepareDialog(id, dialog, args);
		else
			super.onPrepareDialog(id, dialog, args);
	}
	
	@Override
	public boolean onPrepareOptionsMenu(Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onPrepareOptionsMenu(menu);
		else
			return super.onPrepareOptionsMenu(menu);
	}
	
	@Override
	public boolean onPreparePanel(int featureId, View view, Menu menu)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onPreparePanel(featureId, view, menu);
		else
			return super.onPreparePanel(featureId, view, menu);
	}
	
	@Override
	protected void onRestart()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onRestart();
		else
			super.onRestart();
	}
	
	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onRestoreInstanceState(savedInstanceState);
		else
			super.onRestoreInstanceState(savedInstanceState);
	}
	
	@Override
	protected void onResume()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onResume();
		else
			super.onResume();
	}
	
	@Override
	public Object onRetainNonConfigurationInstance()
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onRetainNonConfigurationInstance();
		else
			return super.onRetainNonConfigurationInstance();
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onSaveInstanceState(outState);
		else
			super.onSaveInstanceState(outState);
	}
	
	@Override
	public boolean onSearchRequested()
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onSearchRequested();
		else
			return super.onSearchRequested();
	}
	
	@Override
	protected void onStart()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onStart();
		else
			super.onStart();
	}
	
	@Override
	protected void onStop()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onStop();
		else
			super.onStop();
	}
	
	@Override
	protected void onTitleChanged(CharSequence title, int color)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onTitleChanged(title, color);
		else
			super.onTitleChanged(title, color);
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onTouchEvent(event);
		else
			return super.onTouchEvent(event);
	}
	
	@Override
	public boolean onTrackballEvent(MotionEvent event)
	{
		if (QtApplication.m_activityListener != null)
			return QtApplication.m_activityListener.onTrackballEvent(event);
		else
			return super.onTrackballEvent(event);
	}
	
	@Override
	public void onUserInteraction()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onUserInteraction();
		else
			super.onUserInteraction();
	}
	
	@Override
	protected void onUserLeaveHint()
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onUserLeaveHint();
		else
			super.onUserLeaveHint();
	}
	
	@Override
	public void onWindowAttributesChanged(LayoutParams params)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onWindowAttributesChanged(params);
		else
			super.onWindowAttributesChanged(params);
	}
	
	@Override
	public void onWindowFocusChanged(boolean hasFocus)
	{
		if (QtApplication.m_activityListener != null)
			QtApplication.m_activityListener.onWindowFocusChanged(hasFocus);
		else
			super.onWindowFocusChanged(hasFocus);
	}
	/////////////////////////// forward all notifications ////////////////////////////


	/////////////////////////// Super class calls ////////////////////////////
	@Override
	public void super_onActivityResult(int requestCode, int resultCode, Intent data)
	{
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	public void super_onApplyThemeResource(Theme theme, int resid, boolean first)
	{
		super.onApplyThemeResource(theme, resid, first);
	}

	@Override
	public void super_onAttachedToWindow()
	{
		super.onAttachedToWindow();
	}

	@Override
	public void super_onBackPressed()
	{
		super.onBackPressed();
	}

	@Override
	public void super_onChildTitleChanged(Activity childActivity, CharSequence title)
	{
		super.onChildTitleChanged(childActivity, title);
	}

	@Override
	public void super_onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
	}

	@Override
	public void super_onContentChanged()
	{
		super.onContentChanged();
	}

	@Override
	public boolean super_onContextItemSelected(MenuItem item)
	{
		return super.onContextItemSelected(item);
	}

	@Override
	public void super_onContextMenuClosed(Menu menu)
	{
		super.onContextMenuClosed(menu);
	}

	@Override
	public void super_onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
	}

	@Override
	public void super_onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo)
	{
		super.onCreateContextMenu(menu, v, menuInfo);
	}

	@Override
	public CharSequence super_onCreateDescription()
	{
		return super.onCreateDescription();
	}

	@Override
	public Dialog super_onCreateDialog(int id)
	{
		return super.onCreateDialog(id);
	}

	@Override
	public Dialog super_onCreateDialog(int id, Bundle args)
	{
		return super.onCreateDialog(id, args);
	}

	@Override
	public boolean super_onCreateOptionsMenu(Menu menu)
	{
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean super_onCreatePanelMenu(int featureId, Menu menu)
	{
		return super.onCreatePanelMenu(featureId, menu);
	}

	@Override
	public View super_onCreatePanelView(int featureId)
	{
		return super.onCreatePanelView(featureId);
	}

	@Override
	public boolean super_onCreateThumbnail(Bitmap outBitmap, Canvas canvas)
	{
		return super.onCreateThumbnail(outBitmap, canvas);
	}

	@Override
	public View super_onCreateView(String name, Context context, AttributeSet attrs)
	{
		return super.onCreateView(name, context, attrs);
	}

	@Override
	public void super_onDestroy()
	{
		super.onDestroy();
	}

	@Override
	public void super_onDetachedFromWindow()
	{
		super.onDetachedFromWindow();
	}

	@Override
	public boolean super_onKeyDown(int keyCode, KeyEvent event)
	{
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean super_onKeyLongPress(int keyCode, KeyEvent event)
	{
		return super.onKeyLongPress(keyCode, event);
	}

	@Override
	public boolean super_onKeyMultiple(int keyCode, int repeatCount, KeyEvent event)
	{
		return super.onKeyMultiple(keyCode, repeatCount, event);
	}

	@Override
	public boolean super_onKeyUp(int keyCode, KeyEvent event)
	{
		return super.onKeyUp(keyCode, event);
	}

	@Override
	public void super_onLowMemory()
	{
		super.onLowMemory();
	}

	@Override
	public boolean super_onMenuItemSelected(int featureId, MenuItem item)
	{
		return super.onMenuItemSelected(featureId, item);
	}

	@Override
	public boolean super_onMenuOpened(int featureId, Menu menu)
	{
		return super.onMenuOpened(featureId, menu);
	}

	@Override
	public void super_onNewIntent(Intent intent)
	{
		super.onNewIntent(intent);
	}

	@Override
	public boolean super_onOptionsItemSelected(MenuItem item)
	{
		return super.onOptionsItemSelected(item);
	}

	@Override
	public void super_onOptionsMenuClosed(Menu menu)
	{
		super.onOptionsMenuClosed(menu);
	}

	@Override
	public void super_onPanelClosed(int featureId, Menu menu)
	{
		super.onPanelClosed(featureId, menu);
	}

	@Override
	public void super_onPause()
	{
		super.onPause();
	}

	@Override
	public void super_onPostCreate(Bundle savedInstanceState)
	{
		super.onPostCreate(savedInstanceState);
	}

	@Override
	public void super_onPostResume()
	{
		super.onPostResume();
	}

	@Override
	public void super_onPrepareDialog(int id, Dialog dialog)
	{
		super.onPrepareDialog(id, dialog);
	}

	@Override
	public void super_onPrepareDialog(int id, Dialog dialog, Bundle args)
	{
		super.onPrepareDialog(id, dialog, args);
	}

	@Override
	public boolean super_onPrepareOptionsMenu(Menu menu)
	{
		return super.onPrepareOptionsMenu(menu);
	}

	@Override
	public boolean super_onPreparePanel(int featureId, View view, Menu menu)
	{
		return super.onPreparePanel(featureId, view, menu);
	}

	@Override
	public void super_onRestart()
	{		
		super.onRestart();
	}

	@Override
	public void super_onRestoreInstanceState(Bundle savedInstanceState)
	{
		super.onRestoreInstanceState(savedInstanceState);
	}

	@Override
	public void super_onResume()
	{
		super.onResume();
	}

	@Override
	public Object super_onRetainNonConfigurationInstance()
	{
		return super.onRetainNonConfigurationInstance();
	}

	@Override
	public void super_onSaveInstanceState(Bundle outState)
	{
		super.onSaveInstanceState(outState);
		
	}

	@Override
	public boolean super_onSearchRequested()
	{
		return super.onSearchRequested();
	}

	@Override
	public void super_onStart()
	{
		super.onStart();
	}

	@Override
	public void super_onStop()
	{
		super.onStop();
	}

	@Override
	public void super_onTitleChanged(CharSequence title, int color)
	{
		super.onTitleChanged(title, color);
	}

	@Override
	public boolean super_onTouchEvent(MotionEvent event)
	{
		return super.onTouchEvent(event);
	}

	@Override
	public boolean super_onTrackballEvent(MotionEvent event)
	{
		return super.onTrackballEvent(event);
	}

	@Override
	public void super_onUserInteraction()
	{
		super.onUserInteraction();
	}

	@Override
	public void super_onUserLeaveHint()
	{
		super.onUserLeaveHint();
	}

	@Override
	public void super_onWindowAttributesChanged(LayoutParams params)
	{
		super.onWindowAttributesChanged(params);
	}

	@Override
	public void super_onWindowFocusChanged(boolean hasFocus)
	{
		super.onWindowFocusChanged(hasFocus);
	}
	/////////////////////////// Super class calls ////////////////////////////
}
