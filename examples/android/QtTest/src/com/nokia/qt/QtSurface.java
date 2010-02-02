package com.nokia.qt;

import android.content.Context;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback
{
	private int oldx,oldy;
	public QtSurface(Context context)
	{
		super(context);
		setFocusable(true);
		getHolder().addCallback(this);
		Log.i(QtApplication.QtTAG, "QtSurface constructor !!!");
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		switch(event.getAction())
		{
			case MotionEvent.ACTION_UP:
				QtApplication.mouseUp((int)event.getX(), (int)event.getY());
				return true;
				
			case MotionEvent.ACTION_DOWN:
				QtApplication.mouseDown((int)event.getX(), (int)event.getY());
				oldx=(int)event.getX();
				oldy=(int)event.getY();
				return true;
				
			case MotionEvent.ACTION_MOVE:
				int dx = (int)(event.getX() - oldx);
				int dy = (int)(event.getY() - oldy);
				if (Math.abs(dx) > 5 || Math.abs(dy) > 2)
				{
					QtApplication.mouseMove((int)event.getX(), (int)event.getY());
					oldx = (int)event.getX();
					oldy = (int)event.getY();
				}
				return true;
		}
		return super.onTouchEvent(event);
	}

	@Override
	public boolean onTrackballEvent(MotionEvent event)
	{
		switch(event.getAction())
		{
			case MotionEvent.ACTION_UP:
				QtApplication.mouseUp((int)event.getX(), (int)event.getY());
				return true;
				
			case MotionEvent.ACTION_DOWN:
				QtApplication.mouseDown((int)event.getX(), (int)event.getY());
				oldx=(int)event.getX();
				oldy=(int)event.getY();
				return true;
				
			case MotionEvent.ACTION_MOVE:
				int dx = (int)(event.getX() - oldx);
				int dy = (int)(event.getY() - oldy);
				if (Math.abs(dx) > 5 || Math.abs(dy) > 2)
				{
					QtApplication.mouseMove((int)event.getX(), (int)event.getY());
					oldx = (int)event.getX();
					oldy = (int)event.getY();
				}
				return true;
		}
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
