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
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager.LayoutParams;

public interface QtActivityDelegateInterface
{
    void onActivityResult(int requestCode, int resultCode, Intent data);
    void onApplyThemeResource(Theme theme, int resid, boolean first);
    void onAttachedToWindow();
    void onBackPressed();
    void onChildTitleChanged(Activity childActivity, CharSequence title);
    void onConfigurationChanged(Configuration newConfig);
    void onContentChanged();
    boolean onContextItemSelected(MenuItem item);
    void onContextMenuClosed(Menu menu);
    void onCreate(Bundle savedInstanceState);
    void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo);
    CharSequence onCreateDescription();
    Dialog onCreateDialog(int id);
    Dialog onCreateDialog(int id, Bundle args);
    boolean onCreateOptionsMenu(Menu menu);
    boolean onCreatePanelMenu(int featureId, Menu menu);
    View onCreatePanelView(int featureId);
    boolean onCreateThumbnail(Bitmap outBitmap, Canvas canvas);
    View onCreateView(String name, Context context, AttributeSet attrs);
    void onDestroy();
    void onDetachedFromWindow();
    boolean onKeyDown(int keyCode, KeyEvent event);
    boolean onKeyLongPress(int keyCode, KeyEvent event);
    boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event);
    boolean onKeyUp(int keyCode, KeyEvent event);
    void onLowMemory();
    boolean onMenuItemSelected(int featureId, MenuItem item);
    boolean onMenuOpened(int featureId, Menu menu);
    void onNewIntent(Intent intent);
    boolean onOptionsItemSelected(MenuItem item);
    void onOptionsMenuClosed(Menu menu);
    void onPanelClosed(int featureId, Menu menu);
    void onPause();
    void onPostCreate(Bundle savedInstanceState);
    void onPostResume();
    void onPrepareDialog(int id, Dialog dialog);
    void onPrepareDialog(int id, Dialog dialog, Bundle args);
    boolean onPrepareOptionsMenu(Menu menu);
    boolean onPreparePanel(int featureId, View view, Menu menu);
    void onRestart();
    void onRestoreInstanceState(Bundle savedInstanceState);
    void onResume();
    Object onRetainNonConfigurationInstance();
    void onSaveInstanceState(Bundle outState);
    boolean onSearchRequested();
    void onStart();
    void onStop();
    void onTerminate();
    void onTitleChanged(CharSequence title, int color);
    boolean onTouchEvent(MotionEvent event);
    boolean onTrackballEvent(MotionEvent event);
    void onUserInteraction();
    void onUserLeaveHint();
    void onWindowAttributesChanged(LayoutParams params);
    void onWindowFocusChanged(boolean hasFocus);
}
