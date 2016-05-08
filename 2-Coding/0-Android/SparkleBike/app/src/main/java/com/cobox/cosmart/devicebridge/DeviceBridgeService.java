package com.cobox.cosmart.devicebridge;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

import com.cobox.cosmart.devicebridge.devicelayer.ESP8266Scaner;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

import java.util.LinkedList;
import java.util.List;

/**
 * DeviceBridgeService
 * @Auther Cocoonshu
 * @Date   2016-04-11 19:11:46
 */
class DeviceBridgeService extends Service {

    private DeviceBridgeServiceBinder  mServiceBinder         = null;
    private List<OnDeviceScanListener> mOnDeviceScanListeners = null;
    private ESP8266Scaner              mESP8266Scaner         = null;

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

    @Override
    public void onCreate() {
        super.onCreate();
        mServiceBinder         = new DeviceBridgeServiceBinder();
        mOnDeviceScanListeners = new LinkedList<OnDeviceScanListener>();
        mESP8266Scaner         = new ESP8266Scaner(getApplicationContext());
        mESP8266Scaner.setOnDeviceScanListener(new OnDeviceScanListener() {

            @Override
            public void onScanStart() {
                for (OnDeviceScanListener listener : mOnDeviceScanListeners) {
                    listener.onScanStart();
                }
            }

            @Override
            public void onDeviceScaned(List<Device> deviceList) {
                for (OnDeviceScanListener listener : mOnDeviceScanListeners) {
                    listener.onDeviceScaned(deviceList);
                }
            }

            @Override
            public void onScanCompleted() {
                for (OnDeviceScanListener listener : mOnDeviceScanListeners) {
                    listener.onScanCompleted();
                }
            }

        });
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
        mOnDeviceScanListeners.clear();
        mESP8266Scaner.release();
        mServiceBinder = null;
        mOnDeviceScanListeners = null;
        mESP8266Scaner = null;
    }

    /**
     * Discover devices drivered by CoSmart system on ESP8266
     */
    public void discoverDevice() {
        mESP8266Scaner.scan();
    }

    /**
     * Stop Discovering devices drivered by CoSmart system on ESP8266
     */
    public void cancelDiscoverDevice() {
        mESP8266Scaner.cancel();
    }

    public void addOnDeviceScanListener(OnDeviceScanListener listener) {
        mOnDeviceScanListeners.add(listener);
    }

    public void removeOnDeviceScanListener(OnDeviceScanListener listener) {
        mOnDeviceScanListeners.remove(listener);
    }

    public void setWiFiEnabld(boolean enabled) {
        if (mESP8266Scaner != null) {
            mESP8266Scaner.setWiFiEnabled(enabled);
        }
    }

    public boolean isWiFiEnabled() {
        return mESP8266Scaner == null ? null : mESP8266Scaner.isWiFiEnabled();
    }
}
