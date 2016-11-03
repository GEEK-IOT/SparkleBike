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
#include "cosmart/config.h"
#include "user_interface.h"
#include "mem.h"

#ifndef DELETE_POINTER
#define DELETE_POINTER(ptr) if (ptr != NULL) {os_free(ptr); ptr = NULL;}
#endif
#ifndef MQTT_Task
#define MQTT_Task onSystemTask
#endif

typedef struct espconn Connnection;

LOCAL void onSystemTask(os_event_t *event);
LOCAL void onDNSResponding(const char* name, ip_addr_t* ip, void* args);
LOCAL void onTCPConnected(void* args);
LOCAL void onTCPReconnected(void* args, sint8 error);
LOCAL void onTCPDisconnected(void* args);
LOCAL void onTCPReceived(void *args, char *data, unsigned short length);

LOCAL os_event_t  mTaskQueue[MQTT_TASK_QUEUE_SIZE];
LOCAL MQTTClient* mClient = NULL;

void ICACHE_FLASH_ATTR MQTT_initialize() {
	Log_printfln("");
	Log_printfln("[MQTT] MQTT initialized");

	mClient = (MQTTClient*) os_malloc(sizeof(MQTTClient));
	os_memset(mClient, NULL, sizeof(MQTTClient));
	MQTT_setService(MQTT_SERVER, MQTT_PORT);
	MQTT_setConnectParameters(MQTT_PROTOCOL_LEVEL_3_1_1, MQTT_KEEP_ALIVE, MQTT_QoS, MQTT_KEEP_SESSOIN, MQTT_ENABLE_SSL);
	MQTT_setAuthentication(MQTT_getClientIndentifier(), MQTT_USERNAME, MQTT_PASSWORD);
	MQTT_setWill(MQTT_WILL_TOPIC, MQTT_WILL_MESSAGE);

	system_os_task(MQTT_Task, MQTT_TASK_PRIORITY, mTaskQueue, MQTT_TASK_QUEUE_SIZE);
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
			espconn_gethostbyname(client->connection, client->server, &client->hostIP, onDNSResponding);
			client->currentTask = MQTT_TASK_DNSING;
		} break;
		case MQTT_TASK_CONNECT: {
			client->currentTask = MQTT_TASK_CONNECTING;
			if (client->enabledSSL) {
				espconn_secure_connect(client->connection);
			} else {
				espconn_connect(client->connection);
			}
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
		Log_printfln("[MQTT] mapped %s to %03d.%03d.%03d.%03d",
				name, *((uint8*)ip->addr), *((uint8*)ip->addr + 1), *((uint8*)ip->addr + 2), *((uint8*)ip->addr + 3));
		Log_printfln("[MQTT] Connecting %03d.%03d.%03d.%03d:%d",
				*((uint8*)ip->addr), *((uint8*)ip->addr + 1), *((uint8*)ip->addr + 2), *((uint8*)ip->addr + 3),
				mClient->port);

		// Connect TCP server
		os_memcpy(client->connection->proto.tcp->remote_ip, &ip->addr, 4);
		client->currentTask = MQTT_TASK_CONNECT;
		system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
	}
}

LOCAL ICACHE_FLASH_ATTR void onTCPConnected(void* args) {
	struct espconn* connection = (struct espconn *)args;
	MQTTClient*     client     = (MQTTClient *)connection->reverse;

	DELETE_POINTER(client->protocolStream->fixedHeader);
	DELETE_POINTER(client->protocolStream->variableHeader);
	DELETE_POINTER(client->protocolStream->payload);
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

	espconn_send(client->connection,
			client->protocolStream->encodedStream, client->protocolStream->encodedStreamLength);
	Log_printfln("[MQTT] Sent [CONNECT] to %s:%d", mClient->server, mClient->port);
	DELETE_POINTER(client->protocolStream->fixedHeader);
		DELETE_POINTER(client->protocolStream->variableHeader);
		DELETE_POINTER(client->protocolStream->payload);
		DELETE_POINTER(client->protocolStream);
}

LOCAL ICACHE_FLASH_ATTR void onTCPReconnected(void* args, sint8 error) {

}

LOCAL ICACHE_FLASH_ATTR void onTCPDisconnected(void* args) {

}

LOCAL ICACHE_FLASH_ATTR void onTCPReceived(void *args, char *data, unsigned short length) {
	struct espconn* connection = (struct espconn *)args;
	MQTTClient*     client     = (MQTTClient *)connection->reverse;

	DELETE_POINTER(client->protocolStream->fixedHeader);
	DELETE_POINTER(client->protocolStream->variableHeader);
	DELETE_POINTER(client->protocolStream->payload);
	DELETE_POINTER(client->protocolStream);
//	client->protocolStream = (ProtocolStream*) os_malloc(sizeof(ProtocolStream));
//	client->protocolStream->fixedHeader = (FixHeader*) os_malloc(sizeof(FixHeader));
//	client->protocolStream->fixedHeaderLength = sizeof(FixHeader);
//	os_memet(client->protocolStream->fixedHeader, NULL, client->protocolStream->fixedHeaderLength);
//	os_memcpy(client->protocolStream->fixedHeader, data, client->protocolStream->fixedHeaderLength);

	FixHeader* header = (FixHeader*) os_malloc(sizeof(FixHeader));
	os_memet(header, 0, sizeof(FixHeader));
	os_memcpy(header, data, sizeof(FixHeader));

	switch (header->packetType) {
		case MQTT_CONNECT_ACK: {
			Log_printfln("[MQTT] Received [CONNECT_ACK] from %d.%d.%d.%d:%d",
					connection->proto.tcp->remote_ip[0],
					connection->proto.tcp->remote_ip[1],
					connection->proto.tcp->remote_ip[2],
					connection->proto.tcp->remote_ip[3],
					connection->proto.tcp->remote_port);
		} break;
	}

	DELETE_POINTER(header);
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
		mClient->connection = (Connnection* )os_malloc(sizeof(Connnection));
	}
	mClient->connection->type    = ESPCONN_TCP;
	mClient->connection->state   = ESPCONN_NONE;
	mClient->connection->reverse = mClient;

	if (mClient->connection->proto.tcp == NULL) {
		mClient->connection->proto.tcp = (esp_tcp *)os_malloc(sizeof(esp_tcp));
	}
	mClient->connection->proto.tcp->local_port  = espconn_port();
	mClient->connection->proto.tcp->remote_port = mClient->port;

	espconn_regist_connectcb(mClient->connection, onTCPConnected);
	espconn_regist_reconcb(mClient->connection, onTCPReconnected);
	espconn_regist_recvcb(mClient->connection, onTCPReceived);
	espconn_regist_disconcb(mClient->connection, onTCPDisconnected);

	Log_printfln("[MQTT] Connect to %s:%d", mClient->server, mClient->port);
    system_os_post(MQTT_TASK_PRIORITY, MQTT_TASK_SIGNAL, (os_param_t)mClient);
}
void ICACHE_FLASH_ATTR MQTT_disconnect() {}
