/*
 * wifimanager.c
 *
 *  Created on: 2015-08-06 00:28:22
 *      Author: Cocoonshu
 */

#include "cosmart/wifimanager.h"
#include "cosmart/log.h"
#include "cosmart/cosmart.h"
#include "cosmart/textutils.h"
#include "cosmart/communication/commandserver.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"

// 内部函数声明
LOCAL bool  isNeedRebootAfterSetOpMode();
LOCAL char* generateAPSSID();
LOCAL void  deleteAPSSID(char** pAPSSID);
LOCAL char* generateAPPassword();
LOCAL void  deleteAPPassword(char** pAPPassword);
LOCAL void  onWifiEventReceived(System_Event_t* event);

///////////////
//  外部函数   //
///////////////
void ICACHE_FLASH_ATTR WiFi_initialize() {
	Log_printfln(LOG_EMPTY);
	Log_printfln(LOG_WIFI_START_WIFI_MANAGER);
}

void ICACHE_FLASH_ATTR WiFi_setupAPAndSTA() {
	uint8 opmode = wifi_get_opmode();
	Log_printfln(LOG_WIFI_SETUP_AP_STA_MDOE);
	if (opmode != STATIONAP_MODE) {
		wifi_set_opmode(STATIONAP_MODE);
		if (isNeedRebootAfterSetOpMode()) {
			Log_printfln(LOG_WIFI_SDK_TOO_LOW_TO_REBOOT);
			system_restart();
			return;
		}
	}

	struct softap_config apConfig;
	wifi_softap_get_config(&apConfig);
	char* apSSID       = generateAPSSID();
	char* apPassword   = generateAPPassword();
	if (os_strcmp(apSSID, apConfig.ssid) != 0
			|| os_strcmp(apPassword, apConfig.password) != 0) {
		Log_printfln(LOG_WIFI_SETUP_AP_CONFIGED);

		os_memset(apConfig.ssid, 0x00, 32);
		os_memset(apConfig.password, 0x00, 64);
		os_memcpy(&apConfig.ssid, apSSID, 32);
		os_memcpy(&apConfig.password, apPassword, 64);
		apConfig.ssid_len       = 0;
		apConfig.authmode       = AUTH_WPA_WPA2_PSK;
		apConfig.channel        = AP_CHANNEL;
		apConfig.max_connection = AP_MAX_CONNECTION_SIZE;

		wifi_softap_set_config(&apConfig);
		Log_printfln(LOG_WIFI_REBOOT_FOR_AP_CONFIG_CHANGED);
		system_restart();
	}

	Log_printfln(LOG_WIFI_UPDATING_CONFIG);
	Log_printfln(LOG_WIFI_UPDATING_CONFIG_OP_MODE,        Text_toOPModeString(opmode));
	Log_printfln(LOG_WIFI_UPDATING_CONFIG_SSID,           apConfig.ssid);
	Log_printfln(LOG_WIFI_UPDATING_CONFIG_PASSWORD,       apConfig.password);
	Log_printfln(LOG_WIFI_UPDATING_CONFIG_AUTH_MODE,      Text_toAuthModeString(apConfig.authmode));
	Log_printfln(LOG_WIFI_UPDATING_CONFIG_CHANNEL,        apConfig.channel);
	Log_printfln(LOG_WIFI_UPDATING_CONFIG_MAX_CONNECTION, apConfig.max_connection);

	deleteAPSSID(&apSSID);
	deleteAPPassword(&apPassword);
}

void ICACHE_FLASH_ATTR WiFi_setupAPRecevier() {
	wifi_set_event_handler_cb(onWifiEventReceived);
}

void ICACHE_FLASH_ATTR WiFi_setupProtocolBridge() {
	CMDServer_initialize();
	CMDServer_startCommandServer();
}

LOCAL struct station_config staConfig;

void ICACHE_FLASH_ATTR WiFi_connectAP(const char* ssid, const char* password) {
	Log_printf(LOG_WIFI_CONNECT_TO_AP, ssid);

	os_memset(&staConfig, 0x00, sizeof(struct station_config));
	staConfig.bssid_set = 0; // No check MAC of AP
	os_strcpy(staConfig.ssid, ssid);
	os_strcpy(staConfig.password, password);
	wifi_station_set_config(&staConfig);
	wifi_station_disconnect();
	if (wifi_station_connect()) {
		Log_printfln(LOG_WIFI_OPERATE_SUCCESSED);
	} else {
		Log_printfln(LOG_WIFI_OPERATE_FAILED);
	}
}

void ICACHE_FLASH_ATTR WiFi_disconnectAP() {
	Log_printfln(LOG_WIFI_DISCONNECT_TO_AP);
	uint8 connectStatus = wifi_station_get_connect_status();
}

///////////////
//  内部函数   //
///////////////
LOCAL bool ICACHE_FLASH_ATTR isNeedRebootAfterSetOpMode() {
	const char* refVersion = REF_VERSION;
	const char* sdkVersion = system_get_sdk_version();
	if (sdkVersion != 0x00) {
		while (refVersion != '\0') {
			int refAscii  = (int)(*refVersion);
			int sdkAscii  = (int)(*sdkVersion);
			int diffAscii = sdkAscii - refAscii;
			if (diffAscii == 0) {
				refVersion++;
				sdkVersion++;
			} else if (diffAscii > 0){
				// SDK > REF
				return false;
			} else if (diffAscii < 0) {
				// SDK < REF
				return true;
			}
		}
	}
	return true;
}

