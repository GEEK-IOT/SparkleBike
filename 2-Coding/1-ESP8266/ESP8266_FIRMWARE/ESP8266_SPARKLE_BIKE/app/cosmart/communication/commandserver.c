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

LOCAL void startBroadcastReceiver();
LOCAL void stopBroadcastReceiver();
LOCAL void onBroadcastReceived(void *espconn, char *pdata, unsigned short length);
LOCAL void onUDPBridgeReceived(void *espconn, char *pdata, unsigned short length);
LOCAL void onUDPBridgeConnected(void *espconn);
LOCAL bool verifyCasterIdentity(char *data, unsigned short length);
LOCAL void responseStationBridgeVerify(char *data, unsigned short length, bool verified);
LOCAL bool sendUDPMessageOnce(uint32 ip, uint32 port, const char* message);

LOCAL uint32 mCommandMulticastGroupIP   = 0;
LOCAL uint32 mCommandMulticastGroupPort = 0;

// UDP广播接收器
LOCAL struct espconn mBroadcastReceiverConnection;

void ICACHE_FLASH_ATTR CMDServer_initialize() {
	Log_printfln("");
	Log_printfln("[CMD] Start command server");

	mCommandMulticastGroupIP   = ipaddr_addr(STATION_CMD_MULTICAST_GROUP_IP);
	mCommandMulticastGroupPort = STATION_CMD_MULTICAST_GROUP_PORT;
}

void ICACHE_FLASH_ATTR CMDServer_startCommandServer() {
	startBroadcastReceiver();
}

void ICACHE_FLASH_ATTR CMDServer_stopCommandServer() {
	stopBroadcastReceiver();
}

bool ICACHE_FLASH_ATTR CMDServer_verifyTerminal(const char* requestID, const char* securityCode) {
	// FIXME
	return true;
}

void ICACHE_FLASH_ATTR CMDServer_connectTerminal(Terminal* terminal) {
//	if (terminal != NULL) {
//		bool  isSuccessed = TRUE;
//		sint8 result      = 0;
//
//		// 初始化UDP指令桥参数包
//		Connection* udpConnection = (Connection *) os_zalloc(sizeof(Connection));
//		udpConnection->type = ESPCONN_UDP;
//		udpConnection->proto.udp = (UDP *) os_zalloc(sizeof(UDP));
//		os_memcpy(udpConnection->proto.udp->remote_ip, terminal->terminalIP, 4);
//		udpConnection->proto.udp->remote_port = terminal->terminalPort;
//		udpConnection->proto.udp->local_port  = espconn_port();
//		terminal->connection = udpConnection;
//
//		// 开启UDP指令桥连接接受器
//		result = espconn_regist_connectcb(udpConnection, onUDPBridgeConnected);
//		if (result != 0) {
//			// FIXME register connected listener goes wrong
//			CMDServer_disconnectTerminal(terminal);
//			isSuccessed = FALSE;
//			Log_printfln("[CMD] Start command bridge");
//			Log_printfln("[CMD] Register UDP command bridge connected callback failed! Error code = %d", result);
//		}
//
//		result = espconn_regist_recvcb(udpConnection, onUDPBridgeReceived);
//		if (result != 0) {
//			// FIXME register received listener goes wrong
//			CMDServer_disconnectTerminal(terminal);
//			isSuccessed = FALSE;
//			Log_printfln("[CMD] Start command bridge");
//			Log_printfln("[CMD] Register UDP command bridge received callback failed! Error code = %d", result);
//		}
//
//		result = espconn_create(udpConnection);
//		if (result != 0) {
//			// FIXME create UDP command bridge goes wrong
//			CMDServer_disconnectTerminal(terminal);
//			isSuccessed = FALSE;
//			Log_printfln("[CMD] Start command bridge");
//			Log_printfln("[CMD] Create UDP command bridge failed! Error code = %d", result);
//		}
//
//		// 判断结果
//		if (!isSuccessed) {
//			Log_printfln("[CMD] Start command bridge failed!");
//			Log_printfln("     - Terminal : %d.%d.%d.%d:%d", IPARRAY2STR(terminal->terminalIP), terminal->terminalPort);
//			Log_printfln("     - Platform : %s", Text_terminalPlatformEnumToString(terminal->terminalPlatform));
//			Log_printfln("     - Type     : %s", Text_terminalTypeEnumToString(terminal->terminalType));
//		} else {
//			Log_printfln("[CMD] Start command bridge successed! Waiting for callback...");
//			Log_printfln("     - Terminal : %d.%d.%d.%d:%d", IPARRAY2STR(terminal->terminalIP), terminal->terminalPort);
//			Log_printfln("     - Platform : %s", Text_terminalPlatformEnumToString(terminal->terminalPlatform));
//			Log_printfln("     - Type     : %s", Text_terminalTypeEnumToString(terminal->terminalType));
//		}
//	} else {
//		Log_printfln("[CMD] Start command bridge failed, Terminal is NULL!");
//	}
}

