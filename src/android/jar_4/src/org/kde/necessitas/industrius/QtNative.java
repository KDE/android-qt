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
import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.view.MotionEvent;

public class QtNative
{
    private static Activity m_activity = null;
    private static QtActivityDelegate m_activityDelegate=null;
    public static Object m_mainActivityMutex = new Object(); // mutex used to synchronize runnable operations

    public static final String QtTAG = "Qt JAVA"; // string used for Log.x
    private static ArrayList<Runnable> m_lostActions = new ArrayList<Runnable>(); // a list containing all actions which could not be performed (e.g. the main activity is destroyed, etc.)
    private static boolean m_started = false;
    private static int m_displayMetricsScreenWidthPixels = 0;
    private static int m_displayMetricsScreenHeightPixels = 0;
    private static int m_displayMetricsDesktopWidthPixels = 0;
    private static int m_displayMetricsDesktopHeightPixels = 0;
    private static double m_displayMetricsXDpi = .0;
    private static double m_displayMetricsYDpi = .0;
    private static int m_oldx, m_oldy;
    private static final int m_moveThreshold = 0;


    public static Activity activity()
    {
        synchronized (m_mainActivityMutex) {
            return m_activity;
        }
    }

    public static QtActivityDelegate activityDelegate()
    {
        synchronized (m_mainActivityMutex) {
            return m_activityDelegate;
        }
    }

    public static void openURL(String url)
    {
        Uri uri = Uri.parse(url);
        Intent intent = new Intent(Intent.ACTION_VIEW, uri);
        activity().startActivity(intent);
    }

    // this method loads full path libs
    public static void loadQtLibraries(ArrayList<String> libraries)
    {
        if (libraries == null)
            return;

        for (String libName:libraries)
        {
            try
            {
                File f = new File(libName);
                if (f.exists())
                    System.load(libName);
            }
            catch (SecurityException e)
            {
                Log.i(QtTAG, "Can't load '" + libName + "'", e);
            }
            catch (Exception e)
            {
                Log.i(QtTAG, "Can't load '" + libName + "'", e);
            }
        }
    }

        // this method loads bundled libs by name.
    public static void loadBundledLibraries(ArrayList<String> libraries, String nativeLibraryDir)
    {
        if (libraries == null)
            return;

        for (String libName:libraries)
        {
            try
            {
                File f = new File(nativeLibraryDir+"lib"+libName+".so");
                if (f.exists())
                    System.load(f.getAbsolutePath());
                else
                    Log.i(QtTAG, "Can't find '" + f.getAbsolutePath());
            }
            catch (Exception e)
            {
                Log.i(QtTAG, "Can't load '" + libName + "'", e);
            }
        }
    }

    public static void setActivity(Activity qtMainActivity, QtActivityDelegate qtActivityDelegate)
    {
        synchronized (m_mainActivityMutex)
        {
            m_activity = qtMainActivity;
            m_activityDelegate = qtActivityDelegate;
        }
    }

    static public ArrayList<Runnable> getLostActions()
    {
        return m_lostActions;
    }

    static public void clearLostActions()
    {
        m_lostActions.clear();
    }

    private static boolean runAction(Runnable action)
    {
        synchronized (m_mainActivityMutex)
        {
            if (m_activity == null)
                m_lostActions.add(action);
            else
                m_activity.runOnUiThread(action);
            return m_activity != null;
        }
    }

    public static boolean startApplication(String params, String environment)
    {
        if (params == null)
            params = "-platform\tandroid";
        boolean res=false;
        synchronized (m_mainActivityMutex)
        {
            res=startQtAndroidPlugin();
            setDisplayMetrics(m_displayMetricsScreenWidthPixels,
                            m_displayMetricsScreenHeightPixels,
                            m_displayMetricsDesktopWidthPixels,
                            m_displayMetricsDesktopHeightPixels,
                            m_displayMetricsXDpi,
                            m_displayMetricsYDpi);
            if (params.length()>0)
                params="\t"+params;
            startQtApp("QtApp"+params, environment);
            m_started=true;
        }
        return res;
    }

