/*
 * cosmart.h
 *
 *  Created on: 2015年8月7日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_COSMART_H_
#define APP_INCLUDE_COSMART_COSMART_H_

#include "c_types.h"

#define COSMART_VERSION        "v1.00["__DATE__"]"
#define SSID_PREFIX            "CoSmart"
#define SSID_FORMAT            "CoSmart%02x%02x%02x%02x%02x%02x"
#define COSMART_INDENTIFY      "cosmart"
#define AP_CHANNEL             5
#define AP_MAX_CONNECTION_SIZE 4

#ifndef ICACHE_DATA_ATTR
#define ICACHE_DATA_ATTR //ICACHE_RODATA_ATTR
#endif

// APPLICATION CONSTACTS
static const char UNKNOWN                                 [] ICACHE_DATA_ATTR = {"Unknown"};
static const char REF_VERSION                             [] ICACHE_DATA_ATTR = {"0.9.2"};
static const char FMT_APP_PWD                             [] ICACHE_DATA_ATTR = {"%02x%02x%02x%02x%02x%02x"};
static const char BOOT_MODE_ENHANCE                       [] ICACHE_DATA_ATTR = {"ENHANCE"};
static const char BOOT_MODE_NORMAL                        [] ICACHE_DATA_ATTR = {"NORMAL"};
static const char PHYSICAL_MODE_11B                       [] ICACHE_DATA_ATTR = {"802.11b"};
static const char PHYSICAL_MODE_11G                       [] ICACHE_DATA_ATTR = {"802.11g"};
static const char PHYSICAL_MODE_11N                       [] ICACHE_DATA_ATTR = {"802.11n"};
static const char FLASH_SIZE_MAP_2M                       [] ICACHE_DATA_ATTR = {"2M  "};
static const char FLASH_SIZE_MAP_4M_256                   [] ICACHE_DATA_ATTR = {"4M (256 + 256)"};
static const char FLASH_SIZE_MAP_8M_512                   [] ICACHE_DATA_ATTR = {"8M (512 + 512)"};
static const char FLASH_SIZE_MAP_16M_512                  [] ICACHE_DATA_ATTR = {"16M (512 + 512)"};
static const char FLASH_SIZE_MAP_16M_1024                 [] ICACHE_DATA_ATTR = {"16M (1024 + 1024)"};
static const char FLASH_SIZE_MAP_32M_512                  [] ICACHE_DATA_ATTR = {"32M (512 + 512)"};
static const char FLASH_SIZE_MAP_32M_1024                 [] ICACHE_DATA_ATTR = {"32M (1024 + 1024)"};
static const char OPMODE_NULL                             [] ICACHE_DATA_ATTR = {"Null"};
static const char OPMODE_STATION_MODE                     [] ICACHE_DATA_ATTR = {"Station"};
static const char OPMODE_SOFTAP_MODE                      [] ICACHE_DATA_ATTR = {"SoftAP"};
static const char OPMODE_STATIONAP_MODE                   [] ICACHE_DATA_ATTR = {"SoftAP & Station"};
static const char AUTHMODE_NULL                           [] ICACHE_DATA_ATTR = {"NULL"};
static const char AUTHMODE_OPEN                           [] ICACHE_DATA_ATTR = {"OPEN"};
static const char AUTHMODE_WEP                            [] ICACHE_DATA_ATTR = {"Null"};
static const char AUTHMODE_WPA_PSK                        [] ICACHE_DATA_ATTR = {"Null"};
static const char AUTHMODE_WPA2_PSK                       [] ICACHE_DATA_ATTR = {"Null"};
static const char AUTHMODE_WPA_WPA2_PSK                   [] ICACHE_DATA_ATTR = {"Null"};

// IDENTIFY
static const char IDC_COSMART                             [] ICACHE_DATA_ATTR = {"cosmart"};
static const char IDC_FLATFORM_ANDROID                    [] ICACHE_DATA_ATTR = {"android"};
static const char IDC_FLATFORM_IOS                        [] ICACHE_DATA_ATTR = {"ios"};
static const char IDC_FLATFORM_PC                         [] ICACHE_DATA_ATTR = {"pc"};
static const char IDC_FLATFORM_WEB                        [] ICACHE_DATA_ATTR = {"web"};
static const char IDC_FLATFORM_UNKNOWN                    [] ICACHE_DATA_ATTR = {"unknown"};
static const char IDC_TERMINAL_MONITOR                    [] ICACHE_DATA_ATTR = {"monitor"};
static const char IDC_TERMINAL_COMMANDER                  [] ICACHE_DATA_ATTR = {"commander"};
static const char IDC_TERMINAL_DEVICE                     [] ICACHE_DATA_ATTR = {"device"};
static const char IDC_TERMINAL_UNKNOWN                    [] ICACHE_DATA_ATTR = {"unknown"};

// APPLICATION LOGS
static const char LOG_EMPTY                               []                    = {""};
static const char LOG_ONE_LINE                            []                    = {"\r\n"};
static const char LOG_TWO_LINE                            []                    = {"\r\n\r\n"};

static const char LOG_SYS_INFO                            [] ICACHE_DATA_ATTR = {"[CoSmart] System Information:"};
static const char LOG_SYS_INFO_CHIP_ID                    [] ICACHE_DATA_ATTR = {"       Chip ID       : %d"};
static const char LOG_SYS_INFO_FLASH_ID                   [] ICACHE_DATA_ATTR = {"       Flash ID      : %d"};
static const char LOG_SYS_INFO_BOOT_VERSION               [] ICACHE_DATA_ATTR = {"       Boot Version  : %d"};
static const char LOG_SYS_INFO_BOOT_MODE                  [] ICACHE_DATA_ATTR = {"       Boot Mode     : %s"};
static const char LOG_SYS_INFO_BOOT_ADDRESS               [] ICACHE_DATA_ATTR = {"       Boot Address  : %d"};
static const char LOG_SYS_INFO_SDK_VERSION                [] ICACHE_DATA_ATTR = {"       SDK Version   : %s"};
static const char LOG_SYS_INFO_ROM_VERSION                [] ICACHE_DATA_ATTR = {"       ROM Version   : %s"};
static const char LOG_SYS_INFO_CPU_FREQUENCY              [] ICACHE_DATA_ATTR = {"       CPU Frequency : %d MHz"};
static const char LOG_SYS_INFO_WIFI_PROTOCOL              [] ICACHE_DATA_ATTR = {"       WiFi Protocol : %s"};
static const char LOG_SYS_INFO_RTC_CLOCK                  [] ICACHE_DATA_ATTR = {"       RTC Clock     : %d.%d us"};
static const char LOG_SYS_INFO_FREE_HEAP                  [] ICACHE_DATA_ATTR = {"       Free Heap     : %d bytes"};
static const char LOG_SYS_INFO_FLASH_SIZE                 [] ICACHE_DATA_ATTR = {"       Flash Size    : %s"};
static const char LOG_SYS_INFO_SYSTEM_VOLT                [] ICACHE_DATA_ATTR = {"       System Volt   : %d.%d V (%fV)"};
static const char LOG_SYS_INFO_PWM_VERSION                [] ICACHE_DATA_ATTR = {"       PWN Version   : %d"};
static const char LOG_SYS_INFO_AP_MAC                     [] ICACHE_DATA_ATTR = {"       AP MAC        : %02x:%02x:%02x:%02x:%02x:%02x"};
static const char LOG_SYS_INFO_STA_MAC                    [] ICACHE_DATA_ATTR = {"       STA MAC       : %02x:%02x:%02x:%02x:%02x:%02x"};

static const char LOG_TIMER_INITIALIZED                   [] ICACHE_DATA_ATTR = {"[Timer] Timer initialized"};
static const char LOG_TIMER_SETUP_US_SCALE                [] ICACHE_DATA_ATTR = {"       - Timer is under micro-second scale"};

static const char LOG_WIFI_START_WIFI_MANAGER             [] ICACHE_DATA_ATTR = {"[WiFi] Start WiFi Manager"};
static const char LOG_WIFI_SETUP_AP_STA_MDOE              [] ICACHE_DATA_ATTR = {"[WiFi] Setup AP and STA mixed mode"};
static const char LOG_WIFI_SDK_TOO_LOW_TO_REBOOT          [] ICACHE_DATA_ATTR = {"[WiFi] SDK Version is lower than 0.9.2, need to reboot aftering setting OpMode\r\n"};
static const char LOG_WIFI_SETUP_AP_CONFIGED              [] ICACHE_DATA_ATTR = {"[WiFi] Setup AP configuration"};
static const char LOG_WIFI_REBOOT_FOR_AP_CONFIG_CHANGED   [] ICACHE_DATA_ATTR = {"[WiFi] AP configuration changed, need to reboot"};
static const char LOG_WIFI_UPDATING_CONFIG                [] ICACHE_DATA_ATTR = {"[WiFi] Updating configuration"};
static const char LOG_WIFI_UPDATING_CONFIG_OP_MODE        [] ICACHE_DATA_ATTR = {"       - OP Mode       : %s"};
static const char LOG_WIFI_UPDATING_CONFIG_SSID           [] ICACHE_DATA_ATTR = {"       - SSID          : %s"};
static const char LOG_WIFI_UPDATING_CONFIG_PASSWORD       [] ICACHE_DATA_ATTR = {"       - Password      : %s"};
static const char LOG_WIFI_UPDATING_CONFIG_AUTH_MODE      [] ICACHE_DATA_ATTR = {"       - Auth Mode     : %s"};
static const char LOG_WIFI_UPDATING_CONFIG_CHANNEL        [] ICACHE_DATA_ATTR = {"       - Channel       : %d"};
static const char LOG_WIFI_UPDATING_CONFIG_MAX_CONNECTION [] ICACHE_DATA_ATTR = {"       - MaxConnection : %d"};
static const char LOG_WIFI_STATION_CONNECTED              [] ICACHE_DATA_ATTR = {"[WiFi] Station: %02x:%02x:%02x:%02x:%02x:%02x(%d) connected!"};
static const char LOG_WIFI_STATION_DISCONNECTED           [] ICACHE_DATA_ATTR = {"[WiFi] Station: %02x:%02x:%02x:%02x:%02x:%02x(%d) disconnected!"};

// JSON KEYWORDS
//static const char KEY_REQUEST_ID        [] ICACHE_DATA_ATTR = {"requestID"};
//static const char KEY_RESPONSE_ID       [] ICACHE_DATA_ATTR = {"responseID"};
//static const char KEY_RETURN            [] ICACHE_DATA_ATTR = {"return"};
//static const char KEY_RESPONSE_STATE    [] ICACHE_DATA_ATTR = {"responseState"};
//static const char KEY_METHOD            [] ICACHE_DATA_ATTR = {"method"};
//static const char KEY_PARAMETERS        [] ICACHE_DATA_ATTR = {"parameters"};
//static const char KEY_BROADCAST_IP      [] ICACHE_DATA_ATTR = {"broadcastIP"};
//static const char KEY_BROADCAST_PORT    [] ICACHE_DATA_ATTR = {"broadcastPort"};
//static const char KEY_PLATFORM          [] ICACHE_DATA_ATTR = {"platform"};
//static const char KEY_COMMAND_PORT      [] ICACHE_DATA_ATTR = {"commandPort"};
//static const char KEY_SECURITY_CODE     [] ICACHE_DATA_ATTR = {"securityCode"};
//static const char KEY_STATE_CODE        [] ICACHE_DATA_ATTR = {"stateCode"};
//static const char KEY_STATE_MESSAGE     [] ICACHE_DATA_ATTR = {"stateMessage"};
//static const char KEY_STATE_DESCRIPTION [] ICACHE_DATA_ATTR = {"stateDescription"};

void Cosmart_initialize();

#endif /* APP_INCLUDE_COSMART_COSMART_H_ */
