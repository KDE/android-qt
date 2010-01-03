package com.nokia.qt;

import android.app.Application;

public class QtApplication extends Application {

	public QtApplication() {
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void onCreate() {
		super.onCreate();
		Native.startQtApp();
	}
}
