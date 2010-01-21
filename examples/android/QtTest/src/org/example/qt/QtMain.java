package org.example.qt;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;

import com.nokia.qt.QtApplication;
import com.nokia.qt.QtSurface;

public class QtMain extends Activity
{
	private static final String TAG = "QtTest";
	private boolean quitApp=true;
	// handles counting down
//	private Handler mHandler = new Handler();

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		try
		{
//			QtApplication.setMainLayout((QtLayout) findViewById(R.id.QtSurface));
//			QtApplication.setMainHandler(mHandler);
			if (null==getLastNonConfigurationInstance())
			{
				QtApplication.loadApplication("animatedtiles");
			}
			quitApp=true;
			setContentView(new QtSurface(this));
			Log.i(TAG,"Application start");
		}
		catch (Exception e)
		{
			Log.e(TAG, "service creation problem", e);
		}
    }
    
    @Override
    public Object onRetainNonConfigurationInstance() {
    	quitApp=false;
    	return true;
    }
    
    @Override
	protected void onDestroy() {
    	if (quitApp)
    	{
    		QtApplication.quitQtApp();
    	}
	}   
}