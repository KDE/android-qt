package org.kde.necessitas.industrius;

import android.os.Bundle;

public class QtLoader implements QtLoaderInterface
{
	@Override
	public boolean loadApplication(QtActivityInterface activityInterface,
			Bundle loaderParams) {
		return false;
	}

	@Override
	public boolean statApplication() {
		return false;
	}

}
