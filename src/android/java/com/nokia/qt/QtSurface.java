package com.nokia.qt;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback {
	private int oldx, oldy;
	private Bitmap mBitmap=null;
	public boolean drawRequest=false;
	
	public QtSurface(Context context, int surfaceId, int l, int t, int r, int b)
	{
		super(context);
		// Log.i(QtApplication.QtTAG,"QtSurface "+surfaceId+" Left:"+l+" Top:"+t+" Right:"+r+" Bottom:"+b);
		setFocusable(true);
		getHolder().addCallback(this);
		getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
		setId(surfaceId);
		layout(l, t, r, b);
	}

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
    	QtApplication.lockSurface();
        // Log.i(QtApplication.QtTAG,"surfaceCreated "+getId());
        if (QtApplication.getEgl()!=null)
            QtApplication.getEgl().createSurface(holder, getId());
        else
        {
        	mBitmap=Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.RGB_565);
        	QtApplication.setSurface(getId(), mBitmap);
        }
        QtApplication.surfaceCreated(mBitmap, getId());
        QtApplication.unlockSurface();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
    	QtApplication.lockSurface();
        if (QtApplication.getEgl()!=null)
            QtApplication.getEgl().createSurface(holder, getId());
        else
        {
        	mBitmap=Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        	QtApplication.setSurface(getId(), mBitmap);
        }
        QtApplication.surfaceChanged(mBitmap, getId());
        QtApplication.unlockSurface();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
    	QtApplication.lockSurface();
		drawRequest=true;
        mBitmap=null;
        // Log.i(QtApplication.QtTAG,"surfaceDestroyed ");
        if (QtApplication.getEgl()!=null)
            QtApplication.getEgl().destroySurface(getId());
        else
        	QtApplication.setSurface(getId(), mBitmap);
        QtApplication.surfaceDestroyed(getId());
        QtApplication.unlockSurface();
    }

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
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
		return super.onTouchEvent(event);
	}

	@Override
	public boolean onTrackballEvent(MotionEvent event)
	{
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
		return super.onTrackballEvent(event);
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
				drawRequest=false;
			}
			catch (Exception e)
			{
				Log.e(QtApplication.QtTAG, "Can't create main activity", e);
			}
		}
    	QtApplication.unlockSurface();
    	}
}
