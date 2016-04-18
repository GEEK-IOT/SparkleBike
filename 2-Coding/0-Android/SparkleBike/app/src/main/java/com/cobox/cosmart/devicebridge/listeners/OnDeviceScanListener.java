package com.cobox.cosmart.devicebridge.listeners;

import com.cobox.cosmart.devicebridge.Device;

import java.util.List;

/**
 * OnDeviceScanListener
 * @Auther Cocoonshu
 * @Date   2016-04-11 19:51
 */
public interface OnDeviceScanListener {
    void onScanStart();
    void onDeviceScaned(List<Device> deviceList);
    void onScanCompleted();
}
