/*
 * CommandServer.h
 *
 *  Created on: 2015年9月22日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_COMMANDSERVER_H_
#define APP_INCLUDE_COSMART_COMMANDSERVER_H_

#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"

#define MAX_TERMINAL_SIZE           4

#define COSMART_IDENTIFY            "cosmart"
#define ANDROID_FLATFORM_IDENTIFY   "android"
#define IOS_FLATFORM_IDENTIFY       "ios"
#define PC_FLATFORM_IDENTIFY        "pc"
#define WEB_FLATFORM_IDENTIFY       "web"
#define UNKNOWN_FLATFORM_IDENTIFY   "unknown"
#define MONITOR_TERMINAL_IDENTIFY   "monitor"
#define COMMANDER_TERMINAL_IDENTIFY "commander"
#define DEVICE_TERMINAL_IDENTIFY    "device"
#define UNKNOWN_TERMINAL_IDENTIFY   "unknown"
#define COLUMN_BROADCAST_IP         0
#define COLUMN_BROADCAST_PORT       1
#define COLUMN_BROADCAST_MSG        2
#define COLUMN_CMD_PORT             3

#ifndef MEMZERO
	#define MEMZERO(ptr, len) {if (ptr != NULL) {os_bzero(ptr, len);}}
#endif

#ifndef FREEMEM
	#define FREEMEM(ptr) {if (ptr != NULL) {os_free(ptr);}}
#endif

typedef struct espconn Connection;
typedef esp_udp        UDP;

enum TerminalPlatform {
	UnknownPlatform = 0,
	Android,
	IOS,
	PC,
	Web
};

enum TerminalType {
	UnknownType = 0,
	Monitor,
	Commander,
	Device
};

typedef struct {
	enum TerminalPlatform terminalPlatform;
	enum TerminalType     terminalType;
	uint8                 terminalIP[4];
	int                   terminalPort;
	unsigned long         generation;
	Connection*           connection;
} Terminal;

void CMDServer_initialize();
void CMDServer_startCommandServer();
void CMDServer_stopCommandServer();
bool CMDServer_verifyTerminal(const char* requestID, const char* securityCode);
void CMDServer_connectTerminal(Terminal* terminal);
void CMDServer_disconnectTerminal(Terminal* terminal);

#endif /* APP_INCLUDE_COSMART_COMMANDSERVER_H_ */
