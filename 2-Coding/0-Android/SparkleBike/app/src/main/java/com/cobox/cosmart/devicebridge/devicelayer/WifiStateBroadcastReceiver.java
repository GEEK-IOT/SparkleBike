package com.cobox.cosmart.devicebridge.devicelayer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;

public class WifiStateBroadcastReceiver extends BroadcastReceiver {

    public static interface OnWifiStateChangedListener {
        void onWifiStateChanged(int wifiState);
    }

    public static interface OnWifiScanedCompletedListener {
        void onWifiScanedCompleted();
    }

    public static interface OnWifiRSSIChangedListener {
        void onWifiRSSIChanged(int rssi);
    }

    public static interface OnNetworkStateChangedListener {
        void onNetworkStateChanged(NetworkInfo info);
    }

    private static String[] ListenActions = new String[] {
        WifiManager.WIFI_STATE_CHANGED_ACTION,
        WifiManager.SCAN_RESULTS_AVAILABLE_ACTION,
        WifiManager.RSSI_CHANGED_ACTION,
        WifiManager.NETWORK_STATE_CHANGED_ACTION
    };
    
    private IntentFilter                  mIntentFilter                  = null;
    private OnWifiStateChangedListener    mOnWifiStateChangedListener    = null;
    private OnWifiScanedCompletedListener mOnWifiScanedCompletedListener = null;
    private OnWifiRSSIChangedListener     mOnWifiRSSIChangedListener     = null;
    private OnNetworkStateChangedListener mOnNetworkStateChangedListener = null;
    
    public WifiStateBroadcastReceiver() {
        mIntentFilter = new IntentFilter();
        for (int i = 0; i < ListenActions.length; i++) {
            mIntentFilter.addAction(ListenActions[i]);
        }
    }
    
    public final IntentFilter getIntentFilter() {
        return mIntentFilter;
    }

    public void register(Context context) {
        if (context != null) {
            context.registerReceiver(this, mIntentFilter);
        }
    }

    public void unregister(Context context) {
        if (context != null) {
            context.unregisterReceiver(this);
        }
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent != null) {
            String action = intent.getAction();
            if (action != null) {
                if (action.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                    onWifiStateChanged(intent);
                } else if (action.equals(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
                    onWifiAPScanedCompleted();
                } else if (action.equals(WifiManager.RSSI_CHANGED_ACTION)) {
                    onRSSIChanged(intent);
                } else if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
                    onNetworkStateChanged(intent);
                }
            }
        }
    }
    
    private void onWifiStateChanged(Intent intent) {
        if (intent != null) {
            int wifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN);
            if (mOnWifiStateChangedListener != null) {
                mOnWifiStateChangedListener.onWifiStateChanged(wifiState);
            }
        }
    }

    private void onWifiAPScanedCompleted() {
        if (mOnWifiScanedCompletedListener != null) {
            mOnWifiScanedCompletedListener.onWifiScanedCompleted();
        }
    }

    private void onRSSIChanged(Intent intent) {
        if (intent != null) {
            // dBm
            int rssi = intent.getIntExtra(WifiManager.EXTRA_NEW_RSSI, 0);
            if (mOnWifiRSSIChangedListener != null) {
                mOnWifiRSSIChangedListener.onWifiRSSIChanged(rssi);
            }
        }
    }

    private void onNetworkStateChanged(Intent intent) {
        if (intent != null) {
            NetworkInfo info = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
            if (mOnNetworkStateChangedListener != null) {
                mOnNetworkStateChangedListener.onNetworkStateChanged(info);
            }
        }
    }

    public void setOnWifiStateChangedListener(OnWifiStateChangedListener listener) {
        mOnWifiStateChangedListener = listener;
    }

    public void setOnWifiScanedCompletedListener(OnWifiScanedCompletedListener listener) {
        mOnWifiScanedCompletedListener = listener;
    }

    public void setOnWifiRSSIChangedListener(OnWifiRSSIChangedListener listener) {
        mOnWifiRSSIChangedListener = listener;
    }
    
    public void setOnNetworkStateChangedListener(OnNetworkStateChangedListener listener) {
        mOnNetworkStateChangedListener = listener;
    }

}
