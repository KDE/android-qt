package com.nokia.qt;

import android.content.Context;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class QtSurface extends SurfaceView implements SurfaceHolder.Callback {
	private int oldx, oldy;

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
        // Log.i(QtApplication.QtTAG,"surfaceCreated "+getId());
        if (QtApplication.getEgl()!=null)
            QtApplication.getEgl().createSurface(holder, getId());
        QtApplication.surfaceCreated(holder.getSurface(), getId());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        if (QtApplication.getEgl()!=null)
            QtApplication.getEgl().createSurface(holder, getId());
        QtApplication.surfaceChanged(holder.getSurface(), getId());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        // Log.i(QtApplication.QtTAG,"surfaceDestroyed ");
        if (QtApplication.getEgl()!=null)
            QtApplication.getEgl().destroySurface(getId());
        QtApplication.surfaceDestroyed(getId());
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
}
