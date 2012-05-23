/*
Copyright (c) 2011 Elektrobit (EB), All rights reserved.
Contact: oss-devel@elektrobit.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the Elektrobit (EB) nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Elektrobit (EB) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Elektrobit
(EB) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package org.kde.necessitas.mobile;

import java.io.File;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

import org.kde.necessitas.industrius.QtNative;

import android.app.KeyguardManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.media.AudioManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.BatteryManager;
import android.os.Build;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.telephony.CellLocation;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.telephony.gsm.GsmCellLocation;
import android.util.DisplayMetrics;
import android.view.Display;


class BatteryInfo
{
    @SuppressWarnings("unused")
    private char m_batteryStatus;
    @SuppressWarnings("unused")
    private int m_chargerType;
    int m_chargingState;
    @SuppressWarnings("unused")
    private int m_maxBars;
    @SuppressWarnings("unused")
    private int m_remainingCapacityBars;
    @SuppressWarnings("unused")
    private int m_remainingCapacityPercent;
    @SuppressWarnings("unused")
    private int m_voltage;

    BatteryInfo(char batteryStatus,int chargerType,int chargingState,int maxBars,int remainingCapacityBars,int remainingCapacityPercent,int voltage)
    {

        m_batteryStatus=batteryStatus;
        m_chargerType=chargerType;
        m_chargingState=chargingState;
        m_maxBars=maxBars;
        m_remainingCapacityBars=remainingCapacityBars;
        m_remainingCapacityBars=remainingCapacityBars;
        m_remainingCapacityPercent=remainingCapacityPercent;
        m_voltage=voltage;
    }
}

public class QtSystemInfo
{
    private static Map<Integer,Integer> m_chargerType;
    private static HashMap<Integer,Integer> m_Chargingstatus;
    private IntentFilter m_batteryIntentFilter;
    private BroadcastReceiver m_batteryInfoBroadcastReceiver;

    private KeyguardManager m_keyguardManager;
    private KeyguardManager.KeyguardLock m_keygaurdLock;
    private PowerManager m_powerManager;
    private WakeLock m_wakeLock;

    private TelephonyManager m_telephonyManager;
    private BroadcastReceiver m_deviceInfoBroadcastReceiver;
    //display
    private Display m_display;
    private DisplayMetrics m_displaymatrics;
    private BroadcastReceiver m_displayInfoBroadcastReceiver;

    private BroadcastReceiver m_storageInfoBroadcastReceiver;

    public static final int BRIGHTNESS_OFF = 0;//Brightness value for fully off
    public static final int BRIGHTNESS_DIM = 20;//Brightness value for dim backlight
    public static final int BRIGHTNESS_ON = 255;//Brightness value for fully on

    //network
    private ConnectivityManager m_connectivityManager;
    private WifiManager m_wifiManager;
    private PhoneStateListener m_phoneStateListener;
    private BroadcastReceiver m_networkBroadcastReceiver;


    //system general info
    private BroadcastReceiver m_generalSystemInfo;
    QtSystemInfo ()
    {
        QtNative.activity().runOnUiThread(new Runnable() {

            public void run() {
            }
        });
        m_powerManager = (PowerManager)QtNative.activity().getSystemService(Context.POWER_SERVICE);
        m_telephonyManager= (TelephonyManager) QtNative.activity().getSystemService(Context.TELEPHONY_SERVICE);
        disableLock();
    }

    public void initScreensaver ()
    {
        m_wakeLock = m_powerManager.newWakeLock(PowerManager.FULL_WAKE_LOCK, "QSystemScreenSaver");
    }

    private void disableLock ()
    {
        m_keyguardManager=(KeyguardManager)QtNative.activity().getSystemService(Context.KEYGUARD_SERVICE);
        m_keygaurdLock=m_keyguardManager.newKeyguardLock("QSystemScreenSaver");
        m_keygaurdLock.disableKeyguard();
    }
    public void setScreenSaverInhibit ()
    {
        //m_keygaurdLock.disableKeyguard();
        m_wakeLock.acquire();
    }

    public void DisableScreenSaverInhibit()
    {
        m_wakeLock.release();
        //m_keygaurdLock.reenableKeyguard();
    }

    public void initBattery()
    {
        //Battery Information
        m_chargerType=new HashMap<Integer, Integer>();
        m_chargerType.put(BatteryManager.BATTERY_PLUGGED_AC,1);
        m_chargerType.put(BatteryManager.BATTERY_PLUGGED_USB, 2);
        m_chargerType.put(0, 0);

        createBatteryBroadcastReceiver ();

        //only the first two are supported in qt
        m_Chargingstatus=new HashMap<Integer, Integer>();
        m_Chargingstatus.put(BatteryManager.BATTERY_STATUS_NOT_CHARGING,0);
        m_Chargingstatus.put(BatteryManager.BATTERY_STATUS_CHARGING, 1);
        m_Chargingstatus.put(BatteryManager.BATTERY_STATUS_DISCHARGING, -1);
        m_Chargingstatus.put(BatteryManager.BATTERY_STATUS_FULL, -1);
        m_Chargingstatus.put(BatteryManager.BATTERY_STATUS_UNKNOWN, -1);
        m_batteryIntentFilter=new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        m_batteryIntentFilter.addAction(Intent.ACTION_POWER_CONNECTED);
        m_batteryIntentFilter.addAction(Intent.ACTION_POWER_DISCONNECTED);
        QtNative.activity().registerReceiver(m_batteryInfoBroadcastReceiver,m_batteryIntentFilter);
        //end of battery
    }

    public void exitBattery ()
    {
        QtNative.activity().unregisterReceiver(m_batteryInfoBroadcastReceiver);
    }

    private void createBatteryBroadcastReceiver ()
    {
    }
    public void initDevice ()
    {
        createDeviceInfoBroadcastReceiver ();
        QtNative.activity().registerReceiver(m_deviceInfoBroadcastReceiver,new IntentFilter(AudioManager.RINGER_MODE_CHANGED_ACTION));
        QtNative.activity().registerReceiver(m_deviceInfoBroadcastReceiver,new IntentFilter(Intent.ACTION_SCREEN_OFF));
        QtNative.activity().registerReceiver(m_deviceInfoBroadcastReceiver,new IntentFilter(Intent.ACTION_SCREEN_ON));

    }

    public void exitDevice ()
    {
        try
        {
            QtNative.activity().unregisterReceiver(m_deviceInfoBroadcastReceiver);
        }
        catch(Exception e)
        {

        }
    }

    private void createDeviceInfoBroadcastReceiver ()
    {
        m_deviceInfoBroadcastReceiver=new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent)
            {
                if((intent.getAction().equals(AudioManager.RINGER_MODE_CHANGED_ACTION)))
                {
                        int mode=intent.getIntExtra(AudioManager.EXTRA_RINGER_MODE, -1);
                        profileChanged(mode);
                }

                if((intent.getAction().equals(Intent.ACTION_SCREEN_ON)))
                {
                    deviceLocked(false);
                }
                if((intent.getAction().equals(Intent.ACTION_SCREEN_OFF)))
                {
                    deviceLocked(true);

                }
            }
        };
    }

    public String imei ()
    {
        String imei="";
        if(m_telephonyManager.getDeviceId()!=null)
        {
            imei=m_telephonyManager.getDeviceId();
        }
        else
        {
            imei="";
        }
        return imei;
    }

    public String imsi ()
    {
        String imsi="";
        if(m_telephonyManager.getSubscriberId()!=null)
        {
            imsi=m_telephonyManager.getSubscriberId();
        }
        else
        {
            imsi="";
        }
        return imsi;
    }

    public boolean isKeyboardFlipOpened ()
    {
        Configuration configuration=new Configuration();
        if(configuration.hardKeyboardHidden==Configuration.HARDKEYBOARDHIDDEN_YES)
        {
                return true;
        }
        else
        {
                return false;
        }
    }

    public int inputMethodType ()
    {
        Configuration configuration=new Configuration();
        int inputMethod=0x0000001;//by default the keys will have a key and buttons
        if(configuration.keyboard==Configuration.KEYBOARD_QWERTY)
        {
            inputMethod|=0x0000004;
        }
        if(configuration.keyboard==Configuration.KEYBOARD_12KEY)
        {
            inputMethod|=0x0000002;
        }
        return inputMethod;
    }

    public int keyboardType ()
    {
        int keyboardType=0;
        Configuration configuration=new Configuration();
        if(configuration.keyboard==Configuration.KEYBOARD_QWERTY)
        {
            keyboardType|=0x0000008;
        }
        if(configuration.keyboard==Configuration.KEYBOARD_12KEY)
        {
            keyboardType|=0x0000002;
        }
        if((configuration.touchscreen==Configuration.TOUCHSCREEN_STYLUS)
            ||(configuration.touchscreen==Configuration.TOUCHSCREEN_FINGER))
        {
            keyboardType|=0x0000001;
        }
        return keyboardType;
    }

    public String manufacturer ()
    {
        return Build.MANUFACTURER;
    }

    public String model ()
    {
        return Build.MODEL;
    }

    public String productName ()
    {
        return Build.PRODUCT;
    }

    public int simStatus ()
    {
        int status=0;
        switch(m_telephonyManager.getSimState())
        {

            case TelephonyManager.SIM_STATE_READY:
                status=1;
                break;
            case TelephonyManager.SIM_STATE_PIN_REQUIRED:
            case TelephonyManager.SIM_STATE_PUK_REQUIRED:
            case TelephonyManager.SIM_STATE_NETWORK_LOCKED:
                status=3;
                break;
            default:
                status=0;
        }
        return status;
    }

    //display
    public void initDisplay ()
    {
        createDisplayInfoBroadcastReceiver ();
        m_display=QtNative.activity().getWindowManager().getDefaultDisplay();
        m_displaymatrics=new DisplayMetrics();
        QtNative.activity().getWindowManager().getDefaultDisplay().getMetrics(m_displaymatrics);
        QtNative.activity().registerReceiver(m_displayInfoBroadcastReceiver,new IntentFilter(Intent.ACTION_CONFIGURATION_CHANGED));
    }

    public void exitDisplay ()
    {
        QtNative.activity().unregisterReceiver(m_displayInfoBroadcastReceiver);
    }

    private void createDisplayInfoBroadcastReceiver ()
    {
    }

    public float getDPIHeight  ()
    {
        return m_displaymatrics.ydpi;
    }

    public float getDPIWidth ()
    {
        return m_displaymatrics.xdpi;
    }

    public float physicalHeight ()
    {
        float heightInInches = (float) (m_displaymatrics.heightPixels / m_displaymatrics.ydpi * 25.4);
        return heightInInches;
    }

    public float physicalWidth ()
    {
        float widthInInches = (float) (m_displaymatrics.widthPixels / m_displaymatrics.xdpi*25.4);
        return widthInInches;
    }

    public int orientation ()
    {
        return -1;
    }

    public int backLightStatus()
    {
        int backLight=0;
        try {
            backLight = Settings.System.getInt(QtNative.activity().getContentResolver(),Settings.System.SCREEN_BRIGHTNESS);
        } catch (SettingNotFoundException e) {
            e.printStackTrace();
        }
        if((backLight<=BRIGHTNESS_ON) && (backLight>=BRIGHTNESS_DIM))
        {
            return 2;
        }
        else if((backLight>=BRIGHTNESS_OFF) && (backLight<=BRIGHTNESS_DIM))
        {
            return 1;
        }
        else if(backLight==BRIGHTNESS_OFF)
        {
            return 0;
        }
        else
        {
            return -1;
        }

    }

    public int displayBrightness ()
    {
        int backLight=0;
        try {
            backLight = Settings.System.getInt(QtNative.activity().getContentResolver(),Settings.System.SCREEN_BRIGHTNESS);
        } catch (SettingNotFoundException e) {
            e.printStackTrace();
        }
        backLight=(backLight*100)/BRIGHTNESS_ON;
        return backLight;
    }

    public float colorDepth ()
    {
        return m_displaymatrics.density;
    }
    //systeminfo
    public void initSystemGeneralInfo()
    {
        createGeneralSystemInfo ();
    }

    public void exitSystemGeneralInfo ()
    {
        try
        {
            QtNative.activity().unregisterReceiver(m_generalSystemInfo);
        }
        catch(Exception e)
        {

        }
    }

    public void createGeneralSystemInfo ()
    {
        m_generalSystemInfo=new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent)
            {
                    String language=currentLanguage();
                    languageChanged(language);
            }
        };
    }
    public String[] availableLanguages ()
    {
        Locale[] locales=Locale.getAvailableLocales();
        String[] availableLanguages =new String[locales.length];
        for(int langCounter=0;langCounter<locales.length;langCounter ++)
        {
            availableLanguages [langCounter]=locales[langCounter].getDisplayName();
        }
        return availableLanguages;
    }

    public String currentCountryCode ()
    {
        Locale locale= Locale.getDefault();
        return locale.getISO3Country();
    }
    public String currentLanguage ()
    {
        Locale locale= Locale.getDefault();
        return locale.getDisplayName();
    }

    public int[] featuresAvailable()
    {
        int[] features=new int[14];
        if(sdcardFeatureAvailable ())
        {
            features[3]=5;
        }
        if(UsbFeature ())
        {
            features[4]=6;
        }
        if(VibFeature ())
        {
            features[5]=7;
            features[6]=12;
        }
        if(m_telephonyManager.getPhoneType()!=TelephonyManager.PHONE_TYPE_NONE)
        {
            features[8]=9;
        }
        return features;
    }

    private boolean sdcardFeatureAvailable ()
    {
            File dir = new File("/sys/class");
            String list="";

            String[] children = dir.list();
            if (children == null) {
                return false;
            }
            else {
                for (int i=0; i<children.length; i++) {
                    list += children[i];
                }
            }
            list=list.trim();
            if((-1!=list.toLowerCase().indexOf("mmc")))
            {
                    return true;
            }
            return false;
    }

    private boolean UsbFeature ()
    {
        File dir = new File("/sys/class");
        String list="";

        String[] children = dir.list();
        if (children == null) {
            return false;
        }
        else {
            for (int i=0; i<children.length; i++) {
                list += children[i];
            }
        }
        list=list.trim();
        if((-1!=list.toLowerCase().indexOf("usb")))
        {
                return true;
        }
        return false;
    }

    private boolean VibFeature ()
    {
        File dir = new File("/sys/class/timed_output/vibrator/enable");
        if(dir.exists())
        {
            return true;
        }
        return false;
    }

    //storage
    public void initStorage ()
    {
        createStorageInfoBroadcastReceiver ();
        QtNative.activity().registerReceiver(m_storageInfoBroadcastReceiver,new IntentFilter(Intent.ACTION_DEVICE_STORAGE_LOW));
        QtNative.activity().registerReceiver(m_storageInfoBroadcastReceiver,new IntentFilter(Intent.ACTION_DEVICE_STORAGE_OK));
        IntentFilter mediaFilter=new IntentFilter(Intent.ACTION_MEDIA_SHARED);
        mediaFilter.addAction(Intent.ACTION_MEDIA_EJECT);
        mediaFilter.addAction(Intent.ACTION_MEDIA_MOUNTED);
        mediaFilter.addAction(Intent.ACTION_MEDIA_NOFS);
        mediaFilter.addAction(Intent.ACTION_MEDIA_REMOVED);
        mediaFilter.addAction(Intent.ACTION_MEDIA_UNMOUNTABLE);
        mediaFilter.addAction(Intent.ACTION_MEDIA_UNMOUNTED);
        QtNative.activity().registerReceiver(m_storageInfoBroadcastReceiver,mediaFilter);
    }

    public void exitStorage ()
    {
        try
        {
            QtNative.activity().unregisterReceiver(m_storageInfoBroadcastReceiver);
        }
        catch(Exception e)
        {

        }
    }

    private void createStorageInfoBroadcastReceiver ()
    {
        m_storageInfoBroadcastReceiver=new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                if((intent.getAction().equals(Intent.ACTION_DEVICE_STORAGE_LOW)))
                {
                    storageStateChanged();
                }
                else if((intent.getAction().equals(Intent.ACTION_DEVICE_STORAGE_OK)))
                {
                    storageStateChanged();
                }
                else if( (intent.getAction().equals(Intent.ACTION_MEDIA_SHARED)) ||
                    (intent.getAction().equals(Intent.ACTION_MEDIA_MOUNTED)) ||
                    (intent.getAction().equals(Intent.ACTION_MEDIA_NOFS)) )

                {
                    Uri mountPath=intent.getData();
                    logicalDriveChanged(mountPath.getEncodedPath(),true);
                }
                else if ((intent.getAction().equals(Intent.ACTION_MEDIA_REMOVED)) ||
                    (intent.getAction().equals(Intent.ACTION_MEDIA_UNMOUNTABLE)) ||
                    (intent.getAction().equals(Intent.ACTION_MEDIA_UNMOUNTED)) )
                {
                    Uri mountPath=intent.getData();
                    logicalDriveChanged(mountPath.getEncodedPath(),false);
                }
            }
        };
    }
    //network
    public void initNetwork ()
    {
        m_connectivityManager=(ConnectivityManager)QtNative.activity().
                                                            getSystemService(Context.CONNECTIVITY_SERVICE);
        m_wifiManager=(WifiManager)QtNative.activity().getSystemService(Context.WIFI_SERVICE);

        QtNative.activity().runOnUiThread(new Runnable() {

            public void run()
            {
                m_phoneStateListener=new PhoneStateListener()
                {
                    @Override
                    public void  onCellLocationChanged  (CellLocation location)
                    {
                        if(m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_GSM)
                        {
                            GsmCellLocation gsmCellLocation=(GsmCellLocation)location;
                            int cellId=gsmCellLocation.getCid();
                            QtSystemInfo.cellIdChanged(cellId);
                        }
                    }

                    @Override
                    public void  onServiceStateChanged  (ServiceState serviceState)
                    {
                        int state=0;
                        int networkmode=1;
                        if( (m_telephonyManager.isNetworkRoaming()) )
                        {
                            state=8;
                        }
                        else if(serviceState.getState()==ServiceState.STATE_EMERGENCY_ONLY)
                        {
                            state= 2;
                        }
                        else if(serviceState.getState()==ServiceState.STATE_OUT_OF_SERVICE)
                        {
                            state=1;
                        }
                        else if(serviceState.getState()==ServiceState.STATE_IN_SERVICE)
                        {
                            state=5;
                        }
                        else if(serviceState.getState()==ServiceState.STATE_POWER_OFF)
                        {
                            state=0;
                        }
                        if(m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_GSM)
                        {
                            networkmode=1;
                        }
                        else if(m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_CDMA)
                        {
                            networkmode=2;
                        }
                        String networkName="";
                        networkName=m_telephonyManager.getSimOperator();
                        QtSystemInfo.serviceStatusChanged(state,networkmode);
                        QtSystemInfo.networkNameChanged(networkmode, networkName);
                    }

                };

            }
        });
        createWifiBroadcastReceiver();
        QtNative.activity().registerReceiver(m_networkBroadcastReceiver,new IntentFilter(WifiManager.RSSI_CHANGED_ACTION));
        QtNative.activity().registerReceiver(m_networkBroadcastReceiver,new IntentFilter(WifiManager.NETWORK_STATE_CHANGED_ACTION));
        QtNative.activity().registerReceiver(m_networkBroadcastReceiver,new IntentFilter(WifiManager.WIFI_STATE_CHANGED_ACTION));
        int events=
            PhoneStateListener.LISTEN_SERVICE_STATE
                |PhoneStateListener.LISTEN_DATA_CONNECTION_STATE
                |PhoneStateListener.LISTEN_CELL_LOCATION;
        m_telephonyManager.listen(m_phoneStateListener,events);
    }

    public void exitNetwork ()
    {
        QtNative.activity().unregisterReceiver(m_networkBroadcastReceiver);
        m_telephonyManager.listen(m_phoneStateListener, 0);
    }

    private void  createWifiBroadcastReceiver ()
    {
        m_networkBroadcastReceiver=new BroadcastReceiver() {

            @Override
            public void onReceive(Context context, Intent intent)
            {
                if((intent.getAction().equals(WifiManager.RSSI_CHANGED_ACTION)))
                {
                    int strength=intent.getIntExtra(WifiManager.EXTRA_NEW_RSSI, 0);
                    strength=255+strength;
                    strength=(strength*100)/255;
                    wifiSignalStrengthChanged(strength);
                }
                else if((intent.getAction().equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)))
                {
                    String name="";
                    NetworkInfo networkInfo=intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
                    int status=WifiStatus(networkInfo.getDetailedState());
                    wifiStatusChanged(status);
                    WifiInfo info=m_wifiManager.getConnectionInfo();
                    if(m_wifiManager.getWifiState()!=WifiManager.WIFI_STATE_DISABLED)
                    {
                        name=info.getSSID();
                    }
                    networkNameChanged(4, name);
                }
                else if((intent.getAction().equals(WifiManager.WIFI_STATE_CHANGED_ACTION)))
                {
                    String name="";
                    int wifiState=intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, 0);
                    if(WifiManager.WIFI_STATE_DISABLED==wifiState)
                    {
                        wifiStatusChanged(wifiState);
                        name=" ";
                        networkNameChanged(4, name);
                        wifiSignalStrengthChanged(0);
                    }

                }
            }
        };
    }
    String networkName (int mode)
    {
        String networkName="";
        switch(mode)
        {
        case 1:
            if(m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_GSM)
            {
                networkName=m_telephonyManager.getNetworkOperatorName();
            }
            break;
        case 2:
            if(m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_CDMA)
            {
                networkName=m_telephonyManager.getNetworkOperatorName();
            }
            break;
        case 3:
            networkName="";
            break;
        case 4:
            int wifiState=m_wifiManager.getWifiState();
            if(wifiState!=WifiManager.WIFI_STATE_DISABLED)
            {
                WifiInfo info=m_wifiManager.getConnectionInfo();
                networkName=info.getSSID();
            }
            break;
        case 5:
            networkName="";
            break;
        case 7:
            break;
        case 8:
            if(m_telephonyManager.getNetworkType()==TelephonyManager.NETWORK_TYPE_GPRS)
            {
                networkName=m_telephonyManager.getNetworkOperatorName();
            }
            break;
        case 9:
            if(m_telephonyManager.getNetworkType()==TelephonyManager.NETWORK_TYPE_EDGE)
            {
                networkName=m_telephonyManager.getNetworkOperatorName();
            }
            break;
        default:
            networkName="";
        }
        return networkName;
    }

    public String macAddress (int mode)
    {
        if(mode==4)
        {
            WifiInfo info=m_wifiManager.getConnectionInfo();
            return info.getMacAddress();
        }
        else
        {
            return "";
        }
    }

    public int bluetoothStatus()
    {
        {
            return 0;
        }
    }
    public int wifiStatus ()
    {
            NetworkInfo networkInfo=m_connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
            NetworkInfo.DetailedState state=networkInfo.getDetailedState();
            return WifiStatus (state);
    }

    private int WifiStatus (NetworkInfo.DetailedState state)
    {
        int getState=0;

        if(state==NetworkInfo.DetailedState.CONNECTED)
        {
            getState=5;
        }

        else if(state==NetworkInfo.DetailedState.CONNECTING)
        {
            getState=3;
        }

        else if(state==NetworkInfo.DetailedState.DISCONNECTED)
        {
            getState=1;
        }
        else if(state==NetworkInfo.DetailedState.DISCONNECTING)
        {
            getState=0;
        }
        else if(state== NetworkInfo.DetailedState.FAILED)
        {
            getState=0;
        }
        else if(state== NetworkInfo.DetailedState.IDLE)
        {
            getState=0;
        }
        else if(state==NetworkInfo.DetailedState.SCANNING)
        {
            getState=3;
        }
        else if(state==NetworkInfo.DetailedState.SUSPENDED)
        {
            getState=7;
        }
        return getState;
    }
    public int wifiStrength ()
    {
        WifiInfo info=m_wifiManager.getConnectionInfo();
        int strength=info.getRssi();
        strength=255+strength;
        strength=(strength*100)/255;
        return strength;
    }


    public int cellId ()
    {
        GsmCellLocation gsmLocation = (GsmCellLocation)m_telephonyManager.getCellLocation();
        return gsmLocation.getCid();
    }

    public String currentMobileCountryCode ()
    {
        return m_telephonyManager.getNetworkCountryIso();
    }

    public String currentMobileNetworkCode ()
    {
        String MNC=m_telephonyManager.getSimOperator();
        return MNC;
    }

    public int currentMode ()
    {
        if(m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_GSM)
        {
            return 1;
        }
        else if (m_telephonyManager.getPhoneType()==TelephonyManager.PHONE_TYPE_CDMA)
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }

    public int locationAreaCode ()
    {
        GsmCellLocation gsmLocation = (GsmCellLocation)m_telephonyManager.getCellLocation();
        return gsmLocation.getLac();
    }


    public static native void BatteryDataUpdated(Object batteryInfo);
    //device
    public static native void bluetoothStateChanged(boolean state);
    public static native void profileChanged(int mode);
    public static native void deviceLocked (boolean deviceLocked);
    //system info
    public static native void languageChanged(String language);
    //storage info
    public static native void storageStateChanged();
    public static native void logicalDriveChanged (String mountPath,boolean added);
    //network info
    public static native void phoneSignalStrengthChanged(int strength);
    public static native void wifiSignalStrengthChanged(int strength);
    public static native void serviceStatusChanged (int status,int networkmode);
    public static native void networkStatusChanged (int status,int networkmode);
    public static native void wifiStatusChanged (int status);
    public static native void networkNameChanged(int mode,String name);
    public static native void cellIdChanged (int cellId);
}
