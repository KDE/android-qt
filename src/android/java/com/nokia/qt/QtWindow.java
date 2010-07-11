package com.nokia.qt;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class QtWindow extends View implements QtWindowInterface
{
	private Bitmap bitmap=null;
	private int left, top, right, bottom;
	private int oldx, oldy;

	public QtWindow(Context context, int windowId, int l, int t, int r, int b)
	{
		super(context);
		setFocusable(true);
		setId(windowId);
		left=l;
		top=t;
		right=r;
		bottom=b;
		QtApplication.lockWindow();
		bitmap = Bitmap.createBitmap(right-left+1, bottom-top+1, Bitmap.Config.RGB_565);
        QtApplication.unlockWindow();			
	}
	
	@Override
	protected void onDraw(Canvas canvas)
	{
    	QtApplication.lockWindow();
		canvas.drawBitmap(bitmap, canvas.getClipBounds(), canvas.getClipBounds(), null);
    	//canvas.drawBitmap(bitmap, 0, 0, null);
        QtApplication.unlockWindow();
	}
	
	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh)
	{
		Log.i(QtApplication.QtTAG, "onSizeChanged id:"+getId()+" w:"+w+" h:"+h+" oldw:"+oldw+"oldh:"+oldh+" reqw:"+(right-left)+"reqh"+(bottom -top));
		if ( w != right-left || h != bottom -top )
			layout(left, top, right, bottom);
		invalidate();
	}

	@Override
	protected void onWindowVisibilityChanged(int visibility)
	{
		Log.i(QtApplication.QtTAG, "id:"+getId()+" WindowVisibilityChanged:"+visibility);
		if (visibility==VISIBLE)
		{
	        QtApplication.lockWindow();
	        QtApplication.windowCreated(bitmap, getId());
	        QtApplication.unlockWindow();
	        bringToFront();
	        invalidate();
		}
		else if (visibility==GONE) 
		{
	        QtApplication.lockWindow();
	        QtApplication.windowDestroyed(getId());
	        QtApplication.unlockWindow();
		}
		super.onWindowVisibilityChanged(visibility);
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
					(int)event.getX(i)+left, (int)event.getY(i)+top, event.getSize(i),
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
			QtApplication.mouseUp((int) event.getX()+left, (int) event.getY()+top);
			return true;

		case MotionEvent.ACTION_DOWN:
			QtApplication.mouseDown((int) event.getX()+left, (int) event.getY()+top);
			oldx = (int) event.getX();
			oldy = (int) event.getY();
			return true;

		case MotionEvent.ACTION_MOVE:
			int dx = (int) (event.getX() - oldx);
			int dy = (int) (event.getY() - oldy);
			if (Math.abs(dx) > 5 || Math.abs(dy) > 5)
			{
				QtApplication.mouseMove((int) event.getX()+left, (int) event.getY()+top);
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
			QtApplication.mouseUp((int) event.getX()+left, (int) event.getY()+top);
			return true;

		case MotionEvent.ACTION_DOWN:
			QtApplication.mouseDown((int) event.getX()+left, (int) event.getY()+top);
			oldx = (int) event.getX();
			oldy = (int) event.getY();
			return true;

		case MotionEvent.ACTION_MOVE:
			int dx = (int) (event.getX() - oldx);
			int dy = (int) (event.getY() - oldy);
			if (Math.abs(dx) > 5 || Math.abs(dy) > 5)
			{
				QtApplication.mouseMove((int) event.getX()+left, (int) event.getY()+top);
				oldx = (int) event.getX();
				oldy = (int) event.getY();
			}
			return true;
		}
		return super.onTrackballEvent(event);
	}

	@Override
	public int isOpenGl()
	{
		return 0;
	}

	@Override
	public void Resize(int l, int t, int r, int b)
	{
		left=l;
		top=t;
		right=r;
		bottom=b;
		QtApplication.lockWindow();
		bitmap = Bitmap.createBitmap(right-left+1, bottom-top+1, Bitmap.Config.RGB_565);
        QtApplication.windowChanged(bitmap, getId());
        QtApplication.unlockWindow();			
        bringToFront();
		layout(left, top, right, bottom);
		invalidate();
	}
}
