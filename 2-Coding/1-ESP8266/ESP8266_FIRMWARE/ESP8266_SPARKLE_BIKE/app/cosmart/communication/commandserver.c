/*
 * commandserver.c
 *
 *  Created on: 2015年9月22日
 *      Author: Cocoonshu
 */

#include "cosmart/communication/commandserver.h"
#include "cosmart/communication/commandqueue.h"
#include "cosmart/communication/commandexecutor.h"
#include "cosmart/communication/cmdprotocol.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cosmart/config.h"
#include "cosmart/log.h"

LOCAL void          startBroadcastReceiver();
LOCAL void          stopBroadcastReceiver();
LOCAL void          startCommandQueue();
LOCAL void          stopCommandQueue();
LOCAL bool          moveTerminalToConnectedList(Terminal* terminal);
LOCAL bool          isConnected(Terminal* terminal);
LOCAL unsigned long getGenerationID();
LOCAL void          onBroadcastReceived(void *espconn, char *pdata, unsigned short length);
LOCAL void          onUDPBridgeReceived(void *espconn, char *pdata, unsigned short length);
LOCAL void          onUDPBridgeConnected(void *espconn);
LOCAL void          onCommand(Terminal* terminal, const char* command);
LOCAL void          onMethod(Terminal* terminal, const char* command, Request* inRequest, Response* outResponse);
LOCAL void          onData(Terminal* terminal, const char* command, Request* inRequest, Response* outResponse);

// 接入的Terminal列表
LOCAL Terminal*     mConnectingTerminalList[MAX_TERMINAL_SIZE];
LOCAL Terminal*     mConnectedTerminalList[MAX_TERMINAL_SIZE];
LOCAL unsigned long mGenerationCounter;

// UDP广播接收器
LOCAL struct espconn mBroadcastReceiverConnection;

void ICACHE_FLASH_ATTR CMDServer_initialize() {
	Log_printfln("");
	Log_printfln("[CMD] Start command server");

	int i = 0;
	for (; i < MAX_TERMINAL_SIZE; i++) {
		mConnectingTerminalList[i] = NULL;
		mConnectedTerminalList[i]  = NULL;
	}
	mGenerationCounter = 0;
}

void ICACHE_FLASH_ATTR CMDServer_startCommandServer() {
	startCommandQueue();
	startBroadcastReceiver();
}

