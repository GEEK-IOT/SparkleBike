package com.cobox.cosmart.devicebridge;

import android.app.Activity;
import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import com.cobox.cosmart.devicebridge.DeviceBridgeService.DeviceBridgeServiceBinder;

import com.cobox.cosmart.devicebridge.listeners.OnBridgeConnectionListener;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceConnectionListener;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

import java.util.List;

/**
 * DeviceBridge
 * @Auther Cocoonshu
 * @Date   2016-04-11 19:11:46
 */
public class DeviceBridge {

    private DeviceBridgeService        mService                    = null;
    private ServiceConnection          mServiceConnectionListener  = null;
    private OnDeviceScanListener       mOnDeviceScanListenerAgent  = null;
    private OnDeviceScanListener       mOnDeviceScanListener       = null;
    private OnBridgeConnectionListener mOnBridgeConnectionListener = null;

    public DeviceBridge() {
        initializeListeners();
    }

    private void initializeListeners() {
        mServiceConnectionListener = new ServiceConnection() {

            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                mService = ((DeviceBridgeServiceBinder)service).getService();
                mService.addOnDeviceScanListener(mOnDeviceScanListenerAgent);
                if (mOnBridgeConnectionListener != null) {
                    mOnBridgeConnectionListener.onBridgeConnected();
                }
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                if (mOnBridgeConnectionListener != null) {
                    mOnBridgeConnectionListener.onBridgeDisconnected();
                }
            }

        };

        mOnDeviceScanListenerAgent = new OnDeviceScanListener() {

            @Override
            public void onScanStart() {
                if (mOnDeviceScanListener != null) {
                    mOnDeviceScanListener.onScanStart();
                }
            }

            @Override
            public void onDeviceScaned(List<Device> deviceList) {
                if (mOnDeviceScanListener != null) {
                    mOnDeviceScanListener.onDeviceScaned(deviceList);
                }
            }

            @Override
            public void onScanCompleted() {
                if (mOnDeviceScanListener != null) {
                    mOnDeviceScanListener.onScanCompleted();
                }
            }

        };
    }

    private synchronized boolean bindDeviceBridgeService(Activity sponsor, ServiceConnection connectionListener) {
        if (sponsor == null || connectionListener == null) {
            return false;
        }
        Intent intent = new Intent(sponsor, DeviceBridgeService.class);
        sponsor.bindService(intent, connectionListener, Service.BIND_AUTO_CREATE);
        return true;
    }

    private synchronized boolean unbindDeviceBridgeService(Activity sponsor, ServiceConnection connectionListener) {
        if (sponsor == null || connectionListener == null) {
            return false;
        }
        sponsor.unbindService(connectionListener);
        return true;
    }

    public static boolean startService(Activity sponsor) {
        if (sponsor == null) {
            return false;
        }
        Intent intent = new Intent(sponsor, DeviceBridgeService.class);
        sponsor.startService(intent);
        return true;
    }

    public static boolean stopService(Activity sponsor) {
        if (sponsor == null) {
            return false;
        }
        Intent intent = new Intent(sponsor, DeviceBridgeService.class);
        sponsor.stopService(intent);
        return true;
    }

    public boolean connectService(Activity sponsor) {
        return bindDeviceBridgeService(sponsor, mServiceConnectionListener);
    }

    public boolean disconnectService(Activity sponsor) {
        mService.cancelDiscoverDevice();
        mService.removeOnDeviceScanListener(mOnDeviceScanListenerAgent);
        return unbindDeviceBridgeService(sponsor, mServiceConnectionListener);
    }

    public void setOnDeviceScanListener(OnDeviceScanListener listener) {
        mOnDeviceScanListener = listener;
    }

    public void setOnBridgeConnectionListener(OnBridgeConnectionListener listener) {
        mOnBridgeConnectionListener = listener;
    }

    public void setWiFiEnabled(boolean enabled) {
        if (mService != null) {
            mService.setWiFiEnabld(enabled);
        }
    }

    public boolean isWiFiEnabled() {
        return mService == null ? null : mService.isWiFiEnabled();
    }

    public void discoverDevice() {
        if (mService != null) {
            mService.discoverDevice();
        }
    }

    public void connectDevice(Device device, OnDeviceConnectionListener listener) {
        if (mService != null) {
            mService.connectDevice(device, listener);
        }
    }

    public void disconnectDevice(Device device) {
        if (mService != null) {
            mService.disconnectDevice(device);
        }
    }
}
