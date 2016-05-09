package com.cobox.cosmart.devicebridge;

import android.net.wifi.ScanResult;
import android.util.Log;

import java.util.Arrays;

/**
 * Device
 * @Auther Cocoonshu
 * @Date 2016-04-11 19:55
 */
public class Device {
    static {
        System.loadLibrary("CoSmartDevice");
    }

    public static final String HEX = "0123456789ABCDEF";

    private String SSID         = null;
    private String BSSID        = null;
    private String Capabilities = null;
    private String Password     = null;
    private byte[] MAC          = null;
    private int    Level        = 0;
    private int    Frequency    = 0;

    public static Device fromWifiScanResult(ScanResult result) {
        Device device       = new Device();
        device.SSID         = result.SSID;
        device.BSSID        = result.BSSID;
        device.Capabilities = result.capabilities;
        device.Level        = result.level;
        device.Frequency    = result.frequency;
        device.MAC          = device.readMacFromSSID();
        device.Password     = device.computePassword();
        return device;
    }

    public void updatefromWifiScanResult(ScanResult result) {
        Capabilities = result.capabilities;
        Level        = result.level;
        Frequency    = result.frequency;
    }
    private byte[] readMacFromSSID() {
        byte[] mac = new byte[6];
        String macString = null;
        if (SSID.startsWith(Config.COSMART_DEVICE_SSID_PREFIX)) {
            if (Config.ESP8266_MAC_ALGORITHM == Config.GRAP_MAC_FROM_SSID) {
                macString = SSID.substring(Config.COSMART_DEVICE_SSID_PREFIX.length(), SSID.length());
                macString = macString.toUpperCase();
                for (int i = 0; i < mac.length; i++) {
                    mac[i] = (byte) ((HEX.indexOf(macString.charAt(i * 2)) << 4)
                            | (HEX.indexOf(macString.charAt(i * 2 + 1))));
                }
            } else if (Config.ESP8266_MAC_ALGORITHM == Config.GRAP_MAC_FROM_BSSID) {
                // TODO: 2016-05-09
            }
        }
        Log.e("Cocoonshu", String.format("[readMacFromSSID] mac = %02x%02x%02x%02x%02x%02x",
                mac[0], mac[1], mac[2],
                mac[3], mac[4], mac[5]));
        return mac;
    }

    private String computePassword() {
        byte[] password = null;
        if (MAC != null) {
            password = native_computePasswordBytes(MAC);
        }

        Log.e("Cocoonshu", String.format("[computePassword] pwd = %02x%02x%02x%02x%02x%02x",
                password[0], password[1], password[2],
                password[3], password[4], password[5]));
        return String.format("%02x%02x%02x%02x%02x%02x",
                password[0], password[1], password[2],
                password[3], password[4], password[5]).toLowerCase();
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (obj instanceof byte[]) {
            byte[] mac = (byte[]) obj;
            if (mac.length != MAC.length) {
                return false;
            }
            for (int i = 0; i < MAC.length; i++) {
                if (mac[i] != MAC[i]) {
                    return false;
                }
            }
            return true;
        } else if (obj instanceof ScanResult) {
            ScanResult scanResult = (ScanResult) obj;
            boolean    isEquals   = true;
            isEquals &= (SSID.equals(scanResult.SSID));
            isEquals &= (BSSID.equals(scanResult.BSSID));
            return isEquals;
        } else {
            return false;
        }
    }

    @Override
    public String toString() {
        return String.format("CoSmart Device: SSID(%s), PWD(%s)", this.SSID, this.Password);
    }

    public final String getSSID() {
        return SSID;
    }

    public final String getBSSID() {
        return BSSID;
    }

    public final  String getCapabilities() {
        return Capabilities;
    }

    public final String getPassword() {
        return Password;
    }

    public final byte[] getMAC() {
        return MAC;
    }

    public final String getMACString() {
        if (MAC == null) {
            return null;
        } else {
            return String.format("%02X-%02X-%02X-%02X-%02X-%02X",
                    MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
        }
    }

    public final int getLevel() {
        return Level;
    }

    public final int getFrequency() {
        return Frequency;
    }

    private native byte[] native_computePasswordBytes(byte[] mac);
    private native String native_computePassword(byte[] mac);
}
