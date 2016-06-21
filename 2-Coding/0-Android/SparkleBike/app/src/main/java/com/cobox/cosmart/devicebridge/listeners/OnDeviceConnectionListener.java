package com.cobox.cosmart.devicebridge.listeners;

import com.cobox.cosmart.devicebridge.Device;

/**
 * OnDeviceConnectionListener
 * @Auther Cocoonshu
 * @Date   2016-06-21 15:48:49
 */
public interface OnDeviceConnectionListener {
    void onDeviceConnecting(Device device);
    void onDeviceConnected(Device device);
    void onDeviceDisconnecting(Device device);
    void onDeviceDisconnected(Device device);
    void onDeviceIPGot(Device device);
    void onDeviceAuthorizating(Device device);
    void onDeviceAuthorizated(Device device);
    void onDeviceAuthorizateFailed(Device device);
}
