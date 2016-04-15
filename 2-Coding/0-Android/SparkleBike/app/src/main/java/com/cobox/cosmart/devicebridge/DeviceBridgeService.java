package com.cobox.cosmart.devicebridge;

import android.app.Activity;
import android.app.Service;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Binder;
import android.os.IBinder;

import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

/**
 * DeviceBridgeService
 * @Auther Cocoonshu
 * @Date   2016-04-11 19:11:46
 */
public class DeviceBridgeService extends Service {

    private DeviceBridgeServiceBinder mServiceBinder = null;

    /**
     * DeviceBridgeServiceBinder
     * @Auther Cocoonshu
     * @Date   2016-04-11 19:11:46
     */
    public class DeviceBridgeServiceBinder extends Binder {

        public DeviceBridgeService getService() {
            return DeviceBridgeService.this;
        }

    }

    public static synchronized boolean startDeviceBridgeService(Activity sponsor) {
        if (sponsor == null) {
            return false;
        }
        Intent intent = new Intent(sponsor, DeviceBridgeService.class);
        sponsor.startService(intent);
        return true;
    }

    public static synchronized boolean stopDeviceBridgeService(Activity sponsor) {
        if (sponsor == null) {
            return false;
        }
        Intent intent = new Intent(sponsor, DeviceBridgeService.class);
        sponsor.stopService(intent);
        return true;
    }

    public static synchronized boolean bindDeviceBridgeService(Activity sponsor, ServiceConnection connectionListener) {
        if (sponsor == null || connectionListener == null) {
            return false;
        }
        Intent intent = new Intent(sponsor, DeviceBridgeService.class);
        sponsor.bindService(intent, connectionListener, Service.BIND_AUTO_CREATE);
        return true;
    }

    public static synchronized boolean unbindDeviceBridgeService(Activity sponsor, ServiceConnection connectionListener) {
        if (sponsor == null || connectionListener == null) {
            return false;
        }
        sponsor.unbindService(connectionListener);
        return true;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mServiceBinder = new DeviceBridgeServiceBinder();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mServiceBinder;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public boolean onUnbind(Intent intent) {
        return super.onUnbind(intent);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mServiceBinder = null;
    }

    /**
     * Discovery devices drivered by CoSmart system on ESP8266
     */
    public void discoveryDevice() {
        // TODO: 2016-04-11 
    }
    
    public void setOnDeviceScanListener(OnDeviceScanListener listener) {
        // TODO: 2016-04-11
    }
}
