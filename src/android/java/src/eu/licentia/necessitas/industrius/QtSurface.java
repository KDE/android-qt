package eu.licentia.necessitas.industrius;

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
    private Bitmap m_bitmap=null;
    private boolean m_started = false;
    public QtSurface(Context context, int id)
    {
        super(context);
        setFocusable(true);
        getHolder().addCallback(this);
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
        setId(id);
    }

    public void applicationStared()
    {
        m_started = true;
        if (getWidth() < 1 ||  getHeight() < 1)
            return;
        QtApplication.lockSurface();
        QtApplication.setSurface(null);
        m_bitmap=Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.RGB_565);
        QtApplication.setSurface(m_bitmap);
        QtApplication.unlockSurface();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        DisplayMetrics metrics = new DisplayMetrics();
        ((Activity) getContext()).getWindowManager().getDefaultDisplay().getMetrics(metrics);
        QtApplication.setApplicationDisplayMetrics(metrics.widthPixels,
            metrics.heightPixels, getWidth(), getHeight(), metrics.xdpi, metrics.ydpi);

        if (!m_started)
            return;

        QtApplication.lockSurface();
        QtApplication.setSurface(null);
        m_bitmap=Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.RGB_565);
        QtApplication.setSurface(m_bitmap);
        QtApplication.unlockSurface();

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        Log.i(QtApplication.QtTAG,"surfaceChanged: "+width+","+height);
        if (width<1 || height<1)
                return;

        DisplayMetrics metrics = new DisplayMetrics();
        ((Activity) getContext()).getWindowManager().getDefaultDisplay().getMetrics(metrics);
        QtApplication.setApplicationDisplayMetrics(metrics.widthPixels,
            metrics.heightPixels, width, height, metrics.xdpi, metrics.ydpi);

        if (!m_started)
            return;
        QtApplication.lockSurface();
        QtApplication.setSurface(null);
        m_bitmap=Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        QtApplication.setSurface(m_bitmap);
        QtApplication.unlockSurface();
        QtApplication.updateWindow();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        if (!m_started)
            return;
        Log.i(QtApplication.QtTAG,"surfaceDestroyed ");
        QtApplication.lockSurface();
        QtApplication.setSurface(null);
        QtApplication.unlockSurface();
    }

    public void drawBitmap(Rect rect)
    {
        if (!m_started)
            return;
        QtApplication.lockSurface();
        if (null!=m_bitmap)
        {
            try
            {
                Canvas cv=getHolder().lockCanvas(rect);
                cv.drawBitmap(m_bitmap, rect, rect, null);
                getHolder().unlockCanvasAndPost(cv);
            }
            catch (Exception e)
            {
                Log.e(QtApplication.QtTAG, "Can't create main activity", e);
            }
        }
        QtApplication.unlockSurface();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (!m_started)
            return false;
        QtApplication.sendTouchEvent(event, getId());
        return true;
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event)
    {
        if (!m_started)
            return false;
        QtApplication.sendTrackballEvent(event, getId());
        return true;
    }
}