void ICACHE_FLASH_ATTR CMDServer_stopCommandServer() {
	stopBroadcastReceiver();
	stopCommandQueue();
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

LOCAL void ICACHE_FLASH_ATTR startCommandQueue() {
	CMDQueue_initialize();
	CMDQueue_setCommandListener(onCommand);
	CMDQueue_startLoop();
}

LOCAL void ICACHE_FLASH_ATTR stopCommandQueue() {
	CMDQueue_setCommandListener(NULL);
	CMDQueue_stopLoop();
	CMDQueue_clear();
}

LOCAL bool moveTerminalToConnectedList(Terminal* terminal) {
//	if (terminal != NULL) {
//		int i;
//		int foundPosition = -1;
//		for (i = 0; i < MAX_TERMINAL_SIZE; i++) {
//			foundPosition = mConnectingTerminalList[i] == terminal ? i : foundPosition;
//			if (foundPosition != -1) {
//				break;
//			}
//		}
//
//		int  emptyPosition   = -1;
//		bool hasSameTerminal = false;
//		for (i = 0; i < MAX_TERMINAL_SIZE; i++) {
//			if (emptyPosition == -1) {
//				emptyPosition = mConnectedTerminalList[i] == NULL ? i : emptyPosition;
//			}
//			if (mConnectedTerminalList[i] == terminal) {
//				hasSameTerminal = true;
//				break;
//			}
//		}
//		if (hasSameTerminal) {
//			mConnectingTerminalList[foundPosition] = NULL;
//			return true;
//		} else {
//			if (emptyPosition != -1) {
//				mConnectingTerminalList[foundPosition] = NULL;
//				mConnectedTerminalList[emptyPosition]  = terminal;
//				return true;
//			} else {
//				// 已连接列表中没有空位，断开最老连接
//			}
//		}
//	}
}

LOCAL bool ICACHE_FLASH_ATTR isConnected(Terminal* terminal) {
//	if (terminal != NULL) {
//		Connection* connection = terminal->connection;
//		if (connection != NULL) {
//			// 因为UDP连接获取的remote_info.state是始终为ESPCONN_NONE
//			// 所以在此只判断Terminal是否处于mConnectedList中
//			// espconn_get_connection_info(connection, &info, 0);
//			int i;
//			for (i = 0; i < MAX_TERMINAL_SIZE; i++) {
//				if (mConnectedTerminalList[i] == terminal) {
//					return true;
//				}
//			}
//		}
//	}
//	return false;
}

LOCAL unsigned long ICACHE_FLASH_ATTR getGenerationID() {
//	mGenerationCounter ++;
//	if (mGenerationCounter == 0) {
//		int i;
//		for (i = 0; i < MAX_TERMINAL_SIZE; i++) {
//			if (mConnectingTerminalList[i] != NULL) {
//				mConnectingTerminalList[i]->generation = mGenerationCounter;
//			}
//			if (mConnectedTerminalList[i] != NULL) {
//				mConnectedTerminalList[i]->generation = mGenerationCounter;
//			}
//		}
//	}
//	return mGenerationCounter;
}

LOCAL void ICACHE_FLASH_ATTR onBroadcastReceived(void *espconn, char *data, unsigned short length) {
	/**
	 * Broadcast Message protocol:
	 *   #Version1:
	 *     [BROADCAST_IP]:[BROADCAST_PORT]:[cosmart.PLATFORM.NODETYPE]:[CMD_PORT]
	 *     192.168.4.2:7629:cosmart.android.commander:7630
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
	// 提交一条指令到命令队列中等待执行
	CMDQueue_push(NULL, data);
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

LOCAL void ICACHE_FLASH_ATTR onCommand(Terminal* terminal, const char* command) {
//	if (command == NULL) {
//		return;
//	}
//
//	Request*  request  = NULL;
//	Response* response = NULL;
//	if (CMDDecoder_command(command, &request, &response)) {
//		return;
//	}
//
//	if (request == NULL) {
//		if (response == NULL) {
//			Log_printfln("[CMD][onCommand][OutOfMemory] Cannot alloc Response object");
//			return;
//		} else {
//			Log_printfln("[CMD][onCommand][OutOfMemory] Cannot alloc Request object");
//			response->requestState->stateCode        = State_OutOfMemoryError;
//			response->requestState->stateMessage     = NULL;
//			response->requestState->stateDescription = NULL;
//		}
//	} else {
//		onMethod(terminal, command, request, response);
//	}
}

LOCAL void ICACHE_FLASH_ATTR onMethod(Terminal* terminal, const char* command, Request* inRequest, Response* outResponse) {
//	// 分配方法调用
//	int           i          = 0;
//	const int     methodSize = CMDExecutor_getMethodSize();
//	const Method* methods    = CMDExecutor_getMethods();
//	bool foundExecutor = false;
//	for (i = 0; i < methodSize; i++) {
//		Method method = methods[i];
//		if (os_strcmp(method.name, inRequest->method)) {
//			if (method.decoder(command, inRequest, outResponse)) {
//				method.executor(inRequest, outResponse);
//				if (terminal != NULL) {
//					char* packet = method.encoder(inRequest, outResponse);
//					if (terminal->connection != NULL) {
//						espconn_sendto(terminal->connection, packet, os_strlen(packet));
//						FREEMEM(packet); // FIXME Should FREEMEM packet string here ?
//					}
//				}
//			} else {
//				// 指令解析失败
//				Log_printfln("[CMD][onMethod][IncompletedRequestException] method is %s", inRequest->method);
//				outResponse->requestState->stateCode        = State_IncompletedRequestException;
//				outResponse->requestState->stateMessage     = NULL;
//				outResponse->requestState->stateDescription = inRequest->method;
//			}
//			foundExecutor = true;
//			break;
//		}
//	}
//
//	if (!foundExecutor) {
//		// 不支持的方法
//		Log_printfln("[CMD][onMethod][UnimplementedMethodException] method is %s", inRequest->method);
//		outResponse->requestState->stateCode        = State_UnimplementedMethodException;
//		outResponse->requestState->stateMessage     = NULL;
//		outResponse->requestState->stateDescription = inRequest->method;
//	}
}

LOCAL void ICACHE_FLASH_ATTR onData(Terminal* terminal, const char* command, Request* inRequest, Response* outResponse) {
	// 分配数据处理
	// # if
	// 未知数据格式
	// # else
}
