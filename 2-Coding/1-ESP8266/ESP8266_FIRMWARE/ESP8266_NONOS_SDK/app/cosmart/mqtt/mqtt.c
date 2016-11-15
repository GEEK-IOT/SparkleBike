/* mqtt.c
*  Protocol: http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "cosmart/mqtt/mqtt.h"
#include "cosmart/wifimanager.h"
#include "cosmart/timer.h"
#include "cosmart/config.h"
#include "user_interface.h"
#include "osapi.h"
#include "mem.h"

const char* MQTT_SUBSCRIBE_LIST[] = {
		"dev/oled12864/display"
};

#ifndef DELETE_POINTER
#define DELETE_POINTER(ptr) if (ptr != NULL) {os_free(ptr); ptr = NULL;}
#endif
#ifndef MQTT_Task
#define MQTT_Task onSystemTask
#endif

typedef struct espconn Connnection;

LOCAL void ping();
LOCAL void unping();
LOCAL void sendStream(MQTTClient* client);
LOCAL void onSystemTask(os_event_t *event);
LOCAL void onDNSResponding(const char* name, ip_addr_t* ip, void* args);
LOCAL void onTCPConnected(void* args);
LOCAL void onTCPReconnected(void* args, sint8 error);
LOCAL void onTCPDisconnected(void* args);
LOCAL void onTCPSent(void* args);
LOCAL void onTCPReceived(void *args, char *data, unsigned short length);
LOCAL void onPingTimeout(void *arg);

LOCAL os_event_t  mTaskQueue[MQTT_TASK_QUEUE_SIZE];
LOCAL MQTTClient* mClient    = NULL;
LOCAL Timer*      mPingTimer = NULL;

void ICACHE_FLASH_ATTR MQTT_initialize() {
	Log_printfln("");
	Log_printfln("[MQTT] MQTT initialized");

	mClient = (MQTTClient*) os_malloc(sizeof(MQTTClient));
	os_memset(mClient, NULL, sizeof(MQTTClient));
	MQTT_setService(MQTT_SERVER, MQTT_ENABLE_SSL ? MQTT_SSL_PORT : MQTT_PORT);
	MQTT_setConnectParameters(MQTT_PROTOCOL_LEVEL_3_1_1, MQTT_KEEP_ALIVE, MQTT_QoS, MQTT_KEEP_SESSOIN, MQTT_ENABLE_SSL);
	MQTT_setAuthentication(MQTT_getClientIndentifier(), MQTT_USERNAME, MQTT_PASSWORD);
	MQTT_setWill(MQTT_WILL_TOPIC, MQTT_WILL_MESSAGE);

	system_os_task(MQTT_Task, MQTT_TASK_PRIORITY, mTaskQueue, MQTT_TASK_QUEUE_SIZE);
}

const char* ICACHE_FLASH_ATTR MQTT_getClientIndentifier() {
	return WiFi_generateSTAIdentify();
}

void ICACHE_FLASH_ATTR MQTT_setService(const char* server, uint32 port) {
	mClient->server = server;
	mClient->port   = port;
}

void ICACHE_FLASH_ATTR MQTT_setConnectParameters(uint8 protocolLevel, uint16 keepAlive, uint8 QoS, bool isCleanSession,  bool isSSL) {
	mClient->protocolLevel  = MQTT_PROTOCOL_LEVEL_3_1_1;
	mClient->keepAlive      = keepAlive;
	mClient->QoS            = QoS;
	mClient->isCleanSession = isCleanSession;
	mClient->enabledSSL     = isSSL;
}

void ICACHE_FLASH_ATTR MQTT_setAuthentication(const char* clientId, const char* username, const char* password) {
	mClient->clientId = clientId;
	mClient->username = username;
	mClient->password = password;
}

void ICACHE_FLASH_ATTR MQTT_setWill(const char* willTopic, const char* willMessage) {
	mClient->willTopic   = willTopic;
	mClient->willMessage = willMessage;
}

LOCAL ICACHE_FLASH_ATTR void onSystemTask(os_event_t *event) {
	MQTTClient* client = (MQTTClient*)(event->par);
	if (client == NULL) {
		return;
	}

	switch (client->currentTask) {
		case MQTT_TASK_NONE:
			break;
		case MQTT_TASK_DNS: {
			Log_printf("[MQTT] Query host name %s", client->server);
			sint8 result = espconn_gethostbyname(client->connection, client->server, &client->hostIP, onDNSResponding);
			if (result == ESPCONN_OK) {
				Log_printfln(" - OK");
				onDNSResponding(client->server, &client->hostIP, (void*) client->connection);
			} else if (result == ESPCONN_INPROGRESS){
				client->currentTask = MQTT_TASK_DNSING;
				Log_printfln(" - doing");
			} else {
				Log_printfln(" - failed, result = %d", result);
			}
		} break;

		case MQTT_TASK_CONNECT: {
			client->currentTask = MQTT_TASK_CONNECTING;
			client->status = STATUS_CONNECTING;
			if (client->enabledSSL) {
				Log_printfln("[MQTT] Sent SSL connect to %s", client->server);
				espconn_secure_connect(client->connection);
			} else {
				Log_printfln("[MQTT] Sent TCP connect to %s", client->server);
				espconn_connect(client->connection);
			}
		} break;

		case MQTT_TASK_ABORT_CONNECTION: {
			espconn_disconnect(mClient->connection);
			espconn_delete(mClient->connection);
			DELETE_POINTER(mClient->connection->proto.tcp);
			DELETE_POINTER(mClient->connection);
			if (mClient->protocolStream != NULL) {
				DELETE_POINTER(mClient->protocolStream->fixedHeader);
				DELETE_POINTER(mClient->protocolStream->variableHeader);
				DELETE_POINTER(mClient->protocolStream->payload);
			}
			mClient->currentTask = MQTT_TASK_NONE;
		} break;

		case MQTT_TASK_RECONNECT: {
			espconn_disconnect(mClient->connection);
			espconn_delete(mClient->connection);
			DELETE_POINTER(mClient->connection->proto.tcp);
			DELETE_POINTER(mClient->connection);
			mClient->currentTask = MQTT_TASK_NONE;
			mClient->status = STATUS_IDLE;
			MQTT_reconnect();
		} break;
	}
}

LOCAL ICACHE_FLASH_ATTR void onDNSResponding(const char* name, ip_addr_t* ip, void* args) {
	struct espconn* connection = (struct espconn *)args;
	MQTTClient*     client     = (MQTTClient *)connection->reverse;

	if (client == NULL) {
		return;
	}
	if (ip == NULL) {
		Log_printfln("[MQTT] %s query failed, try again.", client->server);
		client->currentTask = MQTT_TASK_DNS;
		system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
		return;
	} else {
		uint8 ipAddress[4];
		os_memcpy(ipAddress, &ip->addr, 4);
		Log_printfln("[MQTT] Mapped %s to %d.%d.%d.%d",
				name, ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
		Log_printfln("[MQTT] Connecting %d.%d.%d.%d",
				ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3], mClient->port);

		// Connect TCP server
		os_memcpy(client->connection->proto.tcp->remote_ip, &ip->addr, 4);
		client->currentTask = MQTT_TASK_CONNECT;
		system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
	}
}

LOCAL ICACHE_FLASH_ATTR void onTCPConnected(void* args) {
	Log_printfln("[MQTT][onTCPConnected]");
	struct espconn* connection = (struct espconn *)args;
	MQTTClient*     client     = (MQTTClient *)connection->reverse;

	client->status = STATUS_IDLE;

	if (client->protocolStream != NULL) {
		DELETE_POINTER(client->protocolStream->fixedHeader);
		DELETE_POINTER(client->protocolStream->variableHeader);
		DELETE_POINTER(client->protocolStream->payload);
	}
	DELETE_POINTER(client->protocolStream);
	client->protocolStream = (ProtocolStream*) os_malloc(sizeof(ProtocolStream));
	os_memset(client->protocolStream, NULL, sizeof(ProtocolStream));
	MQTTProcotol_encodeConnectPacket(
			client->protocolStream,
			client->clientId,
			client->username,
			client->password,
			client->willTopic,
			client->willMessage,
			client->isCleanSession,
			client->keepAlive);

	sendStream(client);

	DELETE_POINTER(client->protocolStream->fixedHeader);
	DELETE_POINTER(client->protocolStream->variableHeader);
	DELETE_POINTER(client->protocolStream->payload);
	DELETE_POINTER(client->protocolStream);
}

LOCAL ICACHE_FLASH_ATTR void onTCPReconnected(void* args, sint8 error) {
	Log_printf("[MQTT][onTCPReconnected] Error: ");
	switch (error) {
		case ESPCONN_TIMEOUT: {
			Log_printfln("timeout");
			MQTT_reconnect();
		} break;

		case ESPCONN_ABRT: {
			Log_printfln("connection aborted");
			MQTT_reconnect();
		} break;

		case ESPCONN_RST: {
			Log_printfln("connection reset");
			MQTT_reconnect();
		} break;

		case ESPCONN_CLSD: {
			Log_printfln("connection closed");
			MQTT_reconnect();
		} break;

		case ESPCONN_CONN: {
			Log_printfln("not connected yet");
			// Not connected
		} break;

		case ESPCONN_HANDSHAKE: {
			Log_printfln("SSL handshake failed");
			MQTT_reconnect();
		} break;

		case ESPCONN_SSL_INVALID_DATA: {
			Log_printfln("SSL application invalid");
			MQTT_reconnect();
		} break;

		default: {
			Log_printfln("%d", error);
		} break;
	}
}

LOCAL ICACHE_FLASH_ATTR void onTCPDisconnected(void* args) {
	Log_printfln("[MQTT][onTCPDisconnected]");
	espconn_disconnect(mClient->connection);
	espconn_delete(mClient->connection);
	DELETE_POINTER(mClient->connection->proto.tcp);
	DELETE_POINTER(mClient->connection);

	if (mClient->status != STATUS_DISCONNECTING) {
		MQTT_reconnect();
	} else {
		mClient->status != STATUS_IDLE;
	}
}

LOCAL ICACHE_FLASH_ATTR void sendStream(MQTTClient* client) {
	if (client->status == STATUS_CONNECTING || client->status == STATUS_DISCONNECTING) {
		return;
	}

	mClient->status = STATUS_SENDING;
	uint8 buffer[client->protocolStream->encodedStreamLength];
	os_memcpy(buffer, client->protocolStream->encodedStream, client->protocolStream->encodedStreamLength);
	sint8 result = espconn_send(client->connection, buffer, client->protocolStream->encodedStreamLength);
	client->sendTimeCount = MQTT_SENT_TIMEOUT;
	if (result == ESPCONN_MAXNUM) {
		Log_printfln(" - Failed, buffer is full");
	} else {
		Log_printfln(" - OK");
		int i;
		Log_printf("       - ");
		for (i = 0; i < mClient->protocolStream->encodedStreamLength; i++) {
			Log_printf("%x ", *(mClient->protocolStream->encodedStream + i));
		}
		Log_printfln("[Total: %d]", mClient->protocolStream->encodedStreamLength);
	}
}

LOCAL ICACHE_FLASH_ATTR void onTCPSent(void* args) {
	Log_printfln("[MQTT][onTCPSent]");
	struct espconn* connection = (struct espconn *)args;
	MQTTClient*     client     = (MQTTClient *)connection->reverse;
	if (client->status == STATUS_SENDING) {
		client->status = STATUS_IDLE;
		client->keepAliveTimeCount = MQTT_PING_TIMEOUT;
	}
}

LOCAL ICACHE_FLASH_ATTR void onTCPReceived(void *args, char *data, unsigned short length) {
	Log_printf("[MQTT][onTCPReceived] data = ");
	int i;
	for (i = 0; i < length; i++) {
		Log_printf("%x ", *(data + i));
	}
	Log_printfln("[Total: %d]", length);

	struct espconn* connection = (struct espconn *)args;
	MQTTClient*     client     = (MQTTClient *)connection->reverse;

	if (client != NULL) {
		if (client->protocolStream != NULL) {
			DELETE_POINTER(client->protocolStream->fixedHeader);
			DELETE_POINTER(client->protocolStream->variableHeader);
			DELETE_POINTER(client->protocolStream->payload);
		}
		DELETE_POINTER(client->protocolStream);
	}

	uint8 packetType = *(data + 0);
	packetType = (packetType >> 4) & 0x0F;

	switch (packetType) {
		case MQTT_CONNECT_ACK: {
			Log_printfln("[MQTT] Received [CONNECT_ACK] from %d.%d.%d.%d:%d",
					connection->proto.tcp->remote_ip[0],
					connection->proto.tcp->remote_ip[1],
					connection->proto.tcp->remote_ip[2],
					connection->proto.tcp->remote_ip[3],
					connection->proto.tcp->remote_port);
			client->currentTask = MQTT_TASK_NONE;
			ping();
		} break;

		case MQTT_PING_ACK: {
			Log_printfln("[MQTT] Received [PING_ACK] from %d.%d.%d.%d:%d",
					connection->proto.tcp->remote_ip[0],
					connection->proto.tcp->remote_ip[1],
					connection->proto.tcp->remote_ip[2],
					connection->proto.tcp->remote_ip[3],
					connection->proto.tcp->remote_port);
			client->currentTask = MQTT_TASK_NONE;
		} break;
	}

}

LOCAL ICACHE_FLASH_ATTR void onPingTimeout(void *arg) {
	if (mClient->status == STATUS_SENDING) {
		mClient->sendTimeCount--;
		if (mClient->sendTimeCount < 0) {
			Log_printfln("[MQTT] Sending timeout, reconnect");
			unping();
			mClient->currentTask = MQTT_TASK_RECONNECT;
			system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
			return;
		}
	} else {
		mClient->sendTimeCount = 0;
	}
	mClient->keepAliveTimeCount--;

	if (mClient->keepAliveTimeCount > 0) {
		return;
	} else if (mClient->keepAliveTimeCount < 0) {
		mClient->keepAliveTimeCount = -1;
	}

	Log_printfln("[MQTT][ping] PingTimer = %d s, SendTimer = %d s", mClient->keepAliveTimeCount, mClient->sendTimeCount);
	if (mClient->currentTask == MQTT_TASK_NONE && mClient->status == STATUS_IDLE) {
		// Send PING
		if (mClient->protocolStream != NULL) {
			DELETE_POINTER(mClient->protocolStream->fixedHeader);
			DELETE_POINTER(mClient->protocolStream->variableHeader);
			DELETE_POINTER(mClient->protocolStream->payload);
		}
		DELETE_POINTER(mClient->protocolStream);
		mClient->protocolStream = (ProtocolStream*) os_malloc(sizeof(ProtocolStream));
		os_memset(mClient->protocolStream, NULL, sizeof(ProtocolStream));
		MQTTProcotol_encodePingPacket(mClient->protocolStream);

		Log_printf("[MQTT] Sent [PING] to %s:%d", mClient->server, mClient->port);
		sendStream(mClient);

		DELETE_POINTER(mClient->protocolStream->fixedHeader);
		DELETE_POINTER(mClient->protocolStream->variableHeader);
		DELETE_POINTER(mClient->protocolStream->payload);
		DELETE_POINTER(mClient->protocolStream);
	} else {
		Log_printfln("[MQTT] Task is busy, cancel ping this time");
	}
}

LOCAL ICACHE_FLASH_ATTR void ping() {
	if (mPingTimer == NULL) {
		mPingTimer = (Timer*) os_malloc(sizeof(Timer));
		os_memset(mPingTimer, NULL, sizeof(Timer));
		Timer_setCallback(mPingTimer, onPingTimeout);
		Timer_setInterval(mPingTimer, MQTT_PING_INTERVAL);
		Timer_setLoopable(mPingTimer, true);
		Timer_setTimerUnit(mPingTimer, MSTimer);
		Timer_start(mPingTimer);
	}
}

LOCAL ICACHE_FLASH_ATTR void unping() {
	if (mPingTimer != NULL) {
		Timer_stop(mPingTimer);
		os_free(mPingTimer);
		mPingTimer = NULL;
	}
}

void ICACHE_FLASH_ATTR MQTT_connect() {
	if (mClient->connection != NULL) {
		remot_info* remoteInfo = NULL;
		espconn_get_connection_info(mClient->connection, &remoteInfo, mClient->enabledSSL);
		if (remoteInfo != NULL) {
			if (remoteInfo->state != ESPCONN_NONE || remoteInfo->state != ESPCONN_CLOSE) {
				return;
			}
		}
	}

	if (mClient->connection == NULL) {
		mClient->connection = (Connnection* )os_zalloc(sizeof(Connnection));
	}
	mClient->connection->type  = ESPCONN_TCP;
	mClient->connection->state = ESPCONN_NONE;

	if (mClient->connection->proto.tcp == NULL) {
		mClient->connection->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	}
	mClient->connection->proto.tcp->local_port  = espconn_port();
	mClient->connection->proto.tcp->remote_port = mClient->port;
	mClient->connection->reverse = mClient;

	espconn_regist_connectcb(mClient->connection, onTCPConnected);
	espconn_regist_reconcb(mClient->connection, onTCPReconnected);
	espconn_regist_sentcb(mClient->connection, onTCPSent);
	espconn_regist_recvcb(mClient->connection, onTCPReceived);
	espconn_regist_disconcb(mClient->connection, onTCPDisconnected);

	Log_printfln("[MQTT] Connect to %s:%d", mClient->server, mClient->port);
	mClient->currentTask = MQTT_TASK_DNS;
	system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
}

void ICACHE_FLASH_ATTR MQTT_disconnect() {
	Log_printfln("[MQTT] Disconnect from %s:%d", mClient->server, mClient->port);

	unping();

	bool isWifiOK = wifi_station_get_connect_status() == STATION_GOT_IP;
	if (!(isWifiOK)) {
		mClient->currentTask = MQTT_TASK_ABORT_CONNECTION;
		system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
	} else {
		mClient->status = STATUS_DISCONNECTING;
		mClient->currentTask = MQTT_TASK_DISCONNECT;
		system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
	}
}

void ICACHE_FLASH_ATTR MQTT_reconnect() {
	unping();

	bool isWifiOK = wifi_station_get_connect_status() == STATION_GOT_IP;
	if (isWifiOK) {
		if (mClient->connection != NULL) {
			espconn_disconnect(mClient->connection);
			espconn_delete(mClient->connection);
			if (mClient->connection != NULL) {
				DELETE_POINTER(mClient->connection->proto.tcp);
			}
			DELETE_POINTER(mClient->connection);
		}

		MQTT_connect();
	}
}
