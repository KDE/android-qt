package org.kde.necessitas.interfaces;

//activity interface for Android API level 8
public interface QtActivityDelegateInterface8
{
    android.app.Dialog onCreateDialog(int id, android.os.Bundle args);
    void onPrepareDialog(int id, android.app.Dialog dialog, android.os.Bundle args);
}
