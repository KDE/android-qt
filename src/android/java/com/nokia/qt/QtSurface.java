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

    private int getAction(int index, MotionEvent event)
    {
    	int action=event.getAction();
/*
        case 0:
           state=Qt::TouchPointPressed;
           break;
       case 1:
           state=Qt::TouchPointMoved;
           break;
       case 2:
           state=Qt::TouchPointStationary;
           break;
       case 3:
           state=Qt::TouchPointReleased;
           break;

    */	
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
//		Log.i("Duda",event.toString());
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
