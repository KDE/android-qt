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
import java.util.Iterator;

import org.kde.necessitas.interfaces.QtActivityDelegateInterface11;
import org.kde.necessitas.interfaces.QtActivityDelegateInterface12;
import org.kde.necessitas.interfaces.QtActivityDelegateInterface4;
import org.kde.necessitas.interfaces.QtActivityDelegateInterface5;
import org.kde.necessitas.interfaces.QtActivityDelegateInterface8;
import org.kde.necessitas.interfaces.QtActivityInterface;
import org.kde.necessitas.interfaces.QtActivitySuperInterface11;
import org.kde.necessitas.interfaces.QtActivitySuperInterface12;
import org.kde.necessitas.interfaces.QtActivitySuperInterface4;
import org.kde.necessitas.interfaces.QtActivitySuperInterface5;
import org.kde.necessitas.interfaces.QtActivitySuperInterface8;
import org.kde.necessitas.interfaces.QtLoaderInterface;
import org.kde.necessitas.interfaces.QtStaticInitDataInterface;

import android.app.Activity;
import android.app.Dialog;
import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.content.res.Resources.Theme;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Bundle;
import android.text.method.MetaKeyKeyListener;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ActionMode;
import android.view.ActionMode.Callback;
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
import android.view.accessibility.AccessibilityEvent;
import android.view.inputmethod.InputMethodManager;

