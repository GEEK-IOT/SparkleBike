/*
 * mathutils.h
 *
 *  Created on: 2016年1月20日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_MATHUTILS_H_
#define APP_INCLUDE_COSMART_MATHUTILS_H_

#include "c_types.h"

uint8  ICACHE_FLASH_ATTR clampu8(uint8 src, uint8 min, uint8 max);
uint16 ICACHE_FLASH_ATTR clampu16(uint16 src, uint16 min, uint16 max);
uint32 ICACHE_FLASH_ATTR clampf32(uint32 src, uint32 min, uint32 max);
int    ICACHE_FLASH_ATTR clampi(int src, int min, int max);
float  ICACHE_FLASH_ATTR clampf(float src, float min, float max);

#endif /* APP_INCLUDE_COSMART_MATHUTILS_H_ */
