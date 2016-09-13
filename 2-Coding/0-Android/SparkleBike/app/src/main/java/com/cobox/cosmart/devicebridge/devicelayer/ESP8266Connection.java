package com.cobox.cosmart.devicebridge.devicelayer;

import com.cobox.cosmart.devicebridge.Device;

/**
 * ESP8266Connection
 * @Auther Cocoonshu
 * @Date   2016-06-21 16:00:50
 */
public class ESP8266Connection extends DeviceConnection {

    public ESP8266Connection(Device device) {
        super(device);
    }

    @Override
    public void connect() {

    }

    @Override
    public void disconnect() {

    }

    @Override
    protected boolean hasLANRestored() {
        return false;
    }

    @Override
    protected void sendLANCertificate() {

    }

    @Override
    protected boolean confirmLANCertificateWithDevice() {
        return false;
    }

    @Override
    protected void confirmGuideToLANCommand() {

    }

    @Override
    protected void switchToLAN() {

    }
}
