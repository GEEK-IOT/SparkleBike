package com.cobox.cosmart.devicebridge.devicelayer;

import android.app.Service;
import android.content.Context;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;

import com.cobox.cosmart.devicebridge.Config;
import com.cobox.cosmart.devicebridge.Device;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * Created by Cocoonshu on 2016/4/27.
 */
public class ESP8266Scaner implements WifiStateBroadcastReceiver.OnNetworkStateChangedListener, WifiStateBroadcastReceiver.OnWifiRSSIChangedListener, WifiStateBroadcastReceiver.OnWifiScanedCompletedListener, WifiStateBroadcastReceiver.OnWifiStateChangedListener {

    private Context                    mContext                    = null;
    private WifiManager                mWiFiManager                = null;
    private WifiStateBroadcastReceiver mWifiStateBroadcastReceiver = null;
    private boolean                    mIsContinueScan             = false;
    private OnDeviceScanListener       mOnDeviceScanListener       = null;
    private List<Device>               mDeviceList                 = null;

    public ESP8266Scaner(Context context) {
        mContext                    = context;
        mWiFiManager                = (WifiManager) context.getSystemService(Service.WIFI_SERVICE);
        mWifiStateBroadcastReceiver = new WifiStateBroadcastReceiver();
        mDeviceList                 = new ArrayList<Device>();
        initializeComponments();
        initializeListeners();
    }

    private void initializeComponments() {
        mWifiStateBroadcastReceiver.register(mContext);
    }

    private void initializeListeners() {
        mWifiStateBroadcastReceiver.setOnNetworkStateChangedListener(this);
        mWifiStateBroadcastReceiver.setOnWifiRSSIChangedListener(this);
        mWifiStateBroadcastReceiver.setOnWifiScanedCompletedListener(this);
        mWifiStateBroadcastReceiver.setOnWifiStateChangedListener(this);
    }

    public final void setOnDeviceScanListener(OnDeviceScanListener listener) {
        mOnDeviceScanListener = listener;
    }

    public void scan() {
        mWiFiManager.startScan();
        mIsContinueScan = true;
        fireScanStartEvent();
    }

    public void cancel() {
        mIsContinueScan = false;
    }

    public void release() {
        mWifiStateBroadcastReceiver.unregister(mContext);
    }

    @Override
    public void onNetworkStateChanged(NetworkInfo info) {

    }

    @Override
    public void onWifiRSSIChanged(int rssi) {

    }

    @Override
    public void onWifiScanedCompleted() {
        // Process devices
        Iterator<ScanResult> scanResultItr = mWiFiManager.getScanResults().iterator();
        while (scanResultItr.hasNext()) {
            ScanResult result = scanResultItr.next();
            if (result.SSID.startsWith(Config.COSMART_DEVICE_SSID_PREFIX)) {

                synchronized (mDeviceList) {
                    boolean hasUpdatedResult = false;
                    for (Device device : mDeviceList) {
                        if (device.equals(result)) {
                            device.updatefromWifiScanResult(result);
                            hasUpdatedResult = true;
                            break;
                        }
                    }
                    if (!hasUpdatedResult) {
                        Device device = Device.fromWifiScanResult(result);
                        mDeviceList.add(device);
                    }
                }
            }
        }
        fireScaningEvent();

        // Rescan
        if (mIsContinueScan) {
            mWiFiManager.startScan();
        } else {
            fireScanEndEvent();
        }
    }

    @Override
    public void onWifiStateChanged(int wifiState) {

    }

    public boolean isWiFiEnabled() {
        return mWiFiManager == null ? null : mWiFiManager.isWifiEnabled();
    }

    public void setWiFiEnabled(boolean enabled) {
        if (mWiFiManager != null) {
            mWiFiManager.setWifiEnabled(enabled);
        }
    }

    private void fireScanStartEvent() {
        if (mOnDeviceScanListener != null) {
            mOnDeviceScanListener.onScanStart();
        }
    }

    private void fireScaningEvent() {
        if (mOnDeviceScanListener != null) {
            mOnDeviceScanListener.onDeviceScaned(mDeviceList);
        }
    }

    private void fireScanEndEvent() {
        if (mOnDeviceScanListener != null) {
            mOnDeviceScanListener.onScanCompleted();
        }
    }
}
