/*
 * commandexecutor.c
 *
 *  Created on: 2016年7月3日
 *      Author: Cocoonshu
 */

#include "cosmart/communication/commandexecutor.h"
#include "c_types.h"
#include "osapi.h"
#include "mem.h"

LOCAL const int    mMethodSize = 5;
LOCAL const Method mMethods[]  = {
		{"connect",       CMDExecutor_conect,        CMDDecoder_conect,        CMDEncoder_conect},        // 连接CommanderBridge
		{"disconnect",    CMDExecutor_disconnect,    CMDDecoder_disconnect,    CMDEncoder_disconnect},    // 断开CommanderBridge
		{"getSystemInfo", CMDExecutor_getSystemInfo, CMDDecoder_getSystemInfo, CMDEncoder_getSystemInfo}, // 获取系统信息
		{"joinNetwork",   CMDExecutor_joinNetwork,   CMDDecoder_joinNetwork,   CMDEncoder_joinNetwork},   // 加入局域网
		{"reboot",        CMDExecutor_reboot,        CMDDecoder_reboot,        CMDEncoder_reboot}         // 重启
};

void ICACHE_FLASH_ATTR CMDExecutor_conect(Request* inRequest, Response* outResponse) {
	// 解析终端
	ConnectRequest* request = inRequest->parameters->request;
	if (request != NULL) {
		ConnectRequest* params = inRequest->parameters->request;
		// 验证终端
		if (CMDServer_verifyTerminal(inRequest->requestID, params->securityCode)) {
			int         subIndex  = -1;
			uint32      ipAddress = params->broadcastIP;
			const char* platform  = params->platform;

			// 开始与终端建立连接
			Terminal* terminal = (Terminal *) os_zalloc(sizeof(Terminal));
			MEMZERO(terminal, sizeof(Terminal));

			terminal->terminalIP[0] = (ipAddress >> 24) & 0xFF;
			terminal->terminalIP[1] = (ipAddress >> 16) & 0xFF;
			terminal->terminalIP[2] = (ipAddress >>  8) & 0xFF;
			terminal->terminalIP[3] = (ipAddress >>  0) & 0xFF;
			terminal->terminalPort = params->commandPort;

			// COSMART_IDENTIFY
			subIndex = os_strstr(platform, COSMART_IDENTIFY);
			if (subIndex != -1) {
				// PLATFORM_IDENTIFY
				if ((subIndex = os_strstr(platform, ANDROID_FLATFORM_IDENTIFY)) != -1) {
					terminal->terminalPlatform = Android;
				} else if ((subIndex = os_strstr(platform, IOS_FLATFORM_IDENTIFY)) != -1) {
					terminal->terminalPlatform = IOS;
				} else if ((subIndex = os_strstr(platform, PC_FLATFORM_IDENTIFY)) != -1) {
					terminal->terminalPlatform = PC;
				} else if ((subIndex = os_strstr(platform, WEB_FLATFORM_IDENTIFY)) != -1) {
					terminal->terminalPlatform = Web;
				} else {
					terminal->terminalPlatform = UnknownPlatform;
				}

				// TYPE_IDENTIFY
				if ((subIndex = os_strstr(platform, MONITOR_TERMINAL_IDENTIFY)) != -1) {
					terminal->terminalType = Monitor;
				} else if ((subIndex = os_strstr(platform, COMMANDER_TERMINAL_IDENTIFY)) != -1) {
					terminal->terminalType = Commander;
				} else if ((subIndex = os_strstr(platform, DEVICE_TERMINAL_IDENTIFY)) != -1) {
					terminal->terminalType = Device;
				} else {
					terminal->terminalType = UnknownType;
				}
			}

			CMDServer_connectTerminal(terminal);
		} else {
			// 拒绝登陆
			outResponse->result->response->result       = false;
			outResponse->requestState->stateCode        = State_OK;
			outResponse->requestState->stateMessage     = NULL;
			outResponse->requestState->stateDescription = NULL;
		}
	}
}

void ICACHE_FLASH_ATTR CMDExecutor_disconnect(Request* inRequest, Response* outResponse) {

}

void ICACHE_FLASH_ATTR CMDExecutor_getSystemInfo(Request* inRequest, Response* outResponse) {

}

void ICACHE_FLASH_ATTR CMDExecutor_joinNetwork(Request* inRequest, Response* outResponse) {

}

void ICACHE_FLASH_ATTR CMDExecutor_reboot(Request* inRequest, Response* outResponse) {

}

const int ICACHE_FLASH_ATTR CMDExecutor_getMethodSize() {
	return mMethodSize;
}
const Method* ICACHE_FLASH_ATTR CMDExecutor_getMethods() {
	return mMethods;
}
