/*
 * commandserver.c
 *
 *  Created on: 2015年9月22日
 *      Author: Cocoonshu
 */

#include "cosmart/communication/commandserver.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cosmart/config.h"
#include "cosmart/log.h"

LOCAL void   startBroadcastReceiver();
LOCAL void   stopBroadcastReceiver();
LOCAL void   onBroadcastReceived(void *espconn, char *pdata, unsigned short length);
LOCAL void   onUDPBridgeReceived(void *espconn, char *pdata, unsigned short length);
LOCAL void   onUDPBridgeConnected(void *espconn);
LOCAL void   onCommand(Terminal* terminal, const char* command);

LOCAL Connection* mBroadcastReceiverConnection = NULL;
LOCAL Terminal*   mSTACommandTerminal          = NULL;
LOCAL Terminal*   mAPCommandTerminal           = NULL;

void ICACHE_FLASH_ATTR CMDServer_initialize() {
	Log_printfln("");
	Log_printfln("[CMD] Start command server");
}

void ICACHE_FLASH_ATTR CMDServer_startCommandServer() {
	startBroadcastReceiver();
}

void ICACHE_FLASH_ATTR CMDServer_stopCommandServer() {
	stopBroadcastReceiver();
}

/**
 * Connect to a terminal
 * @return if operation success, zero will be returned,
 *         if this connection has connected, a positive will be return,
 *         if operation failed, a negative will be return.
 */
int ICACHE_FLASH_ATTR CMDServer_connectTerminal(Terminal* terminal) {
	if (terminal != NULL) {
		int   isSuccessed = 0;
		sint8 result      = 0;

		// 初始化UDP指令桥参数包
		Connection* udpConnection = (Connection *) os_zalloc(sizeof(Connection));
		udpConnection->type = ESPCONN_UDP;
		udpConnection->proto.udp = (UDP *) os_zalloc(sizeof(UDP));
		os_memcpy(udpConnection->proto.udp->remote_ip, terminal->terminalIP, 4);
		udpConnection->proto.udp->remote_port = terminal->terminalPort;
		udpConnection->proto.udp->local_port  = espconn_port();
		terminal->connection = udpConnection;

		// 开启UDP指令桥连接接受器
		result = espconn_regist_connectcb(udpConnection, onUDPBridgeConnected);
		result = espconn_regist_recvcb(udpConnection, onUDPBridgeReceived);
		result = espconn_create(udpConnection);
		if (result == ESPCONN_ARG) {
			// FIXME create UDP command bridge goes wrong
			CMDServer_disconnectTerminal(terminal);
			isSuccessed = -1;
			Log_printfln("[CMD] Start command bridge");
			Log_printfln("[CMD] Create UDP command bridge failed, Connection arguments are wrong!");
		} else if (result == ESPCONN_MEM) {
			// FIXME create UDP command bridge goes wrong
			CMDServer_disconnectTerminal(terminal);
			isSuccessed = -1;
			Log_printfln("[CMD] Start command bridge");
			Log_printfln("[CMD] Create UDP command bridge failed, Memory is low!");
		} else if (result == ESPCONN_ISCONN) {
			isSuccessed = 1;
			Log_printfln("[CMD] Start command bridge");
			Log_printfln("[CMD] This UDP command connection has already connected!");
		} else {
			CMDServer_disconnectTerminal(terminal);
			isSuccessed = -1;
			Log_printfln("[CMD] Start command bridge");
			Log_printfln("[CMD] Create UDP command bridge failed! Error code = %d", result);
		}

		// 判断结果
		if (isSuccessed < 0) {
			Log_printfln("[CMD] Start command bridge failed!");
			Log_printfln("     - Terminal : %d.%d.%d.%d:%d", IPARRAY2STR(terminal->terminalIP), terminal->terminalPort);
			Log_printfln("     - Platform : %s", Text_terminalPlatformEnumToString(terminal->terminalPlatform));
			Log_printfln("     - Type     : %s", Text_terminalTypeEnumToString(terminal->terminalType));
		} else {
			Log_printfln("[CMD] Start command bridge successed! Waiting for callback...");
			//Log_printfln("     - Terminal : %d.%d.%d.%d:%d", IPARRAY2STR(terminal->terminalIP), terminal->terminalPort);
			//Log_printfln("     - Platform : %s", Text_terminalPlatformEnumToString(terminal->terminalPlatform));
			//Log_printfln("     - Type     : %s", Text_terminalTypeEnumToString(terminal->terminalType));
		}

		return isSuccessed;
	} else {
		Log_printfln("[CMD] Start command bridge failed, Terminal is NULL!");
		return false;
	}
}

void ICACHE_FLASH_ATTR CMDServer_disconnectTerminal(Terminal* terminal) {
	if (terminal != NULL) {
		espconn_disconnect(terminal->connection);
		espconn_delete(terminal->connection);
		FREEMEM(terminal->connection->proto.udp);
		FREEMEM(terminal->connection);
		terminal->connection = NULL;
		FREEMEM(terminal);
	}
}

