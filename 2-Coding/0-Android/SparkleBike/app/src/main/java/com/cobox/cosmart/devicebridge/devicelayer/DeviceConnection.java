package com.cobox.cosmart.devicebridge.devicelayer;

import com.cobox.cosmart.devicebridge.Device;

/**
 * ESP8266Connection
 * @Auther Cocoonshu
 * @Date   2016-06-21 15:55:41
 */
public abstract class DeviceConnection {

    private Device mRefDevice = null;

    public DeviceConnection(Device device) {
        mRefDevice = device;
    }

    public abstract void connect();

    public abstract void disconnect();

    protected abstract boolean hasLANRestored();

    protected abstract void sendLANCertificate();

    protected abstract boolean confirmLANCertificateWithDevice();

    protected abstract void confirmGuideToLANCommand();

    protected abstract void switchToLAN();

}
