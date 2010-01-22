package com.nokia.qt;

import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import com.nokia.qt.QtApplication;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback
{
	private int oldx,oldy;
	public QtSurface(Context context)
	{
		super(context);
//		setId(QtApplication.getNextWindowId());
		Log.i(QtApplication.QtTAG, "QtSurface constructor !!!");
		getHolder().addCallback(this);
		setFocusable(true);
	}

	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		switch(event.getAction())
		{
			case MotionEvent.ACTION_UP:
				QtApplication.actionUp((int)event.getX(), (int)event.getY());
				event.getPointerCount();
				return true;
				
			case MotionEvent.ACTION_DOWN:
				QtApplication.actionDown((int)event.getX(), (int)event.getY());
				oldx=(int)event.getX();
				oldy=(int)event.getY();
				return true;
				
			case MotionEvent.ACTION_MOVE:
				int dx = (int)(event.getX() - oldx);
				int dy = (int)(event.getY() - oldy);
				if (Math.abs(dx) > 5 || Math.abs(dy) > 5)
				{
					QtApplication.actionMove((int)event.getX(), (int)event.getY());
					oldx = (int)event.getX();
					oldy = (int)event.getY();
				}
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
		Log.i(QtApplication.QtTAG,"surfaceCreated"+getId());
		try{
			QtApplication.setSurface(holder.getSurface());
		}catch(Exception e){}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		Log.i(QtApplication.QtTAG,"surfaceChanged: "+width+","+height);
		try{
			QtApplication.setSurface(holder.getSurface());
		}catch(Exception e){}
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.i(QtApplication.QtTAG,"surfaceDestroyed ");
		try{
			QtApplication.destroySurface();
		}catch(Exception e){}
	}
}