public class QtActivityDelegate implements QtLoaderInterface
                                        , QtActivityDelegateInterface4
                                        , QtActivityDelegateInterface5
                                        , QtActivityDelegateInterface8
                                        , QtActivityDelegateInterface11
                                        , QtActivityDelegateInterface12
{
    private QtActivitySuperInterface4 m_activitySuperInterface4=null;
    private QtActivitySuperInterface5 m_activitySuperInterface5=null;
    private QtActivitySuperInterface8 m_activitySuperInterface8=null;
    private QtActivitySuperInterface11 m_activitySuperInterface11=null;
    private QtActivitySuperInterface12 m_activitySuperInterface12=null;
    private Activity m_activity = null;

    private static final String NATIVE_LIBRARIES_KEY="native.libraries";
    private static final String BUNDLED_LIBRARIES_KEY="bundled.libraries";
    private static final String ENVIRONMENT_VARIABLES_KEY="environment.variables";
    private static final String APPLICATION_PARAMETERS_KEY="application.parameters";
    private static final String STATIC_INIT_CLASSES_KEY="application.parameters";


//    private int m_id=-1;
    private boolean softwareKeyboardIsVisible=false;
    private long m_metaState;
    private int m_lastChar=0;
    private boolean m_fullScreen=false;
    private boolean m_started = false;
    private QtSurface m_surface=null;
    private QtLayout m_layout=null;

    private boolean m_quitApp = true;
    private Process m_debuggerProcess=null; // debugger process


    public QtLayout getQtLayout()
    {
        return m_layout;
    }

    public QtSurface getQtSurface()
    {
        return m_surface;
    }

    public void redrawWindow(int left, int top, int right, int bottom) {
        m_surface.drawBitmap(new Rect(left, top, right, bottom));
    }

    public void setFullScreen(boolean enterFullScreen)
    {
        if (m_fullScreen == enterFullScreen)
            return;
        if (m_fullScreen = enterFullScreen)
            m_activity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        else
            m_activity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
    }

    public void showSoftwareKeyboard()
    {
        softwareKeyboardIsVisible = true;
        InputMethodManager imm = (InputMethodManager)m_activity.getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.toggleSoftInput(InputMethodManager.SHOW_FORCED, InputMethodManager.HIDE_IMPLICIT_ONLY );
    }

    public void hideSoftwareKeyboard()
    {
        if (softwareKeyboardIsVisible)
        {
            InputMethodManager imm = (InputMethodManager)m_activity.getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.toggleSoftInput(0, 0);
        }
        softwareKeyboardIsVisible = false;
    }

    @Override
    public boolean startApplication(QtActivityInterface activityInterface, Bundle loaderParams)
    {
        /// check parameters integrity
        if (!loaderParams.containsKey(NATIVE_LIBRARIES_KEY)
                || !loaderParams.containsKey(BUNDLED_LIBRARIES_KEY)
                || !loaderParams.containsKey(ENVIRONMENT_VARIABLES_KEY)
                || !loaderParams.containsKey(APPLICATION_PARAMETERS_KEY))
            return false;

        activityInterface.setQtActivityDelegate(this);
        m_activity = (Activity) activityInterface;
        QtNative.setActivity(m_activity, this);

        m_activitySuperInterface4 = (QtActivitySuperInterface4) activityInterface;
        m_activitySuperInterface5 = (QtActivitySuperInterface5) activityInterface;;
        m_activitySuperInterface8 = (QtActivitySuperInterface8) activityInterface;;
        m_activitySuperInterface11 = (QtActivitySuperInterface11) activityInterface;;
        m_activitySuperInterface12 = (QtActivitySuperInterface12) activityInterface;;

        if (loaderParams.containsKey(STATIC_INIT_CLASSES_KEY))
            for(String className: loaderParams.getStringArrayList(STATIC_INIT_CLASSES_KEY))
            {
                try {
                    @SuppressWarnings("rawtypes")
                    Class initClass = m_activity.getClassLoader().loadClass(className);
                    QtStaticInitDataInterface staticInitDataObject=(QtStaticInitDataInterface)initClass.newInstance(); // create an instance
                    staticInitDataObject.setActivity(m_activity, this);
                } catch (ClassNotFoundException e) {
                    e.printStackTrace();
                } catch (InstantiationException e) {
                    e.printStackTrace();
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                }
            }

        QtNative.loadQtLibraries(loaderParams.getStringArrayList(NATIVE_LIBRARIES_KEY));
        QtNative.loadBundledLibraries(loaderParams.getStringArrayList(BUNDLED_LIBRARIES_KEY));


        // if the applications is debuggable and it has a native debug request
        if ( /*(ai.flags&ApplicationInfo.FLAG_DEBUGGABLE) != 0
                &&*/ m_activity.getIntent().getExtras() != null
                && m_activity.getIntent().getExtras().containsKey("native_debug")
                && m_activity.getIntent().getExtras().getString("native_debug").equals("true"))
        {
            try
            {
                String packagePath=m_activity.getPackageManager().getApplicationInfo(m_activity.getPackageName(), PackageManager.GET_CONFIGURATIONS).dataDir+"/";
                String gdbserverPath=m_activity.getIntent().getExtras().containsKey("gdbserver_path")?m_activity.getIntent().getExtras().getString("gdbserver_path"):packagePath+"lib/gdbserver ";
                String socket=m_activity.getIntent().getExtras().containsKey("gdbserver_socket")?m_activity.getIntent().getExtras().getString("gdbserver_socket"):"+debug-socket";
                // start debugger
                m_debuggerProcess =Runtime.getRuntime().exec(gdbserverPath+socket+" --attach "+android.os.Process.myPid(),null, new File(packagePath));
            }
            catch (IOException ioe)
            {
                Log.e(QtNative.QtTAG,"Can't start debugger"+ioe.getMessage());
            }
            catch (SecurityException se)
            {
                Log.e(QtNative.QtTAG,"Can't start debugger"+se.getMessage());
            } catch (NameNotFoundException e) {
                Log.e(QtNative.QtTAG,"Can't start debugger"+e.getMessage());
            }
        }
        // start application
        m_surface.applicationStared( QtNative.startApplication(loaderParams.getString("APPLICATION_PARAMETERS_KEY")
                                                            , loaderParams.getString("ENVIRONMENT_VARIABLES_KEY")));
        m_started = true;
        return true;
    }

    @Override
    public void onTerminate()
    {
        QtNative.terminateQt();
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        m_activitySuperInterface4.super_onCreate(savedInstanceState);
        m_activity.requestWindowFeature(Window.FEATURE_NO_TITLE);
        m_quitApp = true;
//        QtNative.setMainActivity(this);
//        if (null == getLastNonConfigurationInstance())
//        {
//            DisplayMetrics metrics = new DisplayMetrics();
//            getWindowManager().getDefaultDisplay().getMetrics(metrics);
//            QtNative.setApplicationDisplayMetrics(metrics.widthPixels, metrics.heightPixels,
//                            metrics.widthPixels, metrics.heightPixels,
//                            metrics.xdpi, metrics.ydpi);
//        }
//        // requestWindowFeature(Window.FEATURE_NO_TITLE);
//        m_layout=new QtLayout(this);
//        m_surface = new QtSurface(this, m_id);
//        m_layout.addView(m_surface,0);
//        setContentView(m_layout);
//        m_layout.bringChildToFront(m_surface);
//        if (null == getLastNonConfigurationInstance())
//            startApp(true);
    }

    @Override
    public void onDestroy()
    {
//        QtNative.setMainActivity(null);
        m_activitySuperInterface4.super_onDestroy();
        if (m_quitApp)
        {
            Log.i(QtNative.QtTAG, "onDestroy");
            if (m_debuggerProcess != null)
                m_debuggerProcess.destroy();
            System.exit(0);// FIXME remove it or find a better way
        }
//        QtNative.setMainActivity(null);
    }

    @Override
    public void onRestoreInstanceState(Bundle savedInstanceState)
    {
        m_activitySuperInterface4.super_onRestoreInstanceState(savedInstanceState);
//        setFullScreen(savedInstanceState.getBoolean("FullScreen"));
        m_started = savedInstanceState.getBoolean("Started");
        if (m_started)
            m_surface.applicationStared( true );
    }

    @Override
    public void onResume()
    {
        // fire all lostActions
        synchronized (QtNative.m_mainActivityMutex)
        {
            Iterator<Runnable> itr=QtNative.getLostActions().iterator();
            while(itr.hasNext())
                m_activity.runOnUiThread(itr.next());
            if (m_started)
            {
                QtNative.clearLostActions();
                QtNative.updateWindow();
            }
        }
        m_activitySuperInterface4.super_onResume();
    }

    @Override
    public Object onRetainNonConfigurationInstance()
    {
        m_activitySuperInterface4.super_onRetainNonConfigurationInstance();
        m_quitApp = false;
        return true;
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        m_activitySuperInterface4.super_onSaveInstanceState(outState);
        outState.putBoolean("FullScreen",m_fullScreen);
        outState.putBoolean("Started", m_started);
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
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (m_started && event.getAction() == KeyEvent.ACTION_MULTIPLE &&
            event.getCharacters() != null &&
            event.getCharacters().length() == 1 &&
            event.getKeyCode() == 0) {
            Log.i(QtNative.QtTAG, "dispatchKeyEvent at MULTIPLE with one character: "+event.getCharacters());
            QtNative.keyDown(0, event.getCharacters().charAt(0), event.getMetaState());
            QtNative.keyUp(0, event.getCharacters().charAt(0), event.getMetaState());
        }

        return m_activitySuperInterface4.super_dispatchKeyEvent(event);
    }










    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        m_activitySuperInterface4.super_onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onApplyThemeResource(Theme theme, int resid, boolean first)
    {
        m_activitySuperInterface4.super_onApplyThemeResource(theme, resid, first);
    }

    @Override
    public void onChildTitleChanged(Activity childActivity, CharSequence title)
    {
        m_activitySuperInterface4.super_onChildTitleChanged(childActivity, title);
    }

    @Override
    public boolean onContextItemSelected(MenuItem item)
    {
        return m_activitySuperInterface4.super_onContextItemSelected(item);
    }

    @Override
    public void onContextMenuClosed(Menu menu)
    {
        m_activitySuperInterface4.super_onContextMenuClosed(menu);
    }

    @Override
    public CharSequence onCreateDescription()
    {
        return m_activitySuperInterface4.super_onCreateDescription();
    }

    @Override
    public Dialog onCreateDialog(int id)
    {
        return m_activitySuperInterface4.super_onCreateDialog(id);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        return m_activitySuperInterface4.super_onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onCreateThumbnail(Bitmap outBitmap, Canvas canvas)
    {
        return m_activitySuperInterface4.super_onCreateThumbnail(outBitmap, canvas);
    }

    @Override
    public void onNewIntent(Intent intent)
    {
        m_activitySuperInterface4.super_onNewIntent(intent);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        return m_activitySuperInterface4.super_onOptionsItemSelected(item);
    }

    @Override
    public void onOptionsMenuClosed(Menu menu)
    {
        m_activitySuperInterface4.super_onOptionsMenuClosed(menu);
    }

    @Override
    public void onPause()
    {
        m_activitySuperInterface4.super_onPause();
    }

    @Override
    public void onPostCreate(Bundle savedInstanceState)
    {
        m_activitySuperInterface4.super_onPostCreate(savedInstanceState);
    }

    @Override
    public void onPostResume()
    {
        m_activitySuperInterface4.super_onPostResume();
    }

    @Override
    public void onPrepareDialog(int id, Dialog dialog)
    {
        m_activitySuperInterface4.super_onPrepareDialog(id, dialog);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu)
    {
        return m_activitySuperInterface4.super_onPrepareOptionsMenu(menu);
    }

    @Override
    public void onRestart()
    {
        m_activitySuperInterface4.super_onRestart();
    }

    @Override
    public void onStart()
    {
        m_activitySuperInterface4.super_onStart();
    }

    @Override
    public void onStop()
    {
        m_activitySuperInterface4.super_onStop();
    }

    @Override
    public void onTitleChanged(CharSequence title, int color)
    {
        m_activitySuperInterface4.super_onTitleChanged(title, color);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        return m_activitySuperInterface4.super_onTouchEvent(event);
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event)
    {
        return m_activitySuperInterface4.super_onTrackballEvent(event);
    }

    @Override
    public void onUserInteraction()
    {
        m_activitySuperInterface4.super_onUserInteraction();
    }

    @Override
    public void onUserLeaveHint()
    {
        m_activitySuperInterface4.super_onUserLeaveHint();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {
        m_activitySuperInterface4.super_onConfigurationChanged(newConfig);
    }

    @Override
    public void onLowMemory()
    {
        m_activitySuperInterface4.super_onLowMemory();
    }

    @Override
    public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event)
    {
        return m_activitySuperInterface4.super_onKeyMultiple(keyCode, repeatCount, event);
    }

    @Override
    public View onCreateView(String name, Context context, AttributeSet attrs)
    {
        return m_activitySuperInterface4.super_onCreateView(name, context, attrs);
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo)
    {
        m_activitySuperInterface4.super_onCreateContextMenu(menu, v, menuInfo);
    }

    @Override
    public boolean dispatchPopulateAccessibilityEvent(AccessibilityEvent event)
    {
        return m_activitySuperInterface4.super_dispatchPopulateAccessibilityEvent(event);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event)
    {
        return m_activitySuperInterface4.super_dispatchTouchEvent(event);
    }

    @Override
    public boolean dispatchTrackballEvent(MotionEvent event)
    {
        return m_activitySuperInterface4.super_dispatchTrackballEvent(event);
    }

    @Override
    public void onContentChanged()
    {
        m_activitySuperInterface4.super_onContentChanged();
    }

    @Override
    public boolean onCreatePanelMenu(int featureId, Menu menu)
    {
        return m_activitySuperInterface4.super_onCreatePanelMenu(featureId, menu);
    }

    @Override
    public View onCreatePanelView(int featureId)
    {
        return m_activitySuperInterface4.super_onCreatePanelView(featureId);
    }

    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item)
    {
        return m_activitySuperInterface4.super_onMenuItemSelected(featureId, item);
    }

    @Override
    public boolean onMenuOpened(int featureId, Menu menu)
    {
        return m_activitySuperInterface4.super_onMenuOpened(featureId, menu);
    }

    @Override
    public void onPanelClosed(int featureId, Menu menu)
    {
        m_activitySuperInterface4.super_onPanelClosed(featureId, menu);
    }

    @Override
    public boolean onPreparePanel(int featureId, View view, Menu menu)
    {
        return m_activitySuperInterface4.super_onPreparePanel(featureId, view, menu);
    }

    @Override
    public boolean onSearchRequested()
    {
        return m_activitySuperInterface4.super_onSearchRequested();
    }

    @Override
    public void onWindowAttributesChanged(LayoutParams params)
    {
        m_activitySuperInterface4.super_onWindowAttributesChanged(params);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        m_activitySuperInterface4.super_onWindowFocusChanged(hasFocus);
    }



// Activity API 5

    @Override
    public void onAttachedToWindow()
    {
        m_activitySuperInterface5.super_onAttachedToWindow();
    }

    @Override
    public void onBackPressed()
    {
        m_activitySuperInterface5.super_onBackPressed();
    }

    @Override
    public void onDetachedFromWindow()
    {
        m_activitySuperInterface5.super_onDetachedFromWindow();
    }

    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event)
    {
        return m_activitySuperInterface5.super_onKeyLongPress(keyCode, event);
    }