LOCAL char* ICACHE_FLASH_ATTR generateAPSSID() {
	uint8  apMacAddr[6]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t SSIDBytesSize = sizeof(char) * (os_strlen(SSID_PREFIX) + 12 + 1);
	char*  APSSID        = (char*)os_malloc(SSIDBytesSize);

	os_memset(APSSID, 0x00, SSIDBytesSize);
	wifi_get_macaddr(SOFTAP_IF, apMacAddr);
	os_sprintf(APSSID, SSID_FORMAT, MAC2STR(apMacAddr));

	return APSSID;
}

LOCAL void ICACHE_FLASH_ATTR deleteAPSSID(char** pAPSSID) {
	if (pAPSSID != 0x00) {
		os_free(*pAPSSID);
		*pAPSSID = 0x00;
	}
}

LOCAL char* ICACHE_FLASH_ATTR generateAPPassword() {
	uint8  apMacAddr[6]        = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8  encodedMacAddr[6]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	size_t APPasswordBytesSize = 12 + 1;
	char*  APPassword          = (char*)os_malloc(APPasswordBytesSize);

	os_memset(APPassword, 0x00, APPasswordBytesSize);
	wifi_get_macaddr(SOFTAP_IF, apMacAddr);

	// pwd[i] = (pwd[i] / 2) + (pwd[(i + 1) % 6] / 2)
	uint8 i = 0;
	for (i = 0; i < 6; i++) {
		encodedMacAddr[i] = apMacAddr[i] / 2 + apMacAddr[(i + 1) % 6] / 2;
	}

	// Reference as:
	//     sprintf(APPPassword, "%02x%02x%02x%02x%02x%02x", encodedMacAddr);
	os_sprintf(APPassword, FMT_APP_PWD, MAC2STR(encodedMacAddr));
	return APPassword;
}

LOCAL void ICACHE_FLASH_ATTR deleteAPPassword(char** pAPPassword) {
	if (pAPPassword != 0x00) {
		os_free(*pAPPassword);
		*pAPPassword = 0x00;
	}
}

LOCAL void ICACHE_FLASH_ATTR onWifiEventReceived(System_Event_t* event) {
	if (event != NULL) {
		switch (event->event) {
			case EVENT_SOFTAPMODE_STACONNECTED:{
				// [Wifi] Station: %02x:%02x:%02x:%02x:%02x:%02x(%d) connected!
				Log_printfln(LOG_WIFI_STATION_CONNECTED,
						MAC2STR(event->event_info.sta_connected.mac),
						event->event_info.sta_connected.aid);
				break;
			}
			case EVENT_SOFTAPMODE_STADISCONNECTED:{
				// [Wifi] Station: %02x:%02x:%02x:%02x:%02x:%02x(%d) disconnected!
				Log_printfln(LOG_WIFI_STATION_DISCONNECTED,
						MAC2STR(event->event_info.sta_disconnected.mac),
						event->event_info.sta_disconnected.aid);
				break;
			}
			case EVENT_STAMODE_CONNECTED:{
				// [Wifi] Connected to %s on channel %d
//				Log_printfln("[WiFi] Connected to %s on channel %d",
//						event->event_info.connected.ssid,
//						event->event_info.connected.channel);
				Log_printfln("[WiFi] EVENT_STAMODE_CONNECTED");
				break;
			}
			case EVENT_STAMODE_DISCONNECTED:{
				// [Wifi] Disconnect from %s on channel %d
				Log_printfln("[WiFi] Disconnected from %s for %s",
						event->event_info.disconnected.ssid,
						Text_toConnectReasonString(event->event_info.disconnected.reason));
				break;
			}
			case EVENT_STAMODE_GOT_IP:{
				// [Wifi] Got address:
                //       - IP:   %d.%d.%d.%d
                //       - Mask: %d.%d.%d.%d
                //       - Gate: %d.%d.%d.%d
//				Log_printfln("[WiFi] Got address:\r\n       - IP:   %d.%d.%d.%d\r\n       - Mask: %d.%d.%d.%d\r\n       - Gate: %d.%d.%d.%d\r\n",
//						IP2STR(event->event_info.got_ip.ip.addr),
//						IP2STR(event->event_info.got_ip.mask.addr),
//						IP2STR(event->event_info.got_ip.gw.addr));
				Log_printfln("[WiFi] EVENT_STAMODE_GOT_IP");
				break;
			}
			case EVENT_STAMODE_AUTHMODE_CHANGE:{
				// [Wifi] Auth mode changed from %d to %d
//				Log_printfln("[WiFi] Auth mode changed from %d to %d",
//						Text_toAuthModeString(event->event_info.auth_change.old_mode),
//						Text_toAuthModeString(event->event_info.auth_change.new_mode));
				Log_printfln("[WiFi] EVENT_STAMODE_AUTHMODE_CHANGE");
				break;
			}
			default:
				break;
		}
	}
}
