package com.nokia.qt;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback
{
	private Bitmap mBitmap=null;
	
	private int oldx,oldy;
	public QtSurface(Context context)
	{
		super(context);
		setFocusable(true);
		getHolder().addCallback(this);
		getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder)
	{
    	QtApplication.lockSurface();
    	mBitmap=Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.RGB_565);
		QtApplication.setSurface(mBitmap);
        QtApplication.unlockSurface();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
	{
		Log.i(QtApplication.QtTAG,"surfaceChanged: "+width+","+height);
    	QtApplication.lockSurface();
    	mBitmap=Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
		QtApplication.setSurface(mBitmap);
		DisplayMetrics metrics = new DisplayMetrics();
		((Activity) getContext()).getWindowManager().getDefaultDisplay().getMetrics(metrics);
		QtApplication.setDisplayMetrics(metrics.widthPixels, metrics.heightPixels,
										width, height, metrics.xdpi, metrics.ydpi);
		QtApplication.m_surface=this;
        QtApplication.unlockSurface();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder)
	{
		Log.i(QtApplication.QtTAG,"surfaceDestroyed ");
    	QtApplication.lockSurface();
		QtApplication.destroySurface();
		QtApplication.m_surface=null;
        QtApplication.unlockSurface();
	}

	public void drawBitmap(Rect rect)
	{
		QtApplication.lockSurface();
		if (null!=mBitmap)
		{
			try
			{
				Canvas cv=getHolder().lockCanvas(rect);
				cv.drawBitmap(mBitmap, rect, rect, null);
				getHolder().unlockCanvasAndPost(cv);
			}
			catch (Exception e)
			{
				Log.e(QtApplication.QtTAG, "Can't create main activity", e);
			}
		}
    	QtApplication.unlockSurface();
    }
	
    private int getAction(int index, MotionEvent event)
    {
    	int action=event.getAction();
		if (action == MotionEvent.ACTION_MOVE)
		{
			int hsz=event.getHistorySize();
			if (hsz>0)
			{
				if (Math.abs(event.getX(index)-event.getHistoricalX(index, hsz-1))>1||
						Math.abs(event.getY(index)-event.getHistoricalY(index, hsz-1))>1)
					return 1;
				else
					return 2;
			}
			return 1;
		}

    	switch(index)
		{
		case 0:
			if (action == MotionEvent.ACTION_DOWN || 
					action == MotionEvent.ACTION_POINTER_1_DOWN)
				return 0;
			if (action == MotionEvent.ACTION_UP || 
					action == MotionEvent.ACTION_POINTER_1_UP)
				return 3;
			break;
		case 1:
			if (action == MotionEvent.ACTION_POINTER_2_DOWN ||
					action == MotionEvent.ACTION_POINTER_DOWN)
				return 0;
			if (action == MotionEvent.ACTION_POINTER_2_UP ||
					action == MotionEvent.ACTION_POINTER_UP)
				return 3;
			break;
		case 2:
			if (action == MotionEvent.ACTION_POINTER_3_DOWN ||
					action == MotionEvent.ACTION_POINTER_DOWN)
				return 0;
			if (action == MotionEvent.ACTION_POINTER_3_UP ||
					action == MotionEvent.ACTION_POINTER_UP)
				return 3;
			break;
		}
		return 2;
    }

    public void sendTouchEvents(MotionEvent event)
    {
		QtApplication.touchBegin();
		for (int i=0;i<event.getPointerCount();i++)
			QtApplication.touchAdd(event.getPointerId(i), getAction(i, event), i==0,
					(int)event.getX(i), (int)event.getY(i), event.getSize(i),
					event.getPressure(i));

		switch(event.getAction())
		{
		case MotionEvent.ACTION_DOWN:
			QtApplication.touchEnd(0);
			break;
		case MotionEvent.ACTION_UP:
			QtApplication.touchEnd(2);
			break;
		default:
			QtApplication.touchEnd(1);
		}
    }

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		sendTouchEvents(event);
		switch (event.getAction())
		{
		case MotionEvent.ACTION_UP:
			QtApplication.mouseUp((int) event.getX(), (int) event.getY());
			return true;

		case MotionEvent.ACTION_DOWN:
			QtApplication.mouseDown((int) event.getX(), (int) event.getY());
			oldx = (int) event.getX();
			oldy = (int) event.getY();
			return true;

		case MotionEvent.ACTION_MOVE:
			int dx = (int) (event.getX() - oldx);
			int dy = (int) (event.getY() - oldy);
			if (Math.abs(dx) > 5 || Math.abs(dy) > 5)
			{
				QtApplication.mouseMove((int) event.getX(), (int) event.getY());
				oldx = (int) event.getX();
				oldy = (int) event.getY();
			}
			return true;
		}
		return true;
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
}
