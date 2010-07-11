package com.nokia.qt;

import java.io.File;

import android.app.Activity;
import android.graphics.Rect;
import android.util.Log;
import android.view.View;

public class QtApplication
{
	public static final String QtTAG = "Qt JAVA";
	private static Activity m_activity = null;
	private static QtMainView m_view = null;
	private static QtEgl mEgl = null;
	
	public static QtEgl getEgl()
    {
        return mEgl;
    }
	
	public static void setActivity(Activity mActivity)
	{
		m_activity = mActivity;
	}

	public static QtMainView getView()
	{
		return m_view;
	}
	
	public static void setView(QtMainView view)
	{
		m_view = view;
	}

	public static void loadLibraries(String[] libraries)
	{
		for (int i = 0; i < libraries.length; i++)
		{
			try
			{
				String library = "/data/local/qt/lib/lib" + libraries[i] + ".so";
				File f = new File(library);
				if (f.exists())
					System.load(library);
			}
			catch (Exception e)
			{
				Log.i(QtTAG, "Can't load '/data/local/qt/lib/lib"
						+ libraries[i] + ".so'", e);
			}
		}
	}

	public static void loadApplication(String lib)
	{
		try
		{
			File f = new File("/data/local/lib/lib" + lib + ".so");
			if (f.exists())
				System.load("/data/local/lib/lib" + lib + ".so");
			else
				System.loadLibrary(lib);
			//InitializeOpenGL();
			startQtApp();
		}
		catch (Exception e)
		{
			Log.i(QtTAG, "Can't load 'lib" + lib + ".so'", e);
		}
	}

	public static void InitializeOpenGL()
	{
		mEgl = new QtEgl();
		if (!mEgl.initialize())
			mEgl = null;
	}

	@SuppressWarnings("unused")
	private boolean createSurface(final boolean OpenGl, final int id, final int l, final int t, final int r, final int b)
	{
		if (m_activity == null)
			return false;
		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (OpenGl)
					m_view.addView(new QtGlSurface(m_activity, id, l, t, r, b));
				else
					m_view.addView(new QtSurface(m_activity, id, l, t, r, b));
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private boolean resizeSurface(final int id, final int l, final int t, final int r, final int b)
	{
		if (m_activity == null)
			return false;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtSurfaceInterface surface = (QtSurfaceInterface) m_view.findViewById(id);
				if (surface == null)
					return;
				surface.Resize(l, t, r, b);
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private boolean destroySurface(final int id)
	{
		Log.i(QtTAG,"destroySurface "+id);
		if (m_activity == null)
			return false;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_view.removeView(m_view.findViewById(id));
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private void setSurfaceVisiblity(final int id, final boolean visible)
	{
		Log.i(QtTAG,"setSurfaceVisiblity "+id+" visible "+visible);
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtSurface surface = (QtSurface) m_view.findViewById(id);
				if (surface == null)
					return;
				surface.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
			}
		});
	}

	@SuppressWarnings("unused")
	private void setSurfaceOpacity(final int id, final double alpha)
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtSurface surface = (QtSurface) m_view.findViewById(id);
				if (surface == null)
					return;
//				surface.getHolder().getSurface().setAlpha((float) alpha);
			}
		});
	}

	@SuppressWarnings("unused")
	private void setWindowTitle(final int id, final String title)
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_activity.getWindow().setTitle(title);
			}
		});
	}

	@SuppressWarnings("unused")
	private void raiseSurface(final int id)
	{
		Log.i(QtTAG,"raiseSurface "+id);
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtSurface surface = (QtSurface) m_view.findViewById(id);
				if (surface == null)
					return;
				m_view.bringChildToFront(surface);
			}
		});
	}

	@SuppressWarnings("unused")
	private void redrawSurface(final int id, final int left, final int top, final int right, final int bottom )
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtSurface surface = (QtSurface) m_view.findViewById(id);
				if (surface == null)
					return;
				surface.invalidate(new Rect(left, top, right, bottom));
			}
		});
	}
	// application methods
	public static native void startQtApp();
	public static native void pauseQtApp();
	public static native void resumeQtApp();
	public static native void quitQtApp();
	public static native void setEglObject(Object eglObject);
	// application methods

	// screen methods
	public static native void setDisplayMetrics(int screenWidthPixels,
			int screenHeightPixels, int desktopWidthPixels,
			int desktopHeightPixels, float xdpi, float ydpi);
	// screen methods

	// pointer methods
	public static native void mouseDown(int x, int y);
	public static native void mouseUp(int x, int y);
	public static native void mouseMove(int x, int y);
	public static native void touchBegin();
	public static native void touchAdd(int pointerId, int action, boolean primary, int x, int y, float size, float pressure);
	public static native void touchEnd(int action);
	// pointer methods

	// keyboard methods
	public static native void keyDown(int key, int unicode, int modifier);
	public static native void keyUp(int key, int unicode, int modifier);
	// keyboard methods

	// surface methods
	public static native void surfaceCreated(Object surface, int id);
	public static native void surfaceChanged(Object surface, int id);
	public static native void surfaceDestroyed(int id);
	public static native void lockSurface();
	public static native void unlockSurface();
	// surface methods
}
