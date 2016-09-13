/*
 * mathutils.c
 *
 *  Created on: 2016年1月20日
 *      Author: Cocoonshu
 */
#include "cosmart/mathutils.h"

uint8 ICACHE_FLASH_ATTR clampu8(uint8 src, uint8 min, uint8 max) {
	return src > max ? max : (src < min ? min : src);
}

uint16 ICACHE_FLASH_ATTR clampu16(uint16 src, uint16 min, uint16 max) {
	return src > max ? max : (src < min ? min : src);
}

uint32 ICACHE_FLASH_ATTR clampf32(uint32 src, uint32 min, uint32 max) {
	return src > max ? max : (src < min ? min : src);
}

int ICACHE_FLASH_ATTR clampi(int src, int min, int max) {
	return src > max ? max : (src < min ? min : src);
}

float ICACHE_FLASH_ATTR clampf(float src, float min, float max) {
	return src > max ? max : (src < min ? min : src);
}
