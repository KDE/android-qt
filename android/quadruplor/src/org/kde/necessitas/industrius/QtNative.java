/*
    Copyright (c) 2011, BogDan Vatra <bog_dan_ro@yahoo.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package org.kde.necessitas.industrius;

import java.io.File;
import java.util.ArrayList;

import android.app.Activity;
import android.app.Application;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.view.Menu;
import android.view.MotionEvent;

public class QtNative extends Application
{
    private static QtActivity m_mainActivity = null;
    private static QtSurface m_mainView = null;
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

    public static ClassLoader classLoader()
    {
        return m_mainActivity.getClassLoader();
    }

    public static Activity activity()
    {
        return m_mainActivity;
    }

    public static QtSurface mainView()
    {
        return m_mainView;
    }

    public static void openURL(String url)
    {
        Uri uri = Uri.parse(url);
        Intent intent = new Intent(Intent.ACTION_VIEW, uri);
        activity().startActivity(intent);
    }

    // this method loads full path libs
    public static void loadQtLibraries(String[] libraries)
    {
        if (libraries == null)
            return;

        for (int i = 0; i < libraries.length; i++)
        {
            try
            {
                File f = new File(libraries[i]);
                if (f.exists())
                    System.load(libraries[i]);
            }
            catch (SecurityException e)
            {
                Log.i(QtTAG, "Can't load '" + libraries[i] + "'", e);
            }
            catch (Exception e)
            {
                Log.i(QtTAG, "Can't load '" + libraries[i] + "'", e);
            }
        }
    }

        // this method loads bundled libs by name.
    public static void loadBundledLibraries(String[] libraries)
    {
        for (int i = 0; i < libraries.length; i++)
        {
            try
            {
                System.loadLibrary(libraries[i]);
            }
            catch (UnsatisfiedLinkError e)
            {
                Log.i(QtTAG, "Can't load '" + libraries[i] + "'", e);
            }
            catch (SecurityException e)
            {
                Log.i(QtTAG, "Can't load '" + libraries[i] + "'", e);
            }
            catch (Exception e)
            {
                Log.i(QtTAG, "Can't load '" + libraries[i] + "'", e);
            }
        }
    }

    public static void setMainActivity(QtActivity qtMainActivity)
    {
        synchronized (m_mainActivityMutex)
        {
            m_mainActivity = qtMainActivity;
        }
    }
    public static void setMainView(QtSurface qtSurface)
    {
        synchronized (m_mainActivityMutex)
        {
            m_mainView = qtSurface;
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
            if (m_mainActivity == null)
                m_lostActions.add(action);
            else
                m_mainActivity.runOnUiThread(action);
            return m_mainActivity != null;
        }
    }

    public static boolean startApplication(String params, String environment, String mainLibrary, String nativeLibraryDir) throws Exception
    {
        File f = new File(nativeLibraryDir+"lib"+mainLibrary+".so");
        if (!f.exists())
            throw new Exception("Can't find main library '" + mainLibrary + "'");

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
            startQtApplication(f.getAbsolutePath()+"\t"+params, environment);
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
    public static native void startQtApplication(String params, String env);
    public static native void pauseQtApp();
    public static native void resumeQtApp();
    public static native boolean startQtAndroidPlugin();
    public static native void quitQtAndroidPlugin();
    public static native void terminateQt();
    // application methods

    private static void quitApp()
    {
        m_mainActivity.finish();
    }

    private static void redrawSurface(final int left, final int top, final int right, final int bottom )
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_mainActivity.redrawWindow(left, top, right, bottom);
            }
        });
    }

    @Override
    public void onTerminate() {
        if (m_started)
            terminateQt();
        super.onTerminate();
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


    private static void showSoftwareKeyboard(final int x, final int y
                                        , final int width, final int height
                                        , final int inputHints )
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_mainActivity.showSoftwareKeyboard();
            }
        });
    }

    private static void resetSoftwareKeyboard()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_mainActivity.resetSoftwareKeyboard();
            }
        });
    }

    private static void hideSoftwareKeyboard()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_mainActivity.hideSoftwareKeyboard();
            }
        });
    }

    private static void setFullScreen(final boolean fullScreen)
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                m_mainActivity.setFullScreen(fullScreen);
                updateWindow();
            }
        });
    }

    // screen methods
    public static native void setDisplayMetrics(int screenWidthPixels,
                    int screenHeightPixels, int desktopWidthPixels,
                    int desktopHeightPixels, double XDpi, double YDpi);
    // screen methods

    private static void showOptionsMenu()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                if (m_mainActivity != null)
                    m_mainActivity.openOptionsMenu();
            }
        });
    }

    private static void hideOptionsMenu()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                if (m_mainActivity != null)
                    m_mainActivity.closeOptionsMenu();
            }
        });
    }

    private static void showContextMenu()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                if (m_mainActivity != null)
                    m_mainActivity.openContextMenu(m_mainView);
            }
        });
    }

    private static void hideContextMenu()
    {
        runAction(new Runnable() {
            @Override
            public void run() {
                if (m_mainActivity != null)
                    m_mainActivity.closeContextMenu();
            }
        });
    }

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

    // options menu methods
    public static native void createOptionsMenu(Menu menu);
    public static native boolean prepareOptionsMenu(Menu menu);
    public static native boolean optionsItemSelected(int groupId, int itemId);
    // options menu methods
}