    public static void setApplicationDisplayMetrics(int screenWidthPixels,
            int screenHeightPixels, int desktopWidthPixels,
            int desktopHeightPixels, double XDpi, double YDpi)
    {
        /* Fix buggy dpi report */
        if (XDpi<android.util.DisplayMetrics.DENSITY_LOW)
            XDpi=android.util.DisplayMetrics.DENSITY_LOW;
        if (YDpi<android.util.DisplayMetrics.DENSITY_LOW)
            YDpi=android.util.DisplayMetrics.DENSITY_LOW;

        synchronized (m_mainActivityMutex)
        {
            if (m_started)
                setDisplayMetrics(screenWidthPixels, screenHeightPixels, desktopWidthPixels, desktopHeightPixels, XDpi, YDpi);
            else
            {
                m_displayMetricsScreenWidthPixels = screenWidthPixels;
                m_displayMetricsScreenHeightPixels = screenHeightPixels;
                m_displayMetricsDesktopWidthPixels = desktopWidthPixels;
                m_displayMetricsDesktopHeightPixels = desktopHeightPixels;
                m_displayMetricsXDpi = XDpi;
                m_displayMetricsYDpi = YDpi;
            }
        }
    }

    public static void pauseApplication()
    {
        synchronized (m_mainActivityMutex)
        {
            if (m_started)
                pauseQtApp();
        }
    }

    public static void resumeApplication()
    {
        synchronized (m_mainActivityMutex)
        {
            if (m_started)
            {
                resumeQtApp();
                updateWindow();
            }
        }
    }
    // application methods
    public static native void startQtApp(String params, String env);
    public static native void pauseQtApp();
    public static native void resumeQtApp();
    public static native boolean startQtAndroidPlugin();
    public static native void quitQtAndroidPlugin();
    public static native void terminateQt();
    // application methods

    private static void quitApp()
    {
        m_activity.finish();
    }

    private static void redrawSurface(final int left, final int top, final int right, final int bottom )
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_activityDelegate.redrawWindow(left, top, right, bottom);
            }
        });
    }

    static public void sendTouchEvent(MotionEvent event, int id)
    {
        switch (event.getAction())
        {
            case MotionEvent.ACTION_UP:
                mouseUp(id,(int) event.getX(), (int) event.getY());
                break;

            case MotionEvent.ACTION_DOWN:
                mouseDown(id,(int) event.getX(), (int) event.getY());
                m_oldx = (int) event.getX();
                m_oldy = (int) event.getY();
                break;

            case MotionEvent.ACTION_MOVE:
                int dx = (int) (event.getX() - m_oldx);
                int dy = (int) (event.getY() - m_oldy);
                if (Math.abs(dx) > m_moveThreshold || Math.abs(dy) > m_moveThreshold)
                {
                    mouseMove(id,(int) event.getX(), (int) event.getY());
                    m_oldx = (int) event.getX();
                    m_oldy = (int) event.getY();
                }
                break;
        }
    }

    static public void sendTrackballEvent(MotionEvent event, int id)
    {
        switch (event.getAction())
        {
            case MotionEvent.ACTION_UP:
                mouseUp(id, (int) event.getX(), (int) event.getY());
                break;

            case MotionEvent.ACTION_DOWN:
                mouseDown(id, (int) event.getX(), (int) event.getY());
                m_oldx = (int) event.getX();
                m_oldy = (int) event.getY();
                break;

            case MotionEvent.ACTION_MOVE:
                int dx = (int) (event.getX() - m_oldx);
                int dy = (int) (event.getY() - m_oldy);
                if (Math.abs(dx) > 5 || Math.abs(dy) > 5)
                {
                        mouseMove(id, (int) event.getX(), (int) event.getY());
                        m_oldx = (int) event.getX();
                        m_oldy = (int) event.getY();
                }
                break;
        }
    }


    private static void showSoftwareKeyboard()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_activityDelegate.showSoftwareKeyboard();
            }
        });
    }

    private static void hideSoftwareKeyboard()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_activityDelegate.hideSoftwareKeyboard();
            }
        });
    }

    private static void setFullScreen(final boolean fullScreen)
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_activityDelegate.setFullScreen(fullScreen);
                updateWindow();
            }
        });
    }

    // screen methods
    public static native void setDisplayMetrics(int screenWidthPixels,
                    int screenHeightPixels, int desktopWidthPixels,
                    int desktopHeightPixels, double XDpi, double YDpi);
    // screen methods

    // pointer methods
    public static native void mouseDown(int winId, int x, int y);
    public static native void mouseUp(int winId, int x, int y);
    public static native void mouseMove(int winId, int x, int y);
    public static native void touchBegin(int winId);
    public static native void touchAdd(int winId, int pointerId, int action, boolean primary, int x, int y, float size, float pressure);
    public static native void touchEnd(int winId, int action);
    // pointer methods

    // keyboard methods
    public static native void keyDown(int key, int unicode, int modifier);
    public static native void keyUp(int key, int unicode, int modifier);
    // keyboard methods

    // surface methods
    public static native void destroySurface();
    public static native void setSurface(Object surface);
    public static native void lockSurface();
    public static native void unlockSurface();
    // surface methods

    // window methods
    public static native void updateWindow();
    // window methods
}
