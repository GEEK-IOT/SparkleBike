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
	case AUTH_OPEN         : return AUTHMODE_OPEN;
	case AUTH_WEP          : return AUTHMODE_WEP;
	case AUTH_WPA_PSK      : return AUTHMODE_WPA_PSK;
	case AUTH_WPA2_PSK     : return AUTHMODE_WPA2_PSK;
	case AUTH_WPA_WPA2_PSK : return AUTHMODE_WPA_WPA2_PSK;
	default                : return AUTHMODE_NULL;
	}
}

const char* ICACHE_FLASH_ATTR Text_toOPModeString(uint8 opMode) {
	switch(opMode) {
	case NULL_MODE      : return OPMODE_NULL;
	case STATION_MODE   : return OPMODE_STATION_MODE;
	case SOFTAP_MODE    : return OPMODE_SOFTAP_MODE;
	case STATIONAP_MODE : return OPMODE_STATIONAP_MODE;
	default             : return UNKNOWN;
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

uint32 ICACHE_FLASH_ATTR Text_parsePortString(const char* src, unsigned short start, unsigned short length) {
	if (length == 0)
		return 0;

	uint32 result = 0;
	char numberStr[length + 1];
	os_bzero(numberStr, length + 1);

	int first = -1;
	int last  = length - 1;
	int i     = 0;
	for (i = first; i < length; i++){
		if (*(src + i) >= '0' && *(src + i) <= '9') {
			if (first == -1) {
				first = i;
			}
			numberStr[i - first] = *(src + i);
		} else {
			if (first > -1) {
				last = i;
				numberStr[i - first] = '\0';
				break;
			}
		}

	}

	return atoi(numberStr);
}

const char* ICACHE_FLASH_ATTR Text_terminalPlatformEnumToString(enum TerminalPlatform platform) {
	switch (platform) {
		case Android : return ANDROID_FLATFORM_IDENTIFY;
		case IOS     : return IOS_FLATFORM_IDENTIFY;
		case PC      : return PC_FLATFORM_IDENTIFY;
		case Web     : return WEB_FLATFORM_IDENTIFY;
		default      : return UNKNOWN_FLATFORM_IDENTIFY;
	}
}

const char* ICACHE_FLASH_ATTR Text_terminalTypeEnumToString(enum TerminalType type) {
	switch (type) {
		case Monitor   : return MONITOR_TERMINAL_IDENTIFY;
		case Commander : return COMMANDER_TERMINAL_IDENTIFY;
		case Device    : return DEVICE_TERMINAL_IDENTIFY;
		default        : return UNKNOWN_TERMINAL_IDENTIFY;
	}
}

const char* ICACHE_FLASH_ATTR Text_flashSizeMap2STR(enum flash_size_map flashSizeMap) {
	switch(flashSizeMap) {
	case FLASH_SIZE_2M                : return FLASH_SIZE_MAP_2M;
	case FLASH_SIZE_4M_MAP_256_256    : return FLASH_SIZE_MAP_4M_256;
	case FLASH_SIZE_8M_MAP_512_512    : return FLASH_SIZE_MAP_8M_512;
	case FLASH_SIZE_16M_MAP_512_512   : return FLASH_SIZE_MAP_16M_512;
	case FLASH_SIZE_32M_MAP_512_512   : return FLASH_SIZE_MAP_32M_512;
	case FLASH_SIZE_16M_MAP_1024_1024 : return FLASH_SIZE_MAP_16M_1024;
	case FLASH_SIZE_32M_MAP_1024_1024 : return FLASH_SIZE_MAP_32M_1024;
	default                           : return UNKNOWN;
	}
}

const char* ICACHE_FLASH_ATTR Text_physicalMode2STR(enum phy_mode physicalMode) {
	switch (physicalMode) {
	case PHY_MODE_11B : return PHYSICAL_MODE_11B;
	case PHY_MODE_11G : return PHYSICAL_MODE_11G;
	case PHY_MODE_11N : return PHYSICAL_MODE_11N;
	default           : return UNKNOWN;
	}
}

const char* ICACHE_FLASH_ATTR Text_toConnectReasonString(int connectReason) {
	switch (connectReason) {
	case REASON_UNSPECIFIED              : return RST_UNSPECIFIED;
	case REASON_AUTH_EXPIRE              : return RST_AUTH_EXPIRE;
	case REASON_AUTH_LEAVE               : return RST_AUTH_LEAVE;
	case REASON_ASSOC_EXPIRE             : return RST_ASSOC_EXPIRE;
	case REASON_ASSOC_TOOMANY            : return RST_ASSOC_TOOMANY;
	case REASON_NOT_AUTHED               : return RST_NOT_AUTHED;
	case REASON_NOT_ASSOCED              : return RST_NOT_ASSOCED;
	case REASON_ASSOC_LEAVE              : return RST_ASSOC_LEAVE;
	case REASON_ASSOC_NOT_AUTHED         : return RST_ASSOC_NOT_AUTHED;
	case REASON_DISASSOC_PWRCAP_BAD      : return RST_ASSOC_NOT_AUTHED;
	case REASON_DISASSOC_SUPCHAN_BAD     : return RST_DISASSOC_SUPCHAN_BAD;
	case REASON_IE_INVALID               : return RST_IE_INVALID;
	case REASON_MIC_FAILURE              : return RST_MIC_FAILURE;
	case REASON_4WAY_HANDSHAKE_TIMEOUT   : return RST_4WAY_HANDSHAKE_TIMEOUT;
	case REASON_GROUP_KEY_UPDATE_TIMEOUT : return RST_GROUP_KEY_UPDATE_TIMEOUT;
	case REASON_IE_IN_4WAY_DIFFERS       : return RST_IE_IN_4WAY_DIFFERS;
	case REASON_GROUP_CIPHER_INVALID     : return RST_GROUP_CIPHER_INVALID;
	case REASON_PAIRWISE_CIPHER_INVALID  : return RST_PAIRWISE_CIPHER_INVALID;
	case REASON_AKMP_INVALID             : return RST_AKMP_INVALID;
	case REASON_UNSUPP_RSN_IE_VERSION    : return RST_UNSUPP_RSN_IE_VERSION;
	case REASON_INVALID_RSN_IE_CAP       : return RST_INVALID_RSN_IE_CAP;
	case REASON_802_1X_AUTH_FAILED       : return RST_802_1X_AUTH_FAILED;
	case REASON_CIPHER_SUITE_REJECTED    : return RST_CIPHER_SUITE_REJECTED;
	case REASON_BEACON_TIMEOUT           : return RST_BEACON_TIMEOUT;
	case REASON_NO_AP_FOUND              : return RST_NO_AP_FOUND;
	case REASON_AUTH_FAIL                : return RST_AUTH_FAIL;
	case REASON_ASSOC_FAIL               : return RST_ASSOC_FAIL;
	case REASON_HANDSHAKE_TIMEOUT        : return RST_HANDSHAKE_TIMEOUT;
	default                              : return RST_UNSPECIFIED;
	}
}

const char* ICACHE_FLASH_ATTR Text_toResetReasonString(uint32 resetReason) {
	switch (resetReason) {
	case REASON_DEFAULT_RST      : return RST_POWER_ON;
	case REASON_WDT_RST          : return RST_HW_WDT_RST;
	case REASON_EXCEPTION_RST    : return RST_EXCEPTION_RST;
	case REASON_SOFT_WDT_RST     : return RST_SW_WDT_RST;
	case REASON_SOFT_RESTART     : return RST_SW_RESTART;
	case REASON_DEEP_SLEEP_AWAKE : return RST_DEEP_SLEEP_AWAKE;
	case REASON_EXT_SYS_RST      : return RST_EXTERNAL_SYSTEM_RST;
	}
}

int ICACHE_FLASH_ATTR Text_indexOfOrder(const char* src, const char mod, bool forward, int order) {
	if (src == NULL) {
		return -1;
	}
	int loopSize = os_strlen(src);
	int start    = 0;
	int end      = 0;
	int count    = 0;
	if (forward) {
		for (start = 0, end = loopSize; start < end; start++) {
			if (*(src + start) == mod) {
				if (count == order) {
					return start;
				} else {
					count++;
				}
			}
		}
	} else {
		for (start = loopSize - 1, end = -1; start > end; start--) {
			if (*(src + start) == mod) {
				if (count == order) {
					return start;
				} else {
					count++;
				}
			}
		}
	}
	return -1;
}

char* ICACHE_FLASH_ATTR Text_asciiToUtf8(const char* ascii) {
	if (ascii == NULL)
		return NULL;

	uint16 size    = os_strlen(ascii);
	uint8* utf8    = (uint8*) os_malloc(size + 2);
	uint8* pointer = utf8;

	*(pointer + 0) = size >> 8;
	*(pointer + 1) = size & 0xFF;
	os_memcpy(pointer + 2, ascii, size);
	return (char*)utf8;
}

uint16 ICACHE_FLASH_ATTR Text_utf8Length(const char* utf8) {
	if (utf8 == NULL) {
		return 0;
	} else {
		uint16 size = (((*(utf8 + 0)) << 8) & 0xFF00)
				    | (((*(utf8 + 1)) << 0) & 0x00FF);
		return size + 2;
	}
}
