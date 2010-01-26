package org.example.qt;

import android.util.Log;

import com.nokia.qt.QtActivity;
import com.nokia.qt.QtApplication;

public class QtMain extends QtActivity
{
	public QtMain()
	{
		setApplication("animatedtiles");
		Log.i(QtApplication.QtTAG,"QtMain constructor");
	}
}