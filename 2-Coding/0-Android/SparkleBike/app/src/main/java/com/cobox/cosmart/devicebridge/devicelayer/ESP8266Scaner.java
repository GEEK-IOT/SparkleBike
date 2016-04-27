package com.cobox.cosmart.devicebridge.devicelayer;

import android.content.Context;
import android.net.NetworkInfo;

/**
 * Created by Cocoonshu on 2016/4/27.
 */
public class ESP8266Scaner implements WifiStateBroadcastReceiver.OnNetworkStateChangedListener, WifiStateBroadcastReceiver.OnWifiRSSIChangedListener, WifiStateBroadcastReceiver.OnWifiScanedCompletedListener, WifiStateBroadcastReceiver.OnWifiStateChangedListener {

    private Context                    mContext                    = null;
    private WifiStateBroadcastReceiver mWifiStateBroadcastReceiver = null;

    public ESP8266Scaner(Context context) {
        mContext = context;
        mWifiStateBroadcastReceiver = new WifiStateBroadcastReceiver();
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

    public void scan() {
    }

    public void cancel() {
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

    }

    @Override
    public void onWifiStateChanged(int wifiState) {

    }
}
