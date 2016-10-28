/*
 * textutils.h
 *
 *  Created on: 2015年8月7日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_TEXTUTILS_H_
#define APP_INCLUDE_COSMART_TEXTUTILS_H_

#include "c_types.h"
#include "user_interface.h"
#include "cosmart/cosmart.h"
#include "cosmart/communication/commandserver.h"

/**
 * 拷贝src到dest
 * 最多拷贝copyLenght个字符,
 * 如果src到'\0', 拷贝也会结束
 */
void Text_copyText(char *dest, const char *src, size_t copyLenght);

/**
 * 把AUTH_MODE转为字符串
 */
const char* Text_toAuthModeString(AUTH_MODE authMode);

/**
 * 把OPMode转为字符串
 */
const char* Text_toOPModeString(uint8 opMode);

/**
 * 字符串IP转换为uint32格式的IP
 */
uint32 Text_parseIPAddressString(const char* strSource, unsigned short start, unsigned short length);

uint32 Text_parsePortString(const char* strSource, unsigned short start, unsigned short length);

const char* Text_terminalPlatformEnumToString(enum TerminalPlatform platform);

const char* Text_terminalTypeEnumToString(enum TerminalType type);

const char* Text_flashSizeMap2STR(enum flash_size_map flashSizeMap);

const char* Text_physicalMode2STR(enum phy_mode physicalMode);

const char* Text_toConnectReasonString(int connectReason);

int Text_indexOfOrder(const char* strSource, const char mod, bool forward, int order);

#endif /* APP_INCLUDE_COSMART_TEXTUTILS_H_ */
