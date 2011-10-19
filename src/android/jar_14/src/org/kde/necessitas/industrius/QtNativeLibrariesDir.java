package org.kde.necessitas.industrius;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;

public class QtNativeLibrariesDir {
    public static String nativeLibrariesDir(Activity activity)
    {
        String m_nativeLibraryDir = null;
        try {
            ApplicationInfo ai=activity.getPackageManager().getApplicationInfo(activity.getPackageName(), 0);
            m_nativeLibraryDir = ai.nativeLibraryDir+"/";
        } catch (NameNotFoundException e) {
            e.printStackTrace();
        }
        return m_nativeLibraryDir;
    }
}