bool ICACHE_FLASH_ATTR CMDServer_verifyTerminal(const char* requestID, const char* securityCode) {
	// FIXME
	return true;
}

LOCAL void ICACHE_FLASH_ATTR startBroadcastReceiver() {
	bool  isSuccessed = TRUE;
	sint8 result      = 0;

	// 初始化UDP广播监听器参数包
	mBroadcastReceiverConnection->type = ESPCONN_UDP;
	mBroadcastReceiverConnection->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	mBroadcastReceiverConnection->proto.udp->local_port = DEFAULT_CMD_PORT;

	// 开启UDP广播接受，开始搜寻Commander
	result = espconn_regist_recvcb(mBroadcastReceiverConnection, onBroadcastReceived);
	if (result != 0) {
		// FIXME register received listener goes wrong
		stopBroadcastReceiver();
		isSuccessed = FALSE;
		Log_printfln("[CMD] Start UDP broadcast receiver");
		Log_printfln("[CMD] Register UDP broadcast receiver callback failed! Error code = %d", result);
	}
	result = espconn_create(mBroadcastReceiverConnection);
	if (result != 0) {
		// FIXME create UDP broadcast receiver goes wrong
		stopBroadcastReceiver();
		isSuccessed = FALSE;
		Log_printfln("[CMD] Start UDP broadcast receiver");
		Log_printfln("[CMD] Create UDP broadcast receiver failed! Error code = %d", result);
	}

	// 判断结果
	if (!isSuccessed) {
		Log_printfln("[CMD] Start UDP broadcast receiver failed!");
	} else {
		Log_printfln("[CMD] Start UDP broadcast receiver successed!");
	}
}

LOCAL void ICACHE_FLASH_ATTR stopBroadcastReceiver() {
	espconn_delete(mBroadcastReceiverConnection);
	FREEMEM(mBroadcastReceiverConnection->proto.udp);
	mBroadcastReceiverConnection->proto.udp = NULL;
}

LOCAL void ICACHE_FLASH_ATTR onBroadcastReceived(void *espconn, char *data, unsigned short length) {
	/**
	 * Broadcast Message protocol:
	 *   #Version1:
	 *     [BROADCAST_IP]:[BROADCAST_PORT]:[cosmart.PLATFORM.NODETYPE]:[CMD_PORT]
	 *     192.168.4.2:7629:cosmart.android.commander:7630
	 */
	Log_printfln("[CMD] Broadcast: content = %s, length = %d", data, length);

	/**
	 * 处理流程：
	 *   STEP1 解析此消息为Terminal
	 *  *STEP2 连接到Terminal，如果Terminal无效，则STEP3，否则STEP4
	 *   STEP3 向Terminal发送断开消息，并断开连接，销毁Terminal，退出流程
	 *   STEP4 如果有已连接的Command客户端，发送断开消息，并断开连接
	 *   STEP5 设置此Terminal为当前Command客户端
	 */

	Terminal* terminal = (Terminal*) os_zalloc(sizeof(Terminal));
	os_memset(terminal, 0, sizeof(Terminal));

	{// 解析此消息为Terminal
		int i        = 0;
		int startPos = 0;
		int endPos   = 0;

		// 搜寻':' 0.0.0.0:
		for (i = 0; i < length; i++) {
			if (*(data + i) == ':') {
				endPos = i;
				break;
			}
		}
		// 解析IP (0.0.0.0) -> (000.000.000.000)
		{
			int ipAddrLength = endPos - startPos;
			if (ipAddrLength >= 7 && ipAddrLength <= 15) {
				uint32 ipDigit = Text_parseIPAddressString(data, startPos, ipAddrLength);
				terminal->terminalIP[0] = (ipDigit & 0xFF000000) >> 24;
				terminal->terminalIP[1] = (ipDigit & 0x00FF0000) >> 16;
				terminal->terminalIP[2] = (ipDigit & 0x0000FF00) >> 8;
				terminal->terminalIP[3] = (ipDigit & 0x000000FF) >> 0;
			} else {
				terminal->terminalIP[0] = 0;
				terminal->terminalIP[1] = 0;
				terminal->terminalIP[2] = 0;
				terminal->terminalIP[3] = 0;
				FREEMEM(terminal);
				return;
			}
		}
		// 搜索':' 0.0.0.0:0000
		startPos = endPos + 1;
		endPos   = endPos + 1;
		for (i = endPos + 1; i < length; i++) {
			if (*(data + i) == ':') {
				endPos = i;
				break;
			}
		}
		// 解析广播端口
		{
			int ipPortLength = endPos - startPos;
			if (ipPortLength > 0) {
				uint32 portDigit = Text_parsePortString(data, startPos, ipPortLength);
				terminal->terminalBoardcast = portDigit;
			} else {
				terminal->terminalBoardcast = 0;
				FREEMEM(terminal);
				return;
			}
		}
		// 搜索':' 0.0.0.0:0000:cosmart.android.commander:
		startPos = endPos + 1;
		endPos   = endPos + 1;
		for (i = endPos + 1; i < length; i++) {
			char* letter = data + i;
			if (*(data + i) == ':') {
				endPos = i;
				break;
			}
		}
		// 解析平台标识符 x.x.x
		{
			int flagCounter = 0;
			int markPos     = startPos;
			int flagLength  = endPos - startPos;
			if (flagLength >= 5) {
				for (i = startPos; i < endPos; i++) {
					char* currChar = data + i;
					if (*currChar == '.') {
						if (flagCounter == 0) {
							// 此节为统一标识符"cosmart"
							if (!Text_compareText(IDC_COSMART, data, markPos, i - markPos)) {
								FREEMEM(terminal);
								return;
							}
						} else if (flagCounter == 1) {
							// 此节为客户端平台标识符
							terminal->terminalPlatform = Text_parseTerminalPlatformString(data, data, markPos, i - markPos);
						} else if (flagCounter == 2) {
							// 此节为客户端类型标识符
							terminal->terminalType = Text_parseTerminalTypeString(data, data, markPos, i - markPos);
						}

						markPos = i + 1;
						flagCounter ++;
					}
				}
			} else {
				FREEMEM(terminal);
				return;
			}
		}
		// 搜索':'或'\0' 0.0.0.0:0000:cosmart.android.commander:0000
		startPos = endPos + 1;
		endPos   = endPos + 1;
		for (i = endPos + 1; i < length; i++) {
			char* letter = data + i;
			if (*(data + i) == ':' || *(data + i) == '\0') {
				endPos = i;
				break;
			}
		}
		// 解析通信端口
		{
			int ipPortLength = endPos - startPos;
			if (ipPortLength > 0) {
				uint32 portDigit = Text_parsePortString(data, startPos, ipPortLength);
				terminal->terminalPort = portDigit;
			} else {
				terminal->terminalPort = 0;
				FREEMEM(terminal);
				return;
			}
		}
	}

	{
		// 连接到Terminal，如果Terminal有效，断开前一个客户端，连接新的客户端*
		int result = CMDServer_connectTerminal(terminal);
		if (result == 0) {
			// 等待onUDPBridgeConnected
			if (mAPCommandTerminal != NULL) {
				CMDServer_disconnectTerminal(mAPCommandTerminal);
				mAPCommandTerminal = terminal;
			}
		} else if (result == 1) {
			FREEMEM(terminal);
		} else {
			FREEMEM(terminal);
		}
	}
}

