package org.kde.necessitas.industrius;

import android.os.Bundle;

public interface QtLoaderInterface
{
	boolean loadApplication(QtActivityInterface activityInterface, Bundle loaderParams);
	boolean statApplication();
}