void ICACHE_FLASH_ATTR CMDServer_disconnectTerminal(Terminal* terminal) {
//	if (terminal != NULL) {
//		espconn_disconnect(terminal->connection);
//		espconn_delete(terminal->connection);
//		FREEMEM(terminal->connection->proto.udp);
//		FREEMEM(terminal->connection);
//		terminal->connection = NULL;
//
//		int i;
//		for (i = 0; i < MAX_TERMINAL_SIZE; i++) {
//			if (mConnectingTerminalList[i] == terminal) {
//				mConnectingTerminalList[i] = NULL;
//			}
//			if (mConnectedTerminalList[i] == terminal) {
//				mConnectedTerminalList[i] = NULL;
//			}
//		}
//
//		FREEMEM(terminal);
//	}
}

LOCAL void ICACHE_FLASH_ATTR startBroadcastReceiver() {
	bool  isSuccessed = TRUE;
	sint8 result      = 0;

	// 初始化UDP广播监听器参数包
	mBroadcastReceiverConnection.type = ESPCONN_UDP;
	mBroadcastReceiverConnection.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	mBroadcastReceiverConnection.proto.udp->local_port = DEFAULT_CMD_PORT;

	// 开启UDP广播接受，开始搜寻Commander
	result = espconn_regist_recvcb(&mBroadcastReceiverConnection, onBroadcastReceived);
	if (result != 0) {
		// FIXME register received listener goes wrong
		stopBroadcastReceiver();
		isSuccessed = FALSE;
		Log_printfln("[CMD] Start UDP broadcast receiver");
		Log_printfln("[CMD] Register UDP broadcast receiver callback failed! Error code = %d", result);
	}
	result = espconn_create(&mBroadcastReceiverConnection);
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
	espconn_delete(&mBroadcastReceiverConnection);
	FREEMEM(mBroadcastReceiverConnection.proto.udp);
	mBroadcastReceiverConnection.proto.udp = 0x00;
}

LOCAL void ICACHE_FLASH_ATTR onBroadcastReceived(void *espconn, char *data, unsigned short length) {
	/**
	 * Broadcast Message protocol:
	 *   #Version1:
	 *     [BROADCAST_IP]:[BROADCAST_PORT]:[CMD_PORT]:[IDENTITY]:[cosmart.PLATFORM.NODETYPE]
	 *     192.168.4.2:7629:7630:identity
	 *
	 *   #Version2:
	 *     request ->
	 *     {
	 *         requstID      : "0001",
	 *         method        : "login",
	 *         parameters    : {
	 *             broadcastIP   : "192.168.4.2",
	 *             broadcastPort : 7629,
	 *             platform      : "cosmart.android.commander",
	 *             commandPort   : 7630,
	 *             securityCode  : "j1c81jslf"
	 *         }
	 *     }
	 *     response ->
	 *     {
	 *         responseID   : "0001",
	 *         return       : true,
	 *         requestState : {
	 *             stateCode        : 0,
	 *             stateMessage     : "IllegalParameterException",
	 *             stateDescription : "at parameter broadcastPort: 7629"
	 *         }
	 *     }
	 */

	Log_printfln("[CMD] Broadcast: content = %s, length = %d", data, length);
	// 通知发广播的主机加入多播组
	// 多播地址在239.0.0.0～239.255.255.255
	responseStationBridgeVerify(data, length, verifyCasterIdentity(data, length));
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
}

