package org.kde.necessitas.interfaces;

public interface QtActivitySuperInterface4 {

// Activity
    void super_onActivityResult(int requestCode, int resultCode, android.content.Intent data);
    void super_onApplyThemeResource(android.content.res.Resources.Theme theme, int resid, boolean first);
    void super_onChildTitleChanged(android.app.Activity childActivity, CharSequence title);
    boolean super_onContextItemSelected(android.view.MenuItem item);
    void super_onContextMenuClosed(android.view.Menu menu);
    void super_onCreate(android.os.Bundle savedInstanceState);
    CharSequence super_onCreateDescription();
    android.app.Dialog super_onCreateDialog(int id);
    boolean super_onCreateOptionsMenu(android.view.Menu menu);
    boolean super_onCreateThumbnail(android.graphics.Bitmap outBitmap, android.graphics.Canvas canvas);
    void super_onDestroy();
    void super_onNewIntent(android.content.Intent intent);
    boolean super_onOptionsItemSelected(android.view.MenuItem item);
    void super_onOptionsMenuClosed(android.view.Menu menu);
    void super_onPause();
    void super_onPostCreate(android.os.Bundle savedInstanceState);
    void super_onPostResume();
    void super_onPrepareDialog(int id, android.app.Dialog dialog);
    boolean super_onPrepareOptionsMenu(android.view.Menu menu);
    void super_onRestart();
    void super_onRestoreInstanceState(android.os.Bundle savedInstanceState);
    void super_onResume();
    Object super_onRetainNonConfigurationInstance();
    void super_onSaveInstanceState(android.os.Bundle outState);
    void super_onStart();
    void super_onStop();
    void super_onTitleChanged(CharSequence title, int color);
    boolean super_onTouchEvent(android.view.MotionEvent event);
    boolean super_onTrackballEvent(android.view.MotionEvent event);
    void super_onUserInteraction();
    void super_onUserLeaveHint();
// Activity


// From interface android.content.ComponentCallbacks
    void super_onConfigurationChanged(android.content.res.Configuration newConfig);
    void super_onLowMemory();

// From interface android.view.KeyEvent.Callback
    boolean super_onKeyDown(int keyCode, android.view.KeyEvent event);
    boolean super_onKeyMultiple(int keyCode, int repeatCount, android.view.KeyEvent event);
    boolean super_onKeyUp(int keyCode, android.view.KeyEvent event);

// From interface android.view.LayoutInflater.Factory
    android.view.View super_onCreateView(String name, android.content.Context context, android.util.AttributeSet attrs);

// From interface android.view.View.OnCreateContextMenuListener
    void super_onCreateContextMenu(android.view.ContextMenu menu, android.view.View v, android.view.ContextMenu.ContextMenuInfo menuInfo);

// From interface android.view.Window.Callback
    boolean super_dispatchKeyEvent(android.view.KeyEvent event);
    boolean super_dispatchPopulateAccessibilityEvent(android.view.accessibility.AccessibilityEvent event);
    boolean super_dispatchTouchEvent(android.view.MotionEvent event);
    boolean super_dispatchTrackballEvent(android.view.MotionEvent event);
    void super_onContentChanged();
    boolean super_onCreatePanelMenu(int featureId, android.view.Menu menu);
    android.view.View super_onCreatePanelView(int featureId);
    boolean super_onMenuItemSelected(int featureId, android.view.MenuItem item);
    boolean super_onMenuOpened(int featureId, android.view.Menu menu);
    void super_onPanelClosed(int featureId, android.view.Menu menu);
    boolean super_onPreparePanel(int featureId, android.view.View view, android.view.Menu menu);
    boolean super_onSearchRequested();
    void super_onWindowAttributesChanged(android.view.WindowManager.LayoutParams params);
    void super_onWindowFocusChanged(boolean hasFocus);
}