LOCAL void ICACHE_FLASH_ATTR onUDPBridgeConnected(void *espconn) {
//	int i = 0;
//	for (; i < MAX_TERMINAL_SIZE; i++) {
//		Terminal* terminal = mConnectingTerminalList[i];
//		if (terminal != NULL) {
//			if (terminal->connection == espconn) {
//				terminal->generation = getGenerationID();
//				if (!moveTerminalToConnectedList(terminal)) {
//					CMDServer_disconnectTerminal(terminal);
//					terminal = NULL;
//					return;
//				}
//
//				char* connectConfirmMessage = "Confirm command bridge connect";
//				espconn_sendto(terminal->connection, connectConfirmMessage, os_strlen(connectConfirmMessage)); // FIXME
//				Log_printfln("[CMD] Command bridge connected!");
//				Log_printfln("     - Terminal : %d.%d.%d.%d:%d", IPARRAY2STR(terminal->terminalIP), terminal->terminalPort);
//				Log_printfln("     - Platform : %s", Text_terminalPlatformEnumToString(terminal->terminalPlatform));
//				Log_printfln("     - Type     : %s", Text_terminalTypeEnumToString(terminal->terminalType));
//				break;
//			}
//		}
//		continue;
//	}
	if (mAPCommandTerminal != NULL && mAPCommandTerminal->connection == espconn) {
		// 发送连接确认信息
		Log_printfln("[CMD] Command bridge connected!");
		Log_printfln("     - Terminal : %d.%d.%d.%d:%d", IPARRAY2STR(mAPCommandTerminal->terminalIP), mAPCommandTerminal->terminalPort);
		Log_printfln("     - Platform : %s", Text_terminalPlatformEnumToString(mAPCommandTerminal->terminalPlatform));
		Log_printfln("     - Type     : %s", Text_terminalTypeEnumToString(mAPCommandTerminal->terminalType));

		char* connectConfirmMessage = "Confirm command bridge connect";
		espconn_sendto(mAPCommandTerminal->connection, connectConfirmMessage, os_strlen(connectConfirmMessage));
	}
}

LOCAL void ICACHE_FLASH_ATTR onUDPBridgeReceived(void *espconn, char *data, unsigned short length) {
	Log_printfln("[CMD] UDP Bridge: PData = %s, length = %d", data, length);
}

