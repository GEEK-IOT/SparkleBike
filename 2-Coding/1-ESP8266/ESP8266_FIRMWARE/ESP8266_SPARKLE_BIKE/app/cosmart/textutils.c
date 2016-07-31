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

bool ICACHE_FLASH_ATTR Text_compareText(const char* ref, const char *src, unsigned short start, unsigned short length) {
	if (length == 0 && (ref == NULL || os_strlen(ref) == 0)) {
		return true;
	}

	size_t refLength = os_strlen(ref);
	if (refLength != length) {
		return false;
	}
	size_t i = 0;
	for (i = 0; start < length; start++, i++) {
		if (*(ref + i) != *(src + start)) {
			return false;
		}
	}
	return true;
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
	char   numberStr[4];
	os_memset(numberStr, '\0', 4);

	size_t numItr   = 0;
	size_t ipBitItr = 0;
	size_t end      = start + length + 1;
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

uint32 ICACHE_FLASH_ATTR Text_parsePortString(const char* strSource, unsigned short start, unsigned short length) {
	if (length == 0)
		return 0;

	char numberStr[length + 1];
	os_memset(numberStr, '\0', length + 1);

	size_t numItr = 0;
	size_t end    = start + length + 1;
	for (; start < end; start++, numItr++) {
		numberStr[numItr] = *(strSource + start);
	}

	return atoi(numberStr);
}

enum TerminalType ICACHE_FLASH_ATTR Text_parseTerminalTypeString(const char* strSource, unsigned short start, unsigned short length) {
	if (length == 0) {
		return UnknownType;
	}

	if (Text_compareText(IDC_FLATFORM_ANDROID, strSource, start, length)) {
		return Android;
	} else if (Text_compareText(IDC_FLATFORM_IOS, strSource, start, length)) {
		return IOS;
	} else if (Text_compareText(IDC_FLATFORM_PC, strSource, start, length)) {
		return PC;
	} else if (Text_compareText(IDC_FLATFORM_WEB, strSource, start, length)) {
		return Web;
	} else {
		return UnknownType;
	}
}

enum TerminalPlatform ICACHE_FLASH_ATTR Text_parseTerminalPlatformString(const char* strSource, unsigned short start, unsigned short length) {
	if (length == 0) {
		return UnknownPlatform;
	}

	if (Text_compareText(IDC_TERMINAL_MONITOR, strSource, start, length)) {
		return Monitor;
	} else if (Text_compareText(IDC_TERMINAL_COMMANDER, strSource, start, length)) {
		return Commander;
	} else if (Text_compareText(IDC_TERMINAL_DEVICE, strSource, start, length)) {
		return Device;
	} else {
		return UnknownPlatform;
	}
}

const char* ICACHE_FLASH_ATTR Text_terminalPlatformEnumToString(enum TerminalPlatform platform) {
	switch (platform) {
		case Android: return IDC_FLATFORM_ANDROID;
		case IOS:     return IDC_FLATFORM_IOS;
		case PC:      return IDC_FLATFORM_PC;
		case Web:     return IDC_FLATFORM_WEB;
		default:      return IDC_FLATFORM_UNKNOWN;
	}
}

const char* ICACHE_FLASH_ATTR Text_terminalTypeEnumToString(enum TerminalType type) {
	switch (type) {
		case Monitor:   return IDC_TERMINAL_MONITOR;
		case Commander: return IDC_TERMINAL_COMMANDER;
		case Device:    return IDC_TERMINAL_DEVICE;
		default:        return IDC_TERMINAL_UNKNOWN;
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
