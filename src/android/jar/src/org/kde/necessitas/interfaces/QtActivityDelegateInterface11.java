package org.kde.necessitas.interfaces;

//activity interface for Android API level 11
//@ANDROID-11
public interface QtActivityDelegateInterface11
{
    boolean dispatchKeyShortcutEvent (android.view.KeyEvent event);
    void onActionModeFinished(android.view.ActionMode mode);
    void onActionModeStarted(android.view.ActionMode mode);
    void onAttachFragment(android.app.Fragment fragment);
    android.view.View onCreateView(android.view.View parent, String name, android.content.Context context, android.util.AttributeSet attrs);
    boolean onKeyShortcut(int keyCode, android.view.KeyEvent event);
    android.view.ActionMode onWindowStartingActionMode (android.view.ActionMode.Callback callback);
}
//@ANDROID-11
