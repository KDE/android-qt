package org.kde.necessitas.interfaces;


//activity interface for Android API level 12
public interface QtActivityDelegateInterface12
{
    boolean dispatchGenericMotionEvent(android.view.MotionEvent event);
    boolean onGenericMotionEvent(android.view.MotionEvent event);
}
