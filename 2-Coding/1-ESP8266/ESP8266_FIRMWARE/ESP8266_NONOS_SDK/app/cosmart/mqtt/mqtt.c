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

typedef struct espconn Connnection;

LOCAL void onDNSResponding(const char* name, ip_addr_t* ip, void* args);

LOCAL MQTTClient* mClient = NULL;

void ICACHE_FLASH_ATTR MQTT_initialize() {
	Log_printfln("");
	Log_printfln("[MQTT] MQTT initialized");

	mClient = (MQTTClient*) os_malloc(sizeof(MQTTClient));
	os_memset(mClient, NULL, sizeof(MQTTClient));

	MQTT_setService(MQTT_SERVER, MQTT_PORT);
	MQTT_setConnectParameters(MQTT_KEEP_ALIVE, MQTT_QoS, MQTT_RETAIN_MESSAGE);
	MQTT_setAuthentication(MQTT_getClientIndentifier(), MQTT_USER, MQTT_PASSWORD);
}

void ICACHE_FLASH_ATTR MQTT_setService(const char* server, uint32 port) {
	mClient->server = server;
	mClient->port   = port;

	if (mClient->connection == NULL) {
		mClient->connection          = (Connnection* )os_malloc(sizeof(Connnection));
		mClient->connection->type    = ESPCONN_TCP;
		mClient->connection->state   = ESPCONN_NONE;
		mClient->connection->reverse = mClient;
	}
	if (mClient->connection->proto.tcp == NULL) {
		mClient->connection->proto.tcp = (esp_tcp *)os_malloc(sizeof(esp_tcp));
	}
	mClient->connection->proto.tcp->local_port  = espconn_port();
	mClient->connection->proto.tcp->remote_port = mClient->port;

	espconn_gethostbyname(&mClient->connection, mClient->server, &mClient->hostIP, onDNSResponding);
}

void ICACHE_FLASH_ATTR MQTT_setConnectParameters(uint8 protocolLevel, uint16 keepAlive, uint8 QoS, bool isRetain, bool isSSL) {
	mClient->protocolLevel = PROTOCOL_LEVEL_3_1_1;
	mClient->keepAlive     = keepAlive;
	mClient->QoS           = QoS;
	mClient->isRetain      = isRetain;
	mClient->enabledSSL    = isSSL;
}

void ICACHE_FLASH_ATTR MQTT_setAuthentication(const char* clientId, const char* username, const char* password) {
	mClient->clientId = clientId;
	mClient->username = username;
	mClient->password = password;
}

LOCAL ICACHE_FLASH_ATTR void onDNSResponding(const char* name, ip_addr_t* ip, void* args) {
	Log_printfln("[MQTT] mapped %s to %03d.%03d.%03d.%03d",
			name, *((uint8*)ip->addr), *((uint8*)ip->addr + 1), *((uint8*)ip->addr + 2), *((uint8*)ip->addr + 3));

	struct espconn *connection = (struct espconn *)args;
	MQTTClient* client = (MQTTClient *)connection->reverse;
}

void ICACHE_FLASH_ATTR MQTT_connect() {}
void ICACHE_FLASH_ATTR MQTT_disconnect() {}
