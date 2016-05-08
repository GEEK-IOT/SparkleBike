package com.cobox.cosmart.devicebridge;


public class Config {

    public static String COSMART_DEVICE_SSID_PREFIX = "CoSmart";
    
    public static class Protocol {
        public static final String CosmartIndentify        = "cosmart";
        public static final String TerminalPlatformAndroid = "android";
        public static final String TerminalPlatformIOS     = "ios";
        public static final String TerminalPlatformPC      = "pc";
        public static final String TerminalPlatformWeb     = "web";
        public static final String TerminalTypeCommander   = "commander";
        public static final String TerminalTypeMonitor     = "monitor";
        public static final String TerminalTypeNode        = "device";
    }
    
    public static class Network {
        public static String MulticastLockTag        = "CoSmartMulticastLockTag";
        public static String BroadcastLockTag        = "CoSmartBroadcastLockTag";
        public static int    BroadcastPort           = 7629;
        public static int    CMDBridgePort           = 7630;
        public static int    UDPServerBufferSize     = 1024;
        public static int    UDPClientBufferSize     = 1024;
        public static int    UDPServerReceiveTimeout = 10000;
        public static int    UDPClientReceiveTimeout = 10000;
    }
    
}
