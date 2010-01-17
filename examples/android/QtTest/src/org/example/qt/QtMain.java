package org.example.qt;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import com.nokia.qt.*;

public class QtMain extends Activity {
	private static final String TAG = "QtTest";
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.main);
		try
		{
			if (null==getLastNonConfigurationInstance())
			{
				QtApplication.loadApplication("animatedtiles");
				QtApplication.setMainContext(this);
				Log.i(TAG,"Application start`");
			}
		}
		catch (Exception e)
		{
			Log.e(TAG, "service creation problem", e);
		}
    }
}