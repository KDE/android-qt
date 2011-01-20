package com.nokia.qt.android;
import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

public class QtSensors implements SensorEventListener
{
	private static Activity m_activity;
	private static SensorManager m_SensorManager;
	private Sensor m_Sensor;
	private int m_uniqueId=-1;
	private int m_sensorType=-1;

	public QtSensors(int uniqueID,int datarate,int sensorType) 
	{
		m_uniqueId=uniqueID;
		m_sensorType=sensorType;
		start(datarate);
	}

	public static void setActivity(Activity mActivity)
	{
		m_activity = mActivity;
	}

	private static void registerSensors ()
	{
		m_SensorManager = (SensorManager)m_activity.getSystemService(m_activity.SENSOR_SERVICE);
	}

	private void start (int datarate)
	{
		m_Sensor = m_SensorManager.getDefaultSensor(m_sensorType);
		m_SensorManager.registerListener(this, m_Sensor, datarate);
	}

	public void stop ()
	{
		m_SensorManager.unregisterListener(this, m_Sensor);
	}

	public void onSensorChanged(SensorEvent event) {

		if(event.sensor.getType()==Sensor.TYPE_PROXIMITY)
		{
			event.values[0]=(event.values[0]<event.sensor.getMaximumRange())?1:0;
		} 
		slotDataAvailable(event.values,event.timestamp,event.accuracy,m_uniqueId);

	}

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub

	}

	public static native void slotDataAvailable(float data[],long timeEvent,
			int accuracy,int uniqueID);


}
