package cn.geekiot.sparklebike.adapter;

import android.util.Singleton;

import com.cobox.cosmart.devicebridge.DeviceBridge;

/**
 * Device manager
 * @Auther Cocoonshu
 * @Date 2016-10-14 10:36:05
 * Copyright (c) 2016 Cocoonshu
 */
public class DeviceManager {

    private static final Singleton<DeviceBridge> sDeviceBridge = new Singleton<DeviceBridge>() {

        @Override
        protected DeviceBridge create() {
            return new DeviceBridge();
        }

    };

    public static DeviceBridge getDeviceBridge() {
        return sDeviceBridge.get();
    }

}
