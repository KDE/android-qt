package com.nokia.qt;

import android.app.Activity;
import android.os.Bundle;

public class QtActivity extends Activity {
	private QtSurface mSurface=null;
	
	long getId()
	{
		if (mSurface!=null)
			return mSurface.getId();
		return -1;
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		mSurface = new QtSurface(this);
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}
}