// Activity API 8

    @Override
    public Dialog onCreateDialog(int id, Bundle args)
    {
        return m_activitySuperInterface8.super_onCreateDialog(id, args);
    }

    @Override
    public void onPrepareDialog(int id, Dialog dialog, Bundle args)
    {
        m_activitySuperInterface8.super_onPrepareDialog(id, dialog, args);
    }

// Activity API 11
    @Override
    public boolean dispatchKeyShortcutEvent(KeyEvent event)
    {
        return m_activitySuperInterface11.super_dispatchKeyShortcutEvent(event);
    }

    @Override
    public void onActionModeFinished(ActionMode mode)
    {
        m_activitySuperInterface11.super_onActionModeFinished(mode);
    }

    @Override
    public void onActionModeStarted(ActionMode mode)
    {
        m_activitySuperInterface11.super_onActionModeStarted(mode);
    }

    @Override
    public void onAttachFragment(Fragment fragment)
    {
        m_activitySuperInterface11.super_onAttachFragment(fragment);
    }

    @Override
    public View onCreateView(View parent, String name, Context context, AttributeSet attrs)
    {
        return m_activitySuperInterface11.super_onCreateView(parent, name, context, attrs);
    }

    @Override
    public boolean onKeyShortcut(int keyCode, KeyEvent event)
    {
        return m_activitySuperInterface11.super_onKeyShortcut(keyCode, event);
    }

    @Override
    public ActionMode onWindowStartingActionMode(Callback callback)
    {
        return m_activitySuperInterface11.super_onWindowStartingActionMode(callback);
    }
// Activity API 12

    @Override
    public boolean dispatchGenericMotionEvent(MotionEvent event)
    {
        return m_activitySuperInterface12.super_dispatchGenericMotionEvent(event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event)
    {
        return m_activitySuperInterface12.super_onGenericMotionEvent(event);
    }


}