LOCAL void ICACHE_FLASH_ATTR onUDPBridgeReceived(void *espconn, char *data, unsigned short length) {
	Log_printfln("[CMD] UDP Bridge: PData = %s, length = %d", data, length);
}

LOCAL bool ICACHE_FLASH_ATTR verifyCasterIdentity(char *data, unsigned short length) {
	/**
	 * Broadcast Message protocol:
	 *   #Version1:
	 *     [BROADCAST_IP]:[BROADCAST_PORT]:[CMD_PORT]:[IDENTITY]:[cosmart.PLATFORM.NODETYPE]
	 *     192.168.4.2:7629:7630:identity
	 */
	int start = Text_indexOfOrder(data, ':', true, 2);
	int end   = Text_indexOfOrder(data, ':', true, 3);
	end = (end == -1 ? length : end);
	if (start == -1 || end <= start) {
		return false;
	}

	const char* reference = WiFi_generateSTAIdentify();
	char identify[end - start] = {0};
	os_bzero(&identify, end - start);

	int i = 0;
	for (i = 0; i < end - start; i++) {
		identify[i] = data[start + i];
	}

	bool result = os_strcmp(reference, identify);
	WiFi_freeSTAIdentify(&reference);
	return result;
}

LOCAL void ICACHE_FLASH_ATTR responseStationBridgeVerify(char *data, unsigned short length, bool verified) {
	/**
	 * Broadcast Message protocol:
	 *   #Version1:
	 *      input:
	 *         [BROADCAST_IP]:[BROADCAST_PORT]:[CMD_PORT]:[IDENTITY]:[cosmart.PLATFORM.NODETYPE]
	 *         192.168.4.2:7629:7630:identity
	 *
	 *      output:
	 *         [VERIFY_MESSAGE]:[STATION_MULTICAST_GROUP_ADDRESS]:[STATION_MULTICAST_GROUP_PORT]
	 *         pass:239.120.101.32:7631
	 *         fail:0.0.0.0:0
	 */

	if (verified) {
		Log_printfln("[CMD] Caster identity verify pass, send multicast group");
	} else {
		Log_printfln("[CMD] Caster identity verify failed, reject");
	}

	uint32 ip   = 0;
	uint32 port = 0;

	int start = 0;
	int pos   = Text_indexOfOrder(data, ':', true, 0);
	if (pos > -1) {
		ip = Text_parseIPAddressString(data, start, pos - start);
	} else {
		return;
	}

	start = Text_indexOfOrder(data, ':', true, 1);
	pos   = Text_indexOfOrder(data, ':', true, 2);
	if (start > -1 && pos > -1) {
		port = Text_parsePortString(data, start, pos - start);
	} else {
		return;
	}

	// Responding
	if (ip > 0 && port > 0) {
		char* message = (char*) os_malloc(32);
		os_bzero(message, 32);
		if (verified) {
			os_sprintf(message, "%s:%d.%d.%d.%d:%d",
							CMD_VERIFY_PASS,
							IP2STR(&mCommandMulticastGroupIP), mCommandMulticastGroupPort);
		} else {
			os_sprintf(message, "%s:%d.%d.%d.%d:%d",
							CMD_VERIFY_FAIL,
							0, 0, 0, 0, 0);
		}

		sendUDPMessageOnce(ip, port, message);
	} else {
		return;
	}
}

LOCAL bool ICACHE_FLASH_ATTR sendUDPMessageOnce(uint32 ip, uint32 port, const char* message) {

}
