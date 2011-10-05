package org.kde.necessitas.industrius;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources.Theme;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.Bundle;
import android.util.AttributeSet;
import android.view.ContextMenu;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.WindowManager.LayoutParams;

public interface QtActivityInterface
{
	void setQtActivityListener(QtActivityDelegate listener);
	Activity getActivity();

	// super class calls
	void super_onActivityResult(int requestCode, int resultCode, Intent data);
	void super_onApplyThemeResource(Theme theme, int resid, boolean first);
	void super_onAttachedToWindow();
	void super_onBackPressed();
	void super_onChildTitleChanged(Activity childActivity, CharSequence title);
	void super_onConfigurationChanged(Configuration newConfig);
	void super_onContentChanged();
	boolean super_onContextItemSelected(MenuItem item);
	void super_onContextMenuClosed(Menu menu);
	void super_onCreate(Bundle savedInstanceState);
	void super_onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo);
	CharSequence super_onCreateDescription();
	Dialog super_onCreateDialog(int id);
	Dialog super_onCreateDialog(int id, Bundle args);
	boolean super_onCreateOptionsMenu(Menu menu);
	boolean super_onCreatePanelMenu(int featureId, Menu menu);
	View super_onCreatePanelView(int featureId);
	boolean super_onCreateThumbnail(Bitmap outBitmap, Canvas canvas);
	View super_onCreateView(String name, Context context, AttributeSet attrs);
	void super_onDestroy();
	void super_onDetachedFromWindow();
	boolean super_onKeyDown(int keyCode, KeyEvent event);
	boolean super_onKeyLongPress(int keyCode, KeyEvent event);
	boolean super_onKeyMultiple(int keyCode, int repeatCount, KeyEvent event);
	boolean super_onKeyUp(int keyCode, KeyEvent event);
	void super_onLowMemory();
	boolean super_onMenuItemSelected(int featureId, MenuItem item);
	boolean super_onMenuOpened(int featureId, Menu menu);
	void super_onNewIntent(Intent intent);
	boolean super_onOptionsItemSelected(MenuItem item);
	void super_onOptionsMenuClosed(Menu menu);
	void super_onPanelClosed(int featureId, Menu menu);
	void super_onPause();
	void super_onPostCreate(Bundle savedInstanceState);
	void super_onPostResume();
	void super_onPrepareDialog(int id, Dialog dialog);
	void super_onPrepareDialog(int id, Dialog dialog, Bundle args);
    boolean super_onPrepareOptionsMenu(Menu menu);
    boolean super_onPreparePanel(int featureId, View view, Menu menu);
    void super_onRestart();
    void super_onRestoreInstanceState(Bundle savedInstanceState);
    void super_onResume();
    Object super_onRetainNonConfigurationInstance();
    void super_onSaveInstanceState(Bundle outState);
    boolean super_onSearchRequested();
    void super_onStart();
    void super_onStop();
    void super_onTitleChanged(CharSequence title, int color);
    boolean super_onTouchEvent(MotionEvent event);
    boolean super_onTrackballEvent(MotionEvent event);
    void super_onUserInteraction();
    void super_onUserLeaveHint();
    void super_onWindowAttributesChanged(LayoutParams params);
    void super_onWindowFocusChanged(boolean hasFocus);
	// super class calls
}
