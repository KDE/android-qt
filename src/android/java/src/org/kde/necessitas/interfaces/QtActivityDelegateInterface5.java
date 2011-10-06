package org.kde.necessitas.interfaces;

//activity interface for Android API level 5
public interface QtActivityDelegateInterface5
{
    void onAttachedToWindow();
    void onBackPressed();
    void onDetachedFromWindow();
    boolean onKeyLongPress(int keyCode, android.view.KeyEvent event);
}
