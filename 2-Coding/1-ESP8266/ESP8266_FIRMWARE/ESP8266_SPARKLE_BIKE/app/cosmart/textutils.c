/*
 * textutils.c
 *
 *  Created on: 2015年8月7日
 *      Author: 80074591
 */

#include "cosmart/textutils.h"
#include "osapi.h"

void ICACHE_FLASH_ATTR Text_copyText(char *dest, const char *src, size_t copyLenght) {
	size_t i = 0;
	for (i = 0; i < copyLenght; i++) {
		*dest = *src;
		if (*dest == '\0') {
			return;
		}
		dest++;
		src++;
	}
}

const char* ICACHE_FLASH_ATTR Text_toAuthModeString(AUTH_MODE authMode) {
	switch(authMode) {
	case AUTH_OPEN:
		return AUTHMODE_OPEN;
	case AUTH_WEP:
		return AUTHMODE_WEP;
	case AUTH_WPA_PSK:
		return AUTHMODE_WPA_PSK;
	case AUTH_WPA2_PSK:
		return AUTHMODE_WPA2_PSK;
	case AUTH_WPA_WPA2_PSK:
		return AUTHMODE_WPA_WPA2_PSK;
	default:
		return AUTHMODE_NULL;
	}
}

const char* ICACHE_FLASH_ATTR Text_toOPModeString(uint8 opMode) {
	switch(opMode) {
	case NULL_MODE:
		return OPMODE_NULL;
	case STATION_MODE:
		return OPMODE_STATION_MODE;
	case SOFTAP_MODE:
		return OPMODE_SOFTAP_MODE;
	case STATIONAP_MODE:
		return OPMODE_STATIONAP_MODE;
	default:
		return UNKNOWN;
	}
}

uint32 ICACHE_FLASH_ATTR Text_parseIPAddressString(const char* strSource, unsigned short start, unsigned short length) {
	if (length == 0)
		return 0;

	uint32 result = 0;
	char numberStr[4];
	os_bzero(numberStr, 4);

	size_t numItr = 0;
	size_t ipBitItr = 0;
	size_t end = start + length + 1;
	for (; start < end; start++) {
		const char* current = strSource + start;
		if (*current == '.' || start == (end - 1)) {
			numberStr[numItr] = '\0';
			uint8 value = atoi(numberStr);
			result |= value << ((3 - ipBitItr) * 8);
			ipBitItr++;
			numItr = 0;
		} else {
			numberStr[numItr] = *current;
			numItr++;
		}
	}

	return result;
}

const char* ICACHE_FLASH_ATTR Text_terminalPlatformEnumToString(enum TerminalPlatform platform) {
	switch (platform) {
		case Android: return ANDROID_FLATFORM_IDENTIFY;
		case IOS:     return IOS_FLATFORM_IDENTIFY;
		case PC:      return PC_FLATFORM_IDENTIFY;
		case Web:     return WEB_FLATFORM_IDENTIFY;
		default:      return UNKNOWN_FLATFORM_IDENTIFY;
	}
}

const char* ICACHE_FLASH_ATTR Text_terminalTypeEnumToString(enum TerminalType type) {
	switch (type) {
		case Monitor:   return MONITOR_TERMINAL_IDENTIFY;
		case Commander: return COMMANDER_TERMINAL_IDENTIFY;
		case Device:    return DEVICE_TERMINAL_IDENTIFY;
		default:        return UNKNOWN_TERMINAL_IDENTIFY;
	}
}

