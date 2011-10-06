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

package org.kde.necessitas.interfaces;

// activity interface for Android API level 4
public interface QtActivityDelegateInterface4
{
// Application
    void onTerminate();

// Activity
    void onActivityResult(int requestCode, int resultCode, android.content.Intent data);
    void onApplyThemeResource(android.content.res.Resources.Theme theme, int resid, boolean first);
    void onChildTitleChanged(android.app.Activity childActivity, CharSequence title);
    boolean onContextItemSelected(android.view.MenuItem item);
    void onContextMenuClosed(android.view.Menu menu);
    void onCreate(android.os.Bundle savedInstanceState);
    CharSequence onCreateDescription();
    android.app.Dialog onCreateDialog(int id);
    boolean onCreateOptionsMenu(android.view.Menu menu);
    boolean onCreateThumbnail(android.graphics.Bitmap outBitmap, android.graphics.Canvas canvas);
    void onDestroy();
    void onNewIntent(android.content.Intent intent);
    boolean onOptionsItemSelected(android.view.MenuItem item);
    void onOptionsMenuClosed(android.view.Menu menu);
    void onPause();
    void onPostCreate(android.os.Bundle savedInstanceState);
    void onPostResume();
    void onPrepareDialog(int id, android.app.Dialog dialog);
    boolean onPrepareOptionsMenu(android.view.Menu menu);
    void onRestart();
    void onRestoreInstanceState(android.os.Bundle savedInstanceState);
    void onResume();
    Object onRetainNonConfigurationInstance();
    void onSaveInstanceState(android.os.Bundle outState);
    void onStart();
    void onStop();
    void onTitleChanged(CharSequence title, int color);
    boolean onTouchEvent(android.view.MotionEvent event);
    boolean onTrackballEvent(android.view.MotionEvent event);
    void onUserInteraction();
    void onUserLeaveHint();
// Activity


// From interface android.content.ComponentCallbacks
    void onConfigurationChanged(android.content.res.Configuration newConfig);
    void onLowMemory();

// From interface android.view.KeyEvent.Callback
    boolean onKeyDown(int keyCode, android.view.KeyEvent event);
    boolean onKeyMultiple(int keyCode, int repeatCount, android.view.KeyEvent event);
    boolean onKeyUp(int keyCode, android.view.KeyEvent event);

// From interface android.view.LayoutInflater.Factory
    android.view.View onCreateView(String name, android.content.Context context, android.util.AttributeSet attrs);

// From interface android.view.View.OnCreateContextMenuListener
    void onCreateContextMenu(android.view.ContextMenu menu, android.view.View v, android.view.ContextMenu.ContextMenuInfo menuInfo);

// From interface android.view.Window.Callback
    boolean dispatchKeyEvent(android.view.KeyEvent event);
    boolean dispatchPopulateAccessibilityEvent(android.view.accessibility.AccessibilityEvent event);
    boolean dispatchTouchEvent(android.view.MotionEvent event);
    boolean dispatchTrackballEvent(android.view.MotionEvent event);
    void onContentChanged();
    boolean onCreatePanelMenu(int featureId, android.view.Menu menu);
    android.view.View onCreatePanelView(int featureId);
    boolean onMenuItemSelected(int featureId, android.view.MenuItem item);
    boolean onMenuOpened(int featureId, android.view.Menu menu);
    void onPanelClosed(int featureId, android.view.Menu menu);
    boolean onPreparePanel(int featureId, android.view.View view, android.view.Menu menu);
    boolean onSearchRequested();
    void onWindowAttributesChanged(android.view.WindowManager.LayoutParams params);
    void onWindowFocusChanged(boolean hasFocus);
}

