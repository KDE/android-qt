package com.nokia.qt;

import android.content.Context;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback
{
	public QtSurface(Context context)
	{
		super(context);
		getHolder().addCallback(this);
		setFocusable(true);
	}

	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		switch(event.getAction())
		{
			case MotionEvent.ACTION_UP:
				event.getPointerCount();
				return true;
			case MotionEvent.ACTION_DOWN:
				return true;
			case MotionEvent.ACTION_MOVE:
				return true;
		}
		// TODO Auto-generated method stub
		return super.onTouchEvent(event);
	}
	
	@Override
	public boolean onTrackballEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		return super.onTrackballEvent(event);
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		try{
			QtApplication.setWindowSurface(getId(), holder.getSurface());
		}catch(Exception e){}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		try{
			QtApplication.setWindowSurface(getId(), holder.getSurface());
		}catch(Exception e){}
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		try{
			QtApplication.setWindowSurface(getId(), null);
		}catch(Exception e){}
	}
}
