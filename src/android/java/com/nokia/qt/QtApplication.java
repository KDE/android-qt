package com.nokia.qt;

import java.io.File;
import java.nio.ShortBuffer;

import android.app.Activity;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;

public class QtApplication {
	public static final String QtTAG = "Qt JAVA";
	private static Activity m_activity = null;
	private static ViewGroup m_view = null;
	private static QtEgl mEgl = null;

	public static void setActivity(Activity mActivity) {
		m_activity = mActivity;
	}

	public static void setView(ViewGroup view) {
		m_view = view;
	}

	// static
	// {
	// // System.load("/data/local/qt/lib/libAndroidSetQtEnv.so");
	// }
	//	
	public static void loadLibraries(String[] libraries) {
		for (int i = 0; i < libraries.length; i++) {
			try {
				String library = "/data/local/qt/lib/lib" + libraries[i]
						+ ".so";
				File f = new File(library);
				if (f.exists())
					System.load(library);
			} catch (Exception e) {
				Log.i(QtTAG, "Can't load '/data/local/qt/lib/lib"
						+ libraries[i] + ".so'", e);
			}
		}
	}

	public static void loadApplication(String lib) {
		try {
			File f = new File("/data/local/lib/lib" + lib + ".so");
			if (f.exists())
				System.load("/data/local/lib/lib" + lib + ".so");
			else
				System.loadLibrary(lib);
			startQtApp(mEgl != null);
		} catch (Exception e) {
			Log.i(QtTAG, "Can't load 'lib" + lib + ".so'", e);
		}
	}

	public void InitializeOpenGL(boolean useAA) {
		mEgl = new QtEgl();
		if (!mEgl.initialize(useAA))
			mEgl = null;
	}

	@SuppressWarnings("unused")
	private void flushImage(ShortBuffer image, int bytesPerRow, int x, int y,
			int r, int b) {
	}

	@SuppressWarnings("unused")
	private boolean createSurface(final int id, final int l, final int t,
			final int r, final int b) {
		if (m_activity == null)
			return false;
		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_view.addView(new QtSurface(m_activity, id, l, t, r, b),
						m_view.getChildCount());
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private boolean resizeSurface(final int id, final int l, final int t,
			final int r, final int b) {
		if (m_activity == null)
			return false;

		final QtSurface surface = (QtSurface) m_view.findViewById(id);
		if (surface == null)
			return false;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				surface.layout(l, t, r, b);
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private boolean destroySurface(final int id) {
		if (m_activity == null)
			return false;

		final QtSurface surface = (QtSurface) m_view.findViewById(id);
		if (surface == null)
			return false;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_view.removeView(surface);
			}
		});
		return true;
	}

	@SuppressWarnings("unused")
	private void setSurfaceVisiblity(final int id, final boolean visible) {
		if (m_activity == null)
			return;

		final QtSurface surface = (QtSurface) m_view.findViewById(id);
		if (surface == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				surface.setVisibility(visible ? View.VISIBLE : View.GONE);
			}
		});
	}

	@SuppressWarnings("unused")
	private void setSurfaceOpacity(final int id, final double alpha) {
		if (m_activity == null)
			return;

		final QtSurface surface = (QtSurface) m_view.findViewById(id);
		if (surface == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				surface.getHolder().getSurface().setAlpha((float) alpha);
			}
		});
	}

	@SuppressWarnings("unused")
	private void setWindowTitle(final int id, final String title) {
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
	private void raiseSurface(final int id) {
		if (m_activity == null)
			return;

		final QtSurface surface = (QtSurface) m_view.findViewById(id);
		if (surface == null)
			return;

		m_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				m_view.bringChildToFront(surface);
			}
		});
	}

	// application methods
	public static native void startQtApp(boolean useOpenGL);

	public static native void pauseQtApp();

	public static native void resumeQtApp();

	public static native void quitQtApp();

	// application methods

	// screen methods
	public static native void setDisplayMetrics(int widthPixels,
			int heightPixels, float xdpi, float ydpi);

	// screen methods

	// pointer methods
	public static native void mouseDown(int x, int y);

	public static native void mouseUp(int x, int y);

	public static native void mouseMove(int x, int y);

	// pointer methods

	// keyboard methods
	public static native void keyDown(int key, int unicode, int modifier);

	public static native void keyUp(int key, int unicode, int modifier);

	// keyboard methods

	// surface methods
	public static native void surfaceCreated(Surface surface, int id);

	public static native void surfaceChanged(Surface surface, int id);

	public static native void surfaceDestroyed(int id);
	// surface methods

}
