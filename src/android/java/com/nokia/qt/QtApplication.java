package com.nokia.qt;

import java.io.File;
import java.util.List;

import android.graphics.Rect;
import android.util.Log;
import android.view.View;

public class QtApplication
{
	public static final String QtTAG = "Qt JAVA";
	private static QtActivity m_activity = null;
	private static QtMainView m_view = null;
	private static QtEgl mEgl = null;
	
	public static QtEgl getEgl()
    {
        return mEgl;
    }
	
	public static void setActivity(QtActivity mActivity)
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

	public static void loadLibraries(List<String> libraries)
	{
		for (int i = 0; i < libraries.size(); i++)
		{
			try
			{
				String library = "/data/local/qt/lib/lib" + libraries.get(i) + ".so";
				File f = new File(library);
				if (f.exists())
					System.load(library);
			}
			catch (Exception e)
			{
				Log.i(QtTAG, "Can't load '/data/local/qt/lib/lib"
						+ libraries.get(i) + ".so'", e);
			}
		}
	}

    public static void loadApplication(String lib, Object jniProxyObject)
	{
		try
		{
			String library = "/data/local/lib/lib" + lib + ".so";
			File f = new File(library);
			if (f.exists())
				System.load(library);
			else
				System.loadLibrary(lib);

			//InitializeOpenGL();
			startQtAndroidPlugin();
			startQtApp(jniProxyObject);
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
	private boolean createWindow(final boolean OpenGl, final int id, final int l, final int t, final int r, final int b)
	{
		if (m_activity == null)
			return false;
		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (OpenGl)
					m_view.addView(new QtGlWindow(m_activity, id, l, t, r, b));
				else
					m_view.addView(new QtWindow(m_activity, id, l, t, r, b));
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private boolean resizeWindow(final int id, final int l, final int t, final int r, final int b)
	{
		if (m_activity == null)
			return false;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtWindowInterface window = (QtWindowInterface) m_view.findViewById(id);
				if (window == null)
					return;
				window.Resize(l, t, r, b);
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private boolean destroyWindow(final int id)
	{
		Log.i(QtTAG,"destroyWindow "+id);
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
	private void setWindowVisiblity(final int id, final boolean visible)
	{
		Log.i(QtTAG,"setSurfaceVisiblity "+id+" visible "+visible);
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtWindow window = (QtWindow) m_view.findViewById(id);
				if (window == null)
					return;
				window.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
			}
		});
	}

	@SuppressWarnings("unused")
	private void setWindowOpacity(final int id, final double alpha)
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtWindow window = (QtWindow) m_view.findViewById(id);
				if (window == null)
					return;
//				window.getHolder().getSurface().setAlpha((float) alpha);
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
	private void raiseWindow(final int id)
	{
		Log.i(QtTAG,"raiseSurface "+id);
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtWindow window = (QtWindow) m_view.findViewById(id);
				if (window == null)
					return;
				m_view.bringChildToFront(window);
			}
		});
	}

	@SuppressWarnings("unused")
	private void redrawWindow(final int id, final int left, final int top, final int right, final int bottom )
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				QtWindow window = (QtWindow) m_view.findViewById(id);
				if (window == null)
					return;
				window.invalidate(new Rect(left, top, right, bottom));
			}
		});
	}
	
	@SuppressWarnings("unused")
	private void showSoftwareKeyboard()
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_activity.showSoftwareKeyboard();
			}
		});
	}

	@SuppressWarnings("unused")
	private void hideSoftwareKeyboard()
	{
		if (m_activity == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_activity.hideSoftwareKeyboard();
			}
		});
	}

	@SuppressWarnings("unused")
	private void quitApp()
	{
		m_activity.finish();
	}	
	// application methods
	public static native void startQtApp(Object jniProxyObject);
	public static native void pauseQtApp();
	public static native void resumeQtApp();
	public static native void startQtAndroidPlugin();
	public static native void quitQtAndroidPlugin();
	public static native void setEglObject(Object eglObject);
	// application methods

	// screen methods
	public static native void setDisplayMetrics(int screenWidthPixels,
			int screenHeightPixels, int desktopWidthPixels,
			int desktopHeightPixels, float xdpi, float ydpi);
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

	// window methods
	public static native void windowCreated(Object window, int id);
	public static native void windowChanged(Object window, int id);
	public static native void windowDestroyed(int id);
	public static native void lockWindow();
	public static native void unlockWindow();
	// window methods
}
