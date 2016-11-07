/*
 * mqtt.h
 *
 *  Created on: 2016/10/28
 *      Author: Cocoonshu
 *
 */

#ifndef APP_INCLUDE_COSMART_MQTT_MQTT_H_
#define APP_INCLUDE_COSMART_MQTT_MQTT_H_

#include "c_types.h"
#include "ip_addr.h"
#include "espconn.h"
#include "cosmart/mqtt/mqtt_protocol.h"

#define MQTT_TASK_SIGNAL          0x10
#define MQTT_TASK_PRIORITY        2
#define MQTT_TASK_QUEUE_SIZE      2
#define MQTT_TASK_NONE            0x00
#define MQTT_TASK_DNS             0x01
#define MQTT_TASK_DNSING          0x02
#define MQTT_TASK_CONNECT         0x03
#define MQTT_TASK_CONNECTING      0x04
#define MQTT_TASK_DISCONNECT      0x05
#define MQTT_TASK_DISCONNECTING   0x06

#define MQTT_PROTOCOL_LEVEL_3_1_1 0x04
#define MAX_REMAIN_LENGTH         268435455
#define MQTT_NAME                 "MQTT"
#define MQTT_SERVER               "45.55.5.8" // "m13.cloudmqtt.com"
#define MQTT_PORT                 1883 // 10419
#define MQTT_SSL_PORT             20419
#define MQTT_KEEP_ALIVE           120
#define MQTT_QoS                  MQTT_QoS_AT_LEAST_ONCE
#define MQTT_ENABLE_SSL           false
#define MQTT_KEEP_SESSOIN         true
#define MQTT_WILL_TOPIC           "topic/dev/offline"
#define MQTT_WILL_MESSAGE         "000000"
#define MQTT_USERNAME             "admin" // "ctytmovg"
#define MQTT_PASSWORD             "admin" // "FI4fkpUG97nx"

enum QoS {
	MQTT_QoS_AT_MOST_ONCE  = 0x00,
	MQTT_QoS_AT_LEAST_ONCE = 0x01,
	MQTT_QoS_ONLY_ONCE     = 0x02
};

enum ResponseCode {
	RES_CODE_CONNECT_COMFIRMED            = 0x00,
	RES_CODE_UNSUPPORTED_PROTOCOL_LEVEL   = 0x01,
	RES_CODE_ILLEGAL_CLIENT_IDENTIFIER    = 0x02,
	RES_CODE_SERVER_INVALIDATED           = 0x03,
	RES_CODE_INVALID_USERNAME_OR_PASSWORD = 0x04,
	RES_CODE_UNAUTHORIZATED               = 0x05
};

enum PacketType {
	MQTT_CONNECT           = 1,
	MQTT_CONNECT_ACK       = 2,
	MQTT_PUBLISH           = 3,  // packet identifier
	MQTT_PUBLISH_ACK       = 4,  // packet identifier
	MQTT_PUBLISH_RECEVIED  = 5,  // packet identifier
	MQTT_PUBLISH_RELEASE   = 6,  // packet identifier
	MQTT_PUBLISH_COMPLETED = 7,  // packet identifier
	MQTT_SUBSCRIBE         = 8,  // packet identifier
	MQTT_SUBSCRIBE_ACK     = 9,  // packet identifier
	MQTT_UNSUBSCRIBE       = 10, // packet identifier
	MQTT_UNSUBSCRIBE_ACK   = 11, // packet identifier
	MQTT_PING              = 12,
	MQTT_PING_ACK          = 13,
	MQTT_DISCONNECT        = 14
};

typedef struct {
	const char* clientId;
	const char* username;
	const char* password;
	const char* server;
	uint32      port;
	uint8       protocolLevel;
	uint16      keepAlive;
	uint8       QoS;
	bool        isCleanSession;
	bool        enabledSSL;
	const char* willTopic;
	const char* willMessage;

	struct espconn* connection;
	ip_addr_t       hostIP;
	uint8           currentTask;
	ProtocolStream* protocolStream;
} MQTTClient;

typedef void OnMessageReceived(const char* topic, const char* message);

void        MQTT_initialize();
void        MQTT_setService(const char* server, uint32 port);
void        MQTT_setConnectParameters(uint8 protocolLevel, uint16 keepAlive, uint8 QoS, bool isCleanSession, bool isSSL);
void        MQTT_setAuthentication(const char* clientID, const char* username, const char* password);
void        MQTT_setWill(const char* willTopic, const char* willMessage);
void        MQTT_connect();
void        MQTT_disconnect();
void        MQTT_subscribe();
void        MQTT_unsubscribe();
void        MQTT_publish(const char* topic, const char* message);
void        MQTT_setOnReceivedCallback(OnMessageReceived* listener);
const char* MQTT_getClientIndentifier();


#endif /* APP_INCLUDE_COSMART_MQTT_MQTT_H_ */
