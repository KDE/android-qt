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

package eu.licentia.necessitas.industrius;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.method.MetaKeyKeyListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import eu.licentia.necessitas.ministro.IMinistro;
import eu.licentia.necessitas.ministro.IMinistroCallback;

public class QtActivity extends Activity {

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
        QtApplication.loadQtLibraries(libs);
        try
        {
            ActivityInfo ai=getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
            if (ai.metaData.containsKey("android.app.bundled_libs_resource_id"))
            {
                // Load bundle libs
                int resourceId = ai.metaData.getInt("android.app.bundled_libs_resource_id");
                QtApplication.loadBundledLibraries(getResources().getStringArray(resourceId));
            }

            if (ai.metaData.containsKey("android.app.lib_name")) // Load application
                QtApplication.loadBundledLibraries(new String[]{ai.metaData.getString("android.app.lib_name")});

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
                        Log.e(QtApplication.QtTAG,"Can't start debugging"+ioe.getMessage());
                }
                catch (SecurityException se)
                {
                        Log.e(QtApplication.QtTAG,"Can't start debugging"+se.getMessage());
                }
            }
            // start application

            if (ai.metaData.containsKey("android.app.application_startup_params"))
            {
                if (params.length()>0)
                    params+="\t";
                params += ai.metaData.getString("android.app.application_startup_params");
            }

            final String homePath="HOME="+getFilesDir().getAbsolutePath()+"\tTMPDIR="+getFilesDir().getAbsolutePath();
            if (environment != null && environment.length()>0)
                environment=homePath+"\t"+environment;
            else
                environment=homePath;

            QtApplication.startApplication(params, environment);
            m_surface.applicationStared();
            m_started = true;
        }
        catch (NameNotFoundException e)
        {
            Log.e(QtApplication.QtTAG, "Can't package metadata", e);
        }
    }

    private IMinistroCallback m_ministroCallback = new IMinistroCallback.Stub() {
        @Override
        public void libs(final String[] libs, final String evnVars, final String params,
                         int errorCode, String errorMessage) throws RemoteException {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    startApplication(libs, evnVars, params);
                }
            });
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
                m_ministroCallback.libs(libs,"QT_IMPORT_PATH=/data/local/qt/imports\tQT_PLUGIN_PATH=/data/local/qt/plugins", "-platform\tandroid", 0,null);
                return;
            }

            try {
                if (!bindService(new Intent(eu.licentia.necessitas.ministro.IMinistro.class.getCanonicalName()), m_ministroConnection, Context.BIND_AUTO_CREATE))
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
            Log.e(QtApplication.QtTAG, "Can't create main activity", e);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode == MINISTRO_INSTALL_REQUEST_CODE)
                startApp(false);
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        m_quitApp = true;
        QtApplication.setMainActivity(this);
        if (null == getLastNonConfigurationInstance())
        {
            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(metrics);
            QtApplication.setApplicationDisplayMetrics(metrics.widthPixels, metrics.heightPixels,
                            metrics.widthPixels, metrics.heightPixels,
                            metrics.xdpi, metrics.ydpi);
        }
        requestWindowFeature(Window.FEATURE_NO_TITLE);
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
        QtApplication.setMainActivity(null);
        super.onDestroy();
        if (m_quitApp)
        {
            Log.i(QtApplication.QtTAG, "onDestroy");
            if (m_debuggerProcess != null)
                m_debuggerProcess.destroy();
            System.exit(0);// FIXME remove it or find a better way
        }
        QtApplication.setMainActivity(null);
    }

    @Override
    protected void onResume()
    {
        // fire all lostActions
        synchronized (QtApplication.m_mainActivityMutex)
        {
            Iterator<Runnable> itr=QtApplication.getLostActions().iterator();
            while(itr.hasNext())
                runOnUiThread(itr.next());
                if (m_started)
                {
                    QtApplication.clearLostActions();
                    QtApplication.updateWindow();
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
            m_surface.applicationStared();
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
            Log.i(QtApplication.QtTAG, "dispatchKeyEvent at MULTIPLE with one character: "+event.getCharacters());
            QtApplication.keyDown(0, event.getCharacters().charAt(0), event.getMetaState());
            QtApplication.keyUp(0, event.getCharacters().charAt(0), event.getMetaState());
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
                QtApplication.keyDown(keyCode, c, event.getMetaState());
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (!m_started)
            return false;
        m_metaState = MetaKeyKeyListener.handleKeyUp(m_metaState, keyCode, event);
        QtApplication.keyUp(keyCode, event.getUnicodeChar(), event.getMetaState());
        return true;
    }

    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }
}
