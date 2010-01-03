package com.nokia.qt;

import android.content.Context;
import android.view.MotionEvent;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView {
	public QtSurface(Context context) {
		super(context);
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		return super.onTouchEvent(event);
	}
	
	@Override
	public boolean onTrackballEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		return super.onTrackballEvent(event);
	}
}