const char* ICACHE_FLASH_ATTR Text_flashSizeMap2STR(enum flash_size_map flashSizeMap) {
	switch(flashSizeMap) {
	case FLASH_SIZE_2M:
		return FLASH_SIZE_MAP_2M;
	case FLASH_SIZE_4M_MAP_256_256:
		return FLASH_SIZE_MAP_4M_256;
	case FLASH_SIZE_8M_MAP_512_512:
		return FLASH_SIZE_MAP_8M_512;
	case FLASH_SIZE_16M_MAP_512_512:
		return FLASH_SIZE_MAP_16M_512;
	case FLASH_SIZE_32M_MAP_512_512:
		return FLASH_SIZE_MAP_32M_512;
	case FLASH_SIZE_16M_MAP_1024_1024:
		return FLASH_SIZE_MAP_16M_1024;
	case FLASH_SIZE_32M_MAP_1024_1024:
		return FLASH_SIZE_MAP_32M_1024;
	default:
		return UNKNOWN;
	}
}

const char* ICACHE_FLASH_ATTR Text_physicalMode2STR(enum phy_mode physicalMode) {
	switch (physicalMode) {
	case PHY_MODE_11B:
		return PHYSICAL_MODE_11B;
	case PHY_MODE_11G:
		return PHYSICAL_MODE_11G;
	case PHY_MODE_11N:
		return PHYSICAL_MODE_11N;
	default:
		return UNKNOWN;
	}
}

const char* ICACHE_FLASH_ATTR Text_toConnectReasonString(int connectReason) {
	switch (connectReason) {
	case REASON_BEACON_TIMEOUT:
		return RST_BEACON_TIMEOUT;
	case REASON_NO_AP_FOUND:
		return RST_NO_AP_FOUND;
	case REASON_AUTH_FAIL:
		return RST_AUTH_FAIL;
	case REASON_ASSOC_FAIL:
		return RST_ASSOC_FAIL;
	case REASON_HANDSHAKE_TIMEOUT:
		return RST_HANDSHAKE_TIMEOUT;

	case REASON_UNSPECIFIED              : return "1";
	case REASON_AUTH_EXPIRE              : return "2";
	case REASON_AUTH_LEAVE               : return "3";
	case REASON_ASSOC_EXPIRE             : return "4";
	case REASON_ASSOC_TOOMANY            : return "5";
	case REASON_NOT_AUTHED               : return "6";
	case REASON_NOT_ASSOCED              : return "7";
	case REASON_ASSOC_LEAVE              : return "8";
	case REASON_ASSOC_NOT_AUTHED         : return "9";
	case REASON_DISASSOC_PWRCAP_BAD      : return "10";
	case REASON_DISASSOC_SUPCHAN_BAD     : return "11";
	case REASON_IE_INVALID               : return "12";
	case REASON_MIC_FAILURE              : return "13";
	case REASON_4WAY_HANDSHAKE_TIMEOUT   : return "14";
	case REASON_GROUP_KEY_UPDATE_TIMEOUT : return "15";
	case REASON_IE_IN_4WAY_DIFFERS       : return "16";
	case REASON_GROUP_CIPHER_INVALID     : return "17";
	case REASON_PAIRWISE_CIPHER_INVALID  : return "18";
	case REASON_AKMP_INVALID             : return "19";
	case REASON_UNSUPP_RSN_IE_VERSION    : return "20";
	case REASON_INVALID_RSN_IE_CAP       : return "21";
	case REASON_802_1X_AUTH_FAILED       : return "22";
	case REASON_CIPHER_SUITE_REJECTED    : return "23";
	}
}

const char* ICACHE_FLASH_ATTR Text_toResetReasonString(uint32 resetReason) {
	switch (resetReason) {
	case REASON_DEFAULT_RST:
		return RST_POWER_ON;
	case REASON_WDT_RST:
		return RST_HW_WDT_RST;
	case REASON_EXCEPTION_RST:
		return RST_EXCEPTION_RST;
	case REASON_SOFT_WDT_RST:
		return RST_SW_WDT_RST;
	case REASON_SOFT_RESTART:
		return RST_SW_RESTART;
	case REASON_DEEP_SLEEP_AWAKE:
		return RST_DEEP_SLEEP_AWAKE;
	case REASON_EXT_SYS_RST:
		return RST_EXTERNAL_SYSTEM_RST;
	}
}
